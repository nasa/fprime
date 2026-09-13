// ======================================================================
// \title  TimeConverter.cpp
// \author devin
// \brief  cpp file for TimeConverter component implementation class
// ======================================================================

#include "Svc/TimeConverter/TimeConverter.hpp"

#include <limits>

namespace Svc {

namespace {
//! Microseconds in one second
constexpr I64 US_PER_SECOND = 1000000;

//! Largest time representable by an Fw::Time, in microseconds
constexpr I64 MAX_TIME_US = static_cast<I64>(std::numeric_limits<U32>::max()) * US_PER_SECOND + (US_PER_SECOND - 1);

//! Numeric value of a time base, used to order a pair canonically
FwTimeBaseStoreType timeBaseValue(const TimeBase& timeBase) {
    return static_cast<FwTimeBaseStoreType>(timeBase.e);
}
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TimeConverter ::TimeConverter(const char* const compName) : TimeConverterComponentBase(compName), m_offsetCount(0) {}

TimeConverter ::~TimeConverter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TimeConverter ::offsetUpdate_handler(FwIndexType portNum, const Svc::TimeOffset& offset) {
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
    const bool stored = this->storeOffset(from, to, offset_us);
    if (stored) {
        this->log_ACTIVITY_HI_OffsetSet(from, to, offset_us);
    }
    this->cmdResponse_out(opCode, cmdSeq, stored ? Fw::CmdResponse::OK : Fw::CmdResponse::VALIDATION_ERROR);
}

void TimeConverter ::CLEAR_OFFSETS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    const FwSizeType cleared = this->m_offsetCount;
    this->m_offsetCount = 0;
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
    if (this->m_offsetCount == 0) {
        this->log_ACTIVITY_HI_OffsetTableEmpty();
    }
    for (FwSizeType i = 0; i < this->m_offsetCount; i++) {
        const OffsetEntry& entry = this->m_offsets[i];
        this->log_ACTIVITY_HI_OffsetReport(entry.lower, entry.upper, entry.offset_us);
    }
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

bool TimeConverter ::lookupOffset(const TimeBase& from, const TimeBase& to, I64& offset_us) const {
    FwSizeType index = 0;
    if (!this->findEntry(from, to, index)) {
        return false;
    }
    const OffsetEntry& entry = this->m_offsets[index];
    // Entries hold the offset in canonical order; the reverse conversion negates it
    offset_us = (timeBaseValue(from) == timeBaseValue(entry.lower)) ? entry.offset_us : -entry.offset_us;
    return true;
}

bool TimeConverter ::storeOffset(const TimeBase& from, const TimeBase& to, I64 offset_us) {
    if (timeBaseValue(from) == timeBaseValue(to)) {
        this->log_WARNING_LO_IdenticalTimeBases(from);
        return false;
    }
    // Bound stored offsets so that applying or negating one cannot overflow
    if ((offset_us > MAX_TIME_US) || (offset_us < -MAX_TIME_US)) {
        this->log_WARNING_HI_OffsetOutOfRange(from, to, offset_us);
        return false;
    }

    const bool ascending = timeBaseValue(from) < timeBaseValue(to);
    const TimeBase lower = ascending ? from : to;
    const TimeBase upper = ascending ? to : from;
    const I64 canonical_offset_us = ascending ? offset_us : -offset_us;

    FwSizeType index = 0;
    if (!this->findEntry(from, to, index)) {
        if (this->m_offsetCount >= Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES) {
            this->log_WARNING_HI_OffsetTableFull(from, to, static_cast<U32>(Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES));
            return false;
        }
        index = this->m_offsetCount;
        this->m_offsetCount++;
    }

    this->m_offsets[index].lower = lower;
    this->m_offsets[index].upper = upper;
    this->m_offsets[index].offset_us = canonical_offset_us;
    return true;
}

bool TimeConverter ::findEntry(const TimeBase& from, const TimeBase& to, FwSizeType& index) const {
    const bool ascending = timeBaseValue(from) < timeBaseValue(to);
    const FwTimeBaseStoreType lower = timeBaseValue(ascending ? from : to);
    const FwTimeBaseStoreType upper = timeBaseValue(ascending ? to : from);

    for (FwSizeType i = 0; i < this->m_offsetCount; i++) {
        if ((timeBaseValue(this->m_offsets[i].lower) == lower) && (timeBaseValue(this->m_offsets[i].upper) == upper)) {
            index = i;
            return true;
        }
    }
    return false;
}

}  // namespace Svc
