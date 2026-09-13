// ======================================================================
// \title  TimeConverterTester.cpp
// \author devin
// \brief  cpp file for TimeConverter component test harness implementation class
// ======================================================================

#include "TimeConverterTester.hpp"

#include <limits>

namespace Svc {

namespace {
//! Microseconds in one second
constexpr I64 US_PER_SECOND = 1000000;

//! Largest time representable by an Fw::Time, in microseconds
constexpr I64 MAX_TIME_US = static_cast<I64>(std::numeric_limits<U32>::max()) * US_PER_SECOND + (US_PER_SECOND - 1);

//! Every time base of the default configuration, in ascending numeric order
const TimeBase ALL_TIME_BASES[] = {TimeBase::TB_NONE, TimeBase::TB_PROC_TIME, TimeBase::TB_WORKSTATION_TIME,
                                   TimeBase::TB_SC_TIME, TimeBase::TB_DONT_CARE};

//! Number of distinct pairs that can be formed from the default time bases
constexpr FwSizeType ALL_PAIR_COUNT = 10;
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
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++);
    ASSERT_EVENTS_OffsetReport_SIZE(1);
}

void TimeConverterTester ::setOffsetIdenticalTest() {
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, TimeBase::TB_SC_TIME, TimeBase::TB_SC_TIME,
                             US_PER_SECOND);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_IdenticalTimeBases_SIZE(1);
    ASSERT_EVENTS_IdenticalTimeBases(0, TimeBase::TB_SC_TIME);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_SET_OFFSET, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
}

void TimeConverterTester ::setOffsetOutOfRangeTest() {
    this->clearHistory();
    const U32 cmdSeq = this->m_cmdSeq++;
    const I64 offset_us = MAX_TIME_US + 1;
    this->sendCmd_SET_OFFSET(TimeConverterTester::TEST_INSTANCE_ID, cmdSeq, TimeBase::TB_SC_TIME,
                             TimeBase::TB_WORKSTATION_TIME, offset_us);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_OffsetOutOfRange_SIZE(1);
    ASSERT_EVENTS_OffsetOutOfRange(0, TimeBase::TB_SC_TIME, TimeBase::TB_WORKSTATION_TIME, offset_us);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, TimeConverter::OPCODE_SET_OFFSET, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);

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
    ASSERT_LE(ALL_PAIR_COUNT, Svc::TimeConverterCfg::MAX_OFFSET_ENTRIES);

    // Store an offset for every distinct pair of the configured time bases
    I64 offset_us = US_PER_SECOND;
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(ALL_TIME_BASES); i++) {
        for (FwSizeType j = i + 1; j < FW_NUM_ARRAY_ELEMENTS(ALL_TIME_BASES); j++) {
            this->setOffset(ALL_TIME_BASES[i], ALL_TIME_BASES[j], offset_us);
            offset_us += US_PER_SECOND;
        }
    }

    // Every pair is retrievable, including the entry at the end of the table
    offset_us = US_PER_SECOND;
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(ALL_TIME_BASES); i++) {
        for (FwSizeType j = i + 1; j < FW_NUM_ARRAY_ELEMENTS(ALL_TIME_BASES); j++) {
            this->assertGetOffset(ALL_TIME_BASES[i], ALL_TIME_BASES[j], offset_us);
            offset_us += US_PER_SECOND;
        }
    }

    this->clearHistory();
    this->sendCmd_DUMP_OFFSETS(TimeConverterTester::TEST_INSTANCE_ID, this->m_cmdSeq++);
    ASSERT_EVENTS_OffsetReport_SIZE(ALL_PAIR_COUNT);
}

}  // namespace Svc
