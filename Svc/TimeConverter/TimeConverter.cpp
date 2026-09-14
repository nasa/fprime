// ======================================================================
// \title  TimeConverter.cpp
// \author devin
// \brief  cpp file for TimeConverter component implementation class
// ======================================================================

#include "Svc/TimeConverter/TimeConverter.hpp"

namespace Svc {

constexpr I64 TimeConverter::US_PER_SECOND;
constexpr I64 TimeConverter::MAX_TIME_US;

namespace {
//! Numeric value of a time base, used to order a pair canonically
FwTimeBaseStoreType timeBaseValue(const TimeBase& timeBase) {
    return static_cast<FwTimeBaseStoreType>(timeBase.e);
}

//! A time base denotes a clock, rather than the absence of one or a wildcard
bool usableTimeBase(const TimeBase& timeBase) {
    return (timeBase.e != TimeBase::TB_NONE) && (timeBase.e != TimeBase::TB_DONT_CARE);
}

//! A pair of time bases in canonical order, lesser numeric value first
Svc::TimeBasePair canonicalPair(const TimeBase& from, const TimeBase& to) {
    const bool ascending = timeBaseValue(from) < timeBaseValue(to);
    return Svc::TimeBasePair(ascending ? from : to, ascending ? to : from);
}
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TimeConverter ::TimeConverter(const char* const compName) : TimeConverterComponentBase(compName) {}

TimeConverter ::~TimeConverter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TimeConverter ::offsetUpdate_handler(FwIndexType portNum, const Svc::TimeOffset& offset) {
    // storeOffset reports every failure by event, and the port carries no status back
    (void)this->storeOffset(offset.get_from(), offset.get_to(), offset.get_offset_us());
}

Svc::ConvertTimeStatus TimeConverter ::convertTime_handler(FwIndexType portNum,
                                                           const Fw::Time& in_time,
                                                           Fw::Time& out_time) {
    const TimeBase in_tb = in_time.getTimeBase();
    const TimeBase out_tb = out_time.getTimeBase();

    if (this->checkTimeBases(in_tb, out_tb) != Fw::Success::SUCCESS) {
        return Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE;
    }

    // A time is already in the requested base: no offset is needed
    if (timeBaseValue(in_tb) == timeBaseValue(out_tb)) {
        out_time = in_time;
        return Svc::ConvertTimeStatus::OK;
    }

    I64 offset_us = 0;
    if (this->lookupOffset(in_tb, out_tb, offset_us) != Fw::Success::SUCCESS) {
        this->log_WARNING_LO_NoConversionAvailable(in_tb, out_tb);
        return Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE;
    }

    // Offsets are range-checked when stored, so this addition cannot overflow
    const I64 in_time_us = static_cast<I64>(in_time.getSeconds()) * US_PER_SECOND + in_time.getUSeconds();
    const I64 out_time_us = in_time_us + offset_us;
    if ((out_time_us < 0) || (out_time_us > MAX_TIME_US)) {
        this->log_WARNING_LO_InvalidTime(in_tb, out_tb, offset_us, in_time_us);
        return Svc::ConvertTimeStatus::INVALID_TIME;
    }

    out_time.set(out_tb, in_time.getContext(), static_cast<U32>(out_time_us / US_PER_SECOND),
                 static_cast<U32>(out_time_us % US_PER_SECOND));
    return Svc::ConvertTimeStatus::OK;
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void TimeConverter ::SET_OFFSET_cmdHandler(FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const TimeBase& from,
                                           const TimeBase& to,
                                           I64 offset_us) {
    Fw::CmdResponse response = Fw::CmdResponse::OK;
    switch (this->storeOffset(from, to, offset_us)) {
        case StoreStatus::OK:
            this->log_ACTIVITY_HI_OffsetSet(from, to, offset_us);
            break;
        // A full table is a state the operator clears, not a bad argument
        case StoreStatus::TABLE_FULL:
            response = Fw::CmdResponse::EXECUTION_ERROR;
            break;
        case StoreStatus::INVALID:
            response = Fw::CmdResponse::VALIDATION_ERROR;
            break;
    }
    this->cmdResponse_out(opCode, cmdSeq, response);
}

void TimeConverter ::CLEAR_OFFSETS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    const FwSizeType cleared = this->m_offsets.getSize();
    this->m_offsets.clear();
    // Clearing the table is the operator's recovery action, so re-arm the throttled warnings
    this->log_WARNING_LO_NoConversionAvailable_ThrottleClear();
    this->log_WARNING_LO_InvalidTime_ThrottleClear();
    this->log_WARNING_LO_IdenticalTimeBases_ThrottleClear();
    this->log_WARNING_HI_OffsetOutOfRange_ThrottleClear();
    this->log_WARNING_HI_OffsetTableFull_ThrottleClear();
    this->log_WARNING_HI_UnusableTimeBase_ThrottleClear();
    this->log_ACTIVITY_HI_OffsetsCleared(static_cast<U32>(cleared));
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverter ::GET_OFFSET_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const TimeBase& from, const TimeBase& to) {
    if (this->checkTimeBases(from, to) != Fw::Success::SUCCESS) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    if (timeBaseValue(from) == timeBaseValue(to)) {
        this->log_WARNING_LO_IdenticalTimeBases(from);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    I64 offset_us = 0;
    if (this->lookupOffset(from, to, offset_us) != Fw::Success::SUCCESS) {
        this->log_WARNING_LO_NoOffsetStored(from, to);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->log_ACTIVITY_HI_OffsetReport(from, to, offset_us);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverter ::DUMP_OFFSETS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    if (this->m_offsets.getSize() == 0) {
        this->log_ACTIVITY_HI_OffsetTableEmpty();
    }
    for (auto it = this->m_offsets.begin(); it != this->m_offsets.end(); ++it) {
        const Svc::TimeBasePair& pair = it->getKey();
        this->log_ACTIVITY_HI_OffsetReport(pair.get_lower(), pair.get_upper(), it->getValue());
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Fw::Success TimeConverter ::lookupOffset(const TimeBase& from, const TimeBase& to, I64& offset_us) const {
    I64 stored_us = 0;
    const Fw::Success status = this->m_offsets.find(canonicalPair(from, to), stored_us);
    if (status == Fw::Success::SUCCESS) {
        // Offsets are stored in canonical order; the reverse conversion negates them
        offset_us = (timeBaseValue(from) < timeBaseValue(to)) ? stored_us : -stored_us;
    }
    return status;
}

TimeConverter::StoreStatus TimeConverter ::storeOffset(const TimeBase& from, const TimeBase& to, I64 offset_us) {
    if (this->checkTimeBases(from, to) != Fw::Success::SUCCESS) {
        return StoreStatus::INVALID;
    }
    if (timeBaseValue(from) == timeBaseValue(to)) {
        this->log_WARNING_LO_IdenticalTimeBases(from);
        return StoreStatus::INVALID;
    }
    // Bound stored offsets so that applying or negating one cannot overflow
    if ((offset_us > MAX_TIME_US) || (offset_us < -MAX_TIME_US)) {
        this->log_WARNING_HI_OffsetOutOfRange(from, to, offset_us);
        return StoreStatus::INVALID;
    }

    const bool ascending = timeBaseValue(from) < timeBaseValue(to);
    const I64 stored_us = ascending ? offset_us : -offset_us;

    // An insert replaces the entry for a pair already stored, and fails only when the table is full
    if (this->m_offsets.insert(canonicalPair(from, to), stored_us) != Fw::Success::SUCCESS) {
        this->log_WARNING_HI_OffsetTableFull(from, to, static_cast<U32>(Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES));
        return StoreStatus::TABLE_FULL;
    }
    return StoreStatus::OK;
}

Fw::Success TimeConverter ::checkTimeBases(const TimeBase& from, const TimeBase& to) {
    // TB_NONE and TB_DONT_CARE name no clock, so no offset relates them to one
    Fw::Success status = Fw::Success::SUCCESS;
    if (!usableTimeBase(from)) {
        this->log_WARNING_HI_UnusableTimeBase(from);
        status = Fw::Success::FAILURE;
    }
    if (!usableTimeBase(to)) {
        this->log_WARNING_HI_UnusableTimeBase(to);
        status = Fw::Success::FAILURE;
    }
    return status;
}

}  // namespace Svc
