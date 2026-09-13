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
constexpr I64 US_PER_SECOND = TimeConverter::US_PER_SECOND;
constexpr I64 MAX_TIME_US = TimeConverter::MAX_TIME_US;

//! Bits holding one time base in a table key
constexpr U8 TIME_BASE_BITS = sizeof(FwTimeBaseStoreType) * 8;
static_assert(2 * TIME_BASE_BITS <= 64, "a pair of time bases must fit in a table key");

//! Numeric value of a time base, used to order a pair canonically
FwTimeBaseStoreType timeBaseValue(const TimeBase& timeBase) {
    return static_cast<FwTimeBaseStoreType>(timeBase.e);
}

//! Table key holding a pair of time bases, lesser value first
U64 pairKey(const TimeBase& lower, const TimeBase& upper) {
    return (static_cast<U64>(timeBaseValue(lower)) << TIME_BASE_BITS) | static_cast<U64>(timeBaseValue(upper));
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

    // A time is already in the requested base: no offset is needed
    if (timeBaseValue(in_tb) == timeBaseValue(out_tb)) {
        out_time = in_time;
        return Svc::ConvertTimeStatus::OK;
    }

    I64 offset_us = 0;
    if (!this->lookupOffset(in_tb, out_tb, offset_us)) {
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
    this->log_ACTIVITY_HI_OffsetsCleared(static_cast<U32>(cleared));
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverter ::GET_OFFSET_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const TimeBase& from, const TimeBase& to) {
    if (timeBaseValue(from) == timeBaseValue(to)) {
        this->log_WARNING_LO_IdenticalTimeBases(from);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    I64 offset_us = 0;
    if (!this->lookupOffset(from, to, offset_us)) {
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
        const OffsetEntry& entry = it->getValue();
        this->log_ACTIVITY_HI_OffsetReport(entry.lower, entry.upper, entry.offset_us);
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

bool TimeConverter ::lookupOffset(const TimeBase& from, const TimeBase& to, I64& offset_us) const {
    const bool ascending = timeBaseValue(from) < timeBaseValue(to);
    OffsetEntry entry;
    if (this->m_offsets.find(pairKey(ascending ? from : to, ascending ? to : from), entry) != Fw::Success::SUCCESS) {
        return false;
    }
    // Entries hold the offset in canonical order; the reverse conversion negates it
    offset_us = ascending ? entry.offset_us : -entry.offset_us;
    return true;
}

TimeConverter::StoreStatus TimeConverter ::storeOffset(const TimeBase& from, const TimeBase& to, I64 offset_us) {
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
    OffsetEntry entry;
    entry.lower = ascending ? from : to;
    entry.upper = ascending ? to : from;
    entry.offset_us = ascending ? offset_us : -offset_us;

    // An insert replaces the entry for a pair already stored, and fails only when the table is full
    if (this->m_offsets.insert(pairKey(entry.lower, entry.upper), entry) != Fw::Success::SUCCESS) {
        this->log_WARNING_HI_OffsetTableFull(from, to, static_cast<U32>(Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES));
        return StoreStatus::TABLE_FULL;
    }
    return StoreStatus::OK;
}

}  // namespace Svc
