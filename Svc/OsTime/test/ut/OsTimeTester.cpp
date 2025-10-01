// ======================================================================
// \title  OsTimeTester.cpp
// \author kubiak
// \brief  cpp file for OsTime component test harness implementation class
// ======================================================================

#include "OsTimeTester.hpp"
#include "Svc/OsTime/OsTime.hpp"
#include "Svc/OsTime/test/RawTimeTester/RawTimeTester.hpp"

#include <chrono>
#include <cstdint>
#include <thread>

#define TIMING_UB (1.5)
#define ASSERT_TIMING_RAW(exp_base, exp_offset, act, ub_ratio) \
    {                                                          \
        ASSERT_LT(exp_base + exp_offset, act);                 \
        ASSERT_GT(exp_base + (exp_offset * ub_ratio), act);    \
    }

#define ASSERT_TIMING(exp_base, exp_offset, act) ASSERT_TIMING_RAW(exp_base, exp_offset, act, TIMING_UB)

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

OsTimeTester ::OsTimeTester() : OsTimeGTestBase("OsTimeTester", OsTimeTester::MAX_HISTORY_SIZE), component("OsTime") {
    this->initComponents();
    this->connectPorts();
}

OsTimeTester ::~OsTimeTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void OsTimeTester ::zeroEpochTest() {
    Os::RawTime epoch_os_time;
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 0));
    Os::RawTime::Status os_stat = epoch_os_time.now();
    ASSERT_EQ(os_stat, Os::RawTime::OP_OK);
    component.set_epoch(Fw::ZERO_TIME, epoch_os_time);

    // 200 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 200 * 1000));
    Fw::Time time_200ms;
    invoke_to_timeGetPort(0, time_200ms);
    ASSERT_EQ(time_200ms, Fw::Time(0, 200 * 1000));

    // 400 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 400 * 1000));
    Fw::Time time_400ms;
    invoke_to_timeGetPort(0, time_400ms);
    ASSERT_EQ(time_400ms, Fw::Time(0, 400 * 1000));

    // 1200 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(1, 200 * 1000));
    Fw::Time time_1200ms;
    invoke_to_timeGetPort(0, time_1200ms);
    ASSERT_EQ(time_1200ms, Fw::Time(1, 200 * 1000));

    // 123456.987654 s
    Svc::RawTimeTester::setNowTime(Fw::Time(123456, 987654));
    Fw::Time time_123456;
    invoke_to_timeGetPort(0, time_123456);
    ASSERT_EQ(time_123456, Fw::Time(123456, 987654));
}

void OsTimeTester ::offsetEpochTest() {
    Os::RawTime epoch_os_time;
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 0));
    Os::RawTime::Status os_stat = epoch_os_time.now();
    Fw::Time start_time(12345, 800 * 1000);
    ASSERT_EQ(os_stat, Os::RawTime::OP_OK);
    component.set_epoch(start_time, epoch_os_time);

    // 200 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 200 * 1000));
    Fw::Time time_200ms;
    invoke_to_timeGetPort(0, time_200ms);
    ASSERT_EQ(time_200ms, Fw::Time(12346, 0));
}

void OsTimeTester ::noEpochTest() {
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 0));
    // 0 ms
    Fw::Time time_0ms;
    invoke_to_timeGetPort(0, time_0ms);
    ASSERT_EQ(time_0ms, Fw::ZERO_TIME);

    // 200 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 200 * 1000));
    Fw::Time time_200ms;
    invoke_to_timeGetPort(0, time_200ms);
    ASSERT_EQ(time_200ms, Fw::ZERO_TIME);
}

void OsTimeTester ::updateEpochTest() {
    Os::RawTime epoch_os_time;
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 0));
    Os::RawTime::Status os_stat = epoch_os_time.now();
    ASSERT_EQ(os_stat, Os::RawTime::OP_OK);
    component.set_epoch(Fw::ZERO_TIME, epoch_os_time);

    // 200 ms
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 200 * 1000));
    Fw::Time time_200ms;
    invoke_to_timeGetPort(0, time_200ms);
    ASSERT_EQ(time_200ms, Fw::Time(0, 200 * 1000));

    // Change time base
    const Fw::Time new_base(TimeBase::TB_WORKSTATION_TIME, 7, 1234, 0);
    os_stat = epoch_os_time.now();
    ASSERT_EQ(os_stat, Os::RawTime::OP_OK);
    invoke_to_setEpoch(0, new_base, epoch_os_time);

    // Immediately get time
    Fw::Time time_200ms_2;
    invoke_to_timeGetPort(0, time_200ms_2);
    ASSERT_EQ(time_200ms_2, Fw::Time(TimeBase::TB_WORKSTATION_TIME, 7, 1234, 0));

    // 600 ms
    Fw::Time time_600ms;
    Svc::RawTimeTester::setNowTime(Fw::Time(0, 600 * 1000));
    invoke_to_timeGetPort(0, time_600ms);
    ASSERT_EQ(time_600ms, Fw::Time(TimeBase::TB_WORKSTATION_TIME, 7, 1234, 400 * 1000));
}

}  // namespace Svc
