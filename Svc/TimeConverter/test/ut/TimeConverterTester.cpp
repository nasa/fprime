// ======================================================================
// \title  TimeConverterTester.cpp
// \author devin
// \brief  cpp file for TimeConverter component test harness implementation class
// ======================================================================

#include "TimeConverterTester.hpp"

#include <limits>

namespace Svc {

namespace {
constexpr I64 US_PER_SECOND = TimeConverter::US_PER_SECOND;
constexpr I64 MAX_TIME_US = TimeConverter::MAX_TIME_US;

//! Every pair of the default time bases that denote a clock, in canonical order
const Svc::TimeBasePair PAIRS[] = {{TimeBase::TB_PROC_TIME, TimeBase::TB_WORKSTATION_TIME},
                                   {TimeBase::TB_PROC_TIME, TimeBase::TB_SC_TIME},
                                   {TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME}};

//! Capacity of the offset table under the configuration the tests build against
constexpr FwSizeType CAPACITY = Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES;
}  // namespace

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TimeConverterTester ::TimeConverterTester()
    : TimeConverterGTestBase("TimeConverterTester", TimeConverterTester::MAX_HISTORY_SIZE),
      m_cmdSeq(0),
      component("TimeConverter") {
    this->initComponents();
    this->connectPorts();
}

TimeConverterTester ::~TimeConverterTester() {}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void TimeConverterTester ::setOffset(const TimeBase& from, const TimeBase& to, I64 offset_us) {
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, from, to, offset_us);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetSet_SIZE(1);
    ASSERT_EVENTS_OffsetSet(0, from, to, offset_us);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_SET_OFFSET, cmdSeq, Fw::CmdResponse::OK);
}

Fw::Time TimeConverterTester ::convert(const Fw::Time& in_time, const TimeBase& out_tb, Svc::ConvertTimeStatus status) {
    Fw::Time out_time(out_tb, 0, 0);
    const Svc::ConvertTimeStatus actual = this->invoke_to_convertTime(0, in_time, out_time);
    EXPECT_EQ(actual, status);
    return out_time;
}

void TimeConverterTester ::fillToCapacity() {
    ASSERT_LT(CAPACITY, FW_NUM_ARRAY_ELEMENTS(PAIRS));
    for (FwSizeType i = 0; i < CAPACITY; i++) {
        this->setOffset(PAIRS[i].get_lower(), PAIRS[i].get_upper(), static_cast<I64>(i + 1) * US_PER_SECOND);
    }
}

void TimeConverterTester ::assertSetOffsetRejected(const TimeBase& from,
                                                   const TimeBase& to,
                                                   I64 offset_us,
                                                   Fw::CmdResponse response) {
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, from, to, offset_us);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetSet_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_SET_OFFSET, cmdSeq, response);
}

void TimeConverterTester ::clearOffsets() {
    this->clearHistory();
    this->sendCmd_CLEAR_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++);
    this->clearHistory();
}

void TimeConverterTester ::assertGetOffset(const TimeBase& from, const TimeBase& to, I64 offset_us) {
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_GET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, from, to);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetReport_SIZE(1);
    ASSERT_EVENTS_OffsetReport(0, from, to, offset_us);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_GET_OFFSET, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TimeConverterTester ::sameTimeBaseTest() {
    this->clearHistory();
    const Fw::Time in_time(TimeBase::TB_SC_TIME, 7, 100, 200);
    const Fw::Time out_time = this->convert(in_time, TimeBase::TB_SC_TIME, Svc::ConvertTimeStatus::OK);
    ASSERT_EQ(out_time, in_time);
    ASSERT_EQ(out_time.getContext(), 7);
    ASSERT_EVENTS_SIZE(0);
}

void TimeConverterTester ::convertBothDirectionsTest() {
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -3 * US_PER_SECOND + 250000);

    this->clearHistory();
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 100, 0);
    const Fw::Time ws_time = this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::OK);
    ASSERT_EQ(ws_time, Fw::Time(TimeBase::TB_WORKSTATION_TIME, 97, 250000));

    // The same entry serves the reverse direction with the offset negated
    const Fw::Time round_trip = this->convert(ws_time, TimeBase::TB_SC_TIME, Svc::ConvertTimeStatus::OK);
    ASSERT_EQ(round_trip, sc_time);
    ASSERT_EVENTS_SIZE(0);
}

void TimeConverterTester ::contextPreservedTest() {
    this->setOffset(TimeBase::TB_PROC_TIME, TimeBase::TB_SC_TIME, 5 * US_PER_SECOND);

    this->clearHistory();
    const Fw::Time proc_time(TimeBase::TB_PROC_TIME, 42, 10, 500);
    const Fw::Time sc_time = this->convert(proc_time, TimeBase::TB_SC_TIME, Svc::ConvertTimeStatus::OK);
    ASSERT_EQ(sc_time, Fw::Time(TimeBase::TB_SC_TIME, 42, 15, 500));
    ASSERT_EQ(sc_time.getContext(), proc_time.getContext());
}

void TimeConverterTester ::unknownTimeBaseTest() {
    this->clearHistory();
    const Fw::Time in_time(TimeBase::TB_SC_TIME, 100, 0);
    Fw::Time out_time(TimeBase::TB_WORKSTATION_TIME, 1, 2);
    const Svc::ConvertTimeStatus status = this->invoke_to_convertTime(0, in_time, out_time);
    ASSERT_EQ(status, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EQ(out_time, Fw::Time(TimeBase::TB_WORKSTATION_TIME, 1, 2));
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(1);
    ASSERT_EVENTS_NoConversionAvailable(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME);
}

void TimeConverterTester ::invalidTimeTest() {
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -10 * US_PER_SECOND);

    // A result before the time origin is rejected
    this->clearHistory();
    const Fw::Time early_time(TimeBase::TB_SC_TIME, 1, 0);
    Fw::Time out_time(TimeBase::TB_WORKSTATION_TIME, 3, 4);
    Svc::ConvertTimeStatus status = this->invoke_to_convertTime(0, early_time, out_time);
    ASSERT_EQ(status, Svc::ConvertTimeStatus::INVALID_TIME);
    ASSERT_EQ(out_time, Fw::Time(TimeBase::TB_WORKSTATION_TIME, 3, 4));
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidTime_SIZE(1);
    ASSERT_EVENTS_InvalidTime(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -10 * US_PER_SECOND,
                              US_PER_SECOND);

    // A result beyond the representable range is rejected in the reverse direction
    this->clearHistory();
    const Fw::Time late_time(TimeBase::TB_WORKSTATION_TIME, std::numeric_limits<U32>::max(), 999999);
    out_time = Fw::Time(TimeBase::TB_SC_TIME, 3, 4);
    status = this->invoke_to_convertTime(0, late_time, out_time);
    ASSERT_EQ(out_time, Fw::Time(TimeBase::TB_SC_TIME, 3, 4));
    ASSERT_EQ(status, Svc::ConvertTimeStatus::INVALID_TIME);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidTime_SIZE(1);
    ASSERT_EVENTS_InvalidTime(0, TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, 10 * US_PER_SECOND, MAX_TIME_US);
}

void TimeConverterTester ::portUpdateTest() {
    this->clearHistory();
    Svc::TimeOffset offset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, 2 * US_PER_SECOND);
    this->invoke_to_offsetUpdate(0, offset);
    // Correlation components update continuously, so port updates emit no event
    ASSERT_EVENTS_SIZE(0);

    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 0);
    ASSERT_EQ(this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::OK),
              Fw::Time(TimeBase::TB_WORKSTATION_TIME, 12, 0));

    // A later update replaces the stored offset
    offset.set_offset_us(-2 * US_PER_SECOND);
    this->invoke_to_offsetUpdate(0, offset);
    ASSERT_EQ(this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::OK),
              Fw::Time(TimeBase::TB_WORKSTATION_TIME, 8, 0));
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -2 * US_PER_SECOND);
}

void TimeConverterTester ::replaceOffsetTest() {
    // The pair is supplied in one order, then the other
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, US_PER_SECOND);
    this->setOffset(TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, 4 * US_PER_SECOND);

    this->assertGetOffset(TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, 4 * US_PER_SECOND);
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -4 * US_PER_SECOND);

    // One pair occupies one entry regardless of the order it was supplied in
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_OffsetReport_SIZE(1);
    ASSERT_EVENTS_OffsetReport(0, TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, 4 * US_PER_SECOND);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_DUMP_OFFSETS, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverterTester ::setOffsetIdenticalTest() {
    this->assertSetOffsetRejected(TimeBase::TB_SC_TIME, TimeBase::TB_SC_TIME, US_PER_SECOND,
                                  Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(1);
    ASSERT_EVENTS_IdenticalTimeBases(0, TimeBase::TB_SC_TIME);
}

void TimeConverterTester ::setOffsetOutOfRangeTest() {
    const I64 offset_us = MAX_TIME_US + 1;
    this->assertSetOffsetRejected(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, offset_us,
                                  Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(1);
    ASSERT_EVENTS_OffsetOutOfRange(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, offset_us);

    // The offset was not stored
    this->clearHistory();
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 0);
    (void)this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(1);
}

void TimeConverterTester ::clearOffsetsTest() {
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, US_PER_SECOND);
    this->setOffset(TimeBase::TB_PROC_TIME, TimeBase::TB_SC_TIME, US_PER_SECOND);

    this->clearHistory();
    U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_CLEAR_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetsCleared_SIZE(1);
    ASSERT_EVENTS_OffsetsCleared(0, 2);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_CLEAR_OFFSETS, cmdSeq, Fw::CmdResponse::OK);

    // Conversions no longer resolve and the table reports itself empty
    this->clearHistory();
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 0);
    (void)this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(1);

    this->clearHistory();
    cmdSeq = this->m_cmdSeq++;
    this->sendCmd_CLEAR_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_OffsetsCleared(0, 0);
}

void TimeConverterTester ::getOffsetTest() {
    // An unknown pair is reported as such
    this->clearHistory();
    U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_GET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, TimeBase::TB_SC_TIME,
                             TimeBase::TB_WORKSTATION_TIME);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_NoOffsetStored_SIZE(1);
    ASSERT_EVENTS_NoOffsetStored(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_GET_OFFSET, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);

    // A single time base is not a pair
    this->clearHistory();
    cmdSeq = this->m_cmdSeq++;
    this->sendCmd_GET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, TimeBase::TB_SC_TIME, TimeBase::TB_SC_TIME);
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_GET_OFFSET, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);

    // A stored pair is reported in both directions
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, 7 * US_PER_SECOND);
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, 7 * US_PER_SECOND);
    this->assertGetOffset(TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, -7 * US_PER_SECOND);
}

void TimeConverterTester ::dumpOffsetsTest() {
    // An empty table reports itself empty
    this->clearHistory();
    U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetTableEmpty_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_DUMP_OFFSETS, cmdSeq, Fw::CmdResponse::OK);

    this->setOffset(TimeBase::TB_PROC_TIME, TimeBase::TB_WORKSTATION_TIME, US_PER_SECOND);
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -2 * US_PER_SECOND);

    // Every entry is reported in canonical order
    this->clearHistory();
    cmdSeq = this->m_cmdSeq++;
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_OffsetReport_SIZE(2);
    ASSERT_EVENTS_OffsetReport(0, TimeBase::TB_PROC_TIME, TimeBase::TB_WORKSTATION_TIME, US_PER_SECOND);
    ASSERT_EVENTS_OffsetReport(1, TimeBase::TB_WORKSTATION_TIME, TimeBase::TB_SC_TIME, 2 * US_PER_SECOND);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_DUMP_OFFSETS, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverterTester ::fillTableTest() {
    this->fillToCapacity();

    // Every pair is retrievable, including the entry at the end of the table
    for (FwSizeType i = 0; i < CAPACITY; i++) {
        this->assertGetOffset(PAIRS[i].get_lower(), PAIRS[i].get_upper(), static_cast<I64>(i + 1) * US_PER_SECOND);
    }

    // The dump reports every entry, in the order the pairs were first stored
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq);
    ASSERT_EVENTS_OffsetReport_SIZE(CAPACITY);
    for (FwSizeType i = 0; i < CAPACITY; i++) {
        ASSERT_EVENTS_OffsetReport(static_cast<U32>(i), PAIRS[i].get_lower(), PAIRS[i].get_upper(),
                                   static_cast<I64>(i + 1) * US_PER_SECOND);
    }
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_DUMP_OFFSETS, cmdSeq, Fw::CmdResponse::OK);
}

void TimeConverterTester ::tableFullTest() {
    this->fillToCapacity();

    // A pair beyond the capacity is rejected, and the command reports a state rather than a bad argument
    const Svc::TimeBasePair& extra = PAIRS[CAPACITY];
    this->assertSetOffsetRejected(extra.get_lower(), extra.get_upper(), US_PER_SECOND,
                                  Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_OffsetTableFull_SIZE(1);
    ASSERT_EVENTS_OffsetTableFull(0, extra.get_lower(), extra.get_upper(), static_cast<U32>(CAPACITY));

    // The rejected pair was not stored
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_GET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, extra.get_lower(), extra.get_upper());
    ASSERT_EVENTS_NoOffsetStored_SIZE(1);
    ASSERT_EVENTS_NoOffsetStored(0, extra.get_lower(), extra.get_upper());
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_GET_OFFSET, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);

    // A pair already stored is still replaceable with the table full
    this->setOffset(PAIRS[0].get_lower(), PAIRS[0].get_upper(), 42 * US_PER_SECOND);
    this->assertGetOffset(PAIRS[0].get_lower(), PAIRS[0].get_upper(), 42 * US_PER_SECOND);

    // Clearing the table makes room again
    this->clearHistory();
    this->sendCmd_CLEAR_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++);
    ASSERT_EVENTS_OffsetsCleared(0, static_cast<U32>(CAPACITY));
    this->setOffset(extra.get_lower(), extra.get_upper(), US_PER_SECOND);
}

void TimeConverterTester ::offsetBoundariesTest() {
    // An offset of zero is a valid correlation
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, 0);
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 20);
    ASSERT_EQ(this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::OK),
              Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 20));

    // The largest shift a time can undergo in either direction is accepted
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, MAX_TIME_US);
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, MAX_TIME_US);
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -MAX_TIME_US);
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -MAX_TIME_US);

    // One microsecond beyond that range is rejected in the negative direction as well
    const I64 offset_us = -MAX_TIME_US - 1;
    this->assertSetOffsetRejected(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, offset_us,
                                  Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(1);
    ASSERT_EVENTS_OffsetOutOfRange(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, offset_us);

    // The rejection left the previously stored offset in place
    this->assertGetOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -MAX_TIME_US);
}

void TimeConverterTester ::portUpdateRejectedTest() {
    // A single time base supplied as both ends of a pair is rejected
    this->clearHistory();
    Svc::TimeOffset offset(TimeBase::TB_SC_TIME, TimeBase::TB_SC_TIME, US_PER_SECOND);
    this->invoke_to_offsetUpdate(0, offset);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(1);
    ASSERT_EVENTS_IdenticalTimeBases(0, TimeBase::TB_SC_TIME);

    // An offset beyond the representable range is rejected and not stored
    this->clearHistory();
    offset.set_to(TimeBase::TB_WORKSTATION_TIME);
    offset.set_offset_us(MAX_TIME_US + 1);
    this->invoke_to_offsetUpdate(0, offset);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(1);

    this->clearHistory();
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 0);
    (void)this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(1);
}

void TimeConverterTester ::throttleResetTest() {
    const Fw::Time sc_time(TimeBase::TB_SC_TIME, 10, 0);
    const Fw::Time early_time(TimeBase::TB_SC_TIME, 1, 0);
    const Svc::TimeBasePair& extra = PAIRS[CAPACITY];

    // Each warning is emitted until its throttle count is reached, then suppressed
    this->clearHistory();
    for (FwSizeType i = 0; i < this->getNoConversionAvailableThrottle() + 2; i++) {
        (void)this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    }
    ASSERT_EVENTS_NoConversionAvailable_SIZE(this->getNoConversionAvailableThrottle());

    this->clearHistory();
    for (FwSizeType i = 0; i < this->getIdenticalTimeBasesThrottle() + 2; i++) {
        this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_SC_TIME,
                                 TimeBase::TB_SC_TIME, US_PER_SECOND);
    }
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(this->getIdenticalTimeBasesThrottle());

    this->clearHistory();
    for (FwSizeType i = 0; i < this->getOffsetOutOfRangeThrottle() + 2; i++) {
        this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_SC_TIME,
                                 TimeBase::TB_WORKSTATION_TIME, MAX_TIME_US + 1);
    }
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(this->getOffsetOutOfRangeThrottle());

    this->fillToCapacity();
    this->clearHistory();
    for (FwSizeType i = 0; i < this->getOffsetTableFullThrottle() + 2; i++) {
        this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, extra.get_lower(),
                                 extra.get_upper(), US_PER_SECOND);
    }
    ASSERT_EVENTS_OffsetTableFull_SIZE(this->getOffsetTableFullThrottle());

    this->clearHistory();
    for (FwSizeType i = 0; i < this->getUnusableTimeBaseThrottle() + 2; i++) {
        this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_NONE,
                                 TimeBase::TB_SC_TIME, US_PER_SECOND);
    }
    ASSERT_EVENTS_UnusableTimeBase_SIZE(this->getUnusableTimeBaseThrottle());

    // A conversion outside the representable range is warned about under the same scheme
    this->clearOffsets();
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -10 * US_PER_SECOND);
    this->clearHistory();
    for (FwSizeType i = 0; i < this->getInvalidTimeThrottle() + 2; i++) {
        (void)this->convert(early_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::INVALID_TIME);
    }
    ASSERT_EVENTS_InvalidTime_SIZE(this->getInvalidTimeThrottle());

    // Clearing the offsets re-arms every throttled warning
    this->clearOffsets();
    (void)this->convert(sc_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(1);

    this->clearHistory();
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_SC_TIME,
                             TimeBase::TB_SC_TIME, US_PER_SECOND);
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(1);

    this->clearHistory();
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_SC_TIME,
                             TimeBase::TB_WORKSTATION_TIME, MAX_TIME_US + 1);
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(1);

    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, -10 * US_PER_SECOND);
    this->clearHistory();
    (void)this->convert(early_time, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::INVALID_TIME);
    ASSERT_EVENTS_InvalidTime_SIZE(1);

    this->clearOffsets();
    this->fillToCapacity();
    this->clearHistory();
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, extra.get_lower(),
                             extra.get_upper(), US_PER_SECOND);
    ASSERT_EVENTS_OffsetTableFull_SIZE(1);

    this->clearOffsets();
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++, TimeBase::TB_NONE,
                             TimeBase::TB_SC_TIME, US_PER_SECOND);
    ASSERT_EVENTS_UnusableTimeBase_SIZE(1);
}

void TimeConverterTester ::unusableTimeBaseTest() {
    // A time base naming no clock is rejected at either end of a pair
    this->assertSetOffsetRejected(TimeBase::TB_NONE, TimeBase::TB_SC_TIME, US_PER_SECOND,
                                  Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_UnusableTimeBase_SIZE(1);
    ASSERT_EVENTS_UnusableTimeBase(0, TimeBase::TB_NONE);

    this->assertSetOffsetRejected(TimeBase::TB_SC_TIME, TimeBase::TB_DONT_CARE, US_PER_SECOND,
                                  Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_UnusableTimeBase_SIZE(1);
    ASSERT_EVENTS_UnusableTimeBase(0, TimeBase::TB_DONT_CARE);

    // Both ends are reported when neither denotes a clock
    this->clearHistory();
    const Svc::TimeOffset offset(TimeBase::TB_NONE, TimeBase::TB_DONT_CARE, US_PER_SECOND);
    this->invoke_to_offsetUpdate(0, offset);
    ASSERT_EVENTS_UnusableTimeBase_SIZE(2);

    // A conversion involving one is refused before any offset is consulted
    this->clearHistory();
    const Fw::Time in_time(TimeBase::TB_NONE, 10, 0);
    (void)this->convert(in_time, TimeBase::TB_SC_TIME, Svc::ConvertTimeStatus::UNKNOWN_TIMEBASE);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UnusableTimeBase(0, TimeBase::TB_NONE);
    ASSERT_EVENTS_NoConversionAvailable_SIZE(0);

    // So is a request to report an offset for one, once the warning has been re-armed
    this->clearOffsets();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_GET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, TimeBase::TB_DONT_CARE,
                             TimeBase::TB_SC_TIME);
    ASSERT_EVENTS_UnusableTimeBase_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_GET_OFFSET, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
}

void TimeConverterTester ::conversionRangeEndsTest() {
    // A conversion landing exactly on the end of the range is valid in both directions
    this->setOffset(TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, MAX_TIME_US);

    this->clearHistory();
    const Fw::Time origin(TimeBase::TB_SC_TIME, 0, 0);
    const Fw::Time latest(TimeBase::TB_WORKSTATION_TIME, std::numeric_limits<U32>::max(), 999999);
    ASSERT_EQ(this->convert(origin, TimeBase::TB_WORKSTATION_TIME, Svc::ConvertTimeStatus::OK), latest);
    ASSERT_EQ(this->convert(latest, TimeBase::TB_SC_TIME, Svc::ConvertTimeStatus::OK),
              Fw::Time(TimeBase::TB_SC_TIME, 0, 0));
    ASSERT_EVENTS_SIZE(0);
}

void TimeConverterTester ::portUpdateTableFullTest() {
    this->fillToCapacity();

    // A correlation component pushing a new pair into a full table learns of it only by event
    this->clearHistory();
    const Svc::TimeBasePair& extra = PAIRS[CAPACITY];
    const Svc::TimeOffset offset(extra.get_lower(), extra.get_upper(), US_PER_SECOND);
    this->invoke_to_offsetUpdate(0, offset);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetTableFull_SIZE(1);
    ASSERT_EVENTS_OffsetTableFull(0, extra.get_lower(), extra.get_upper(), static_cast<U32>(CAPACITY));
}

}  // namespace Svc
