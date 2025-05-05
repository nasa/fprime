// ======================================================================
// \title  ChronoTimeTester.cpp
// \author mstarch
// \brief  cpp file for ChronoTime component test harness implementation class
// ======================================================================

#include "ChronoTimeTester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <iostream>
namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ChronoTimeTester ::ChronoTimeTester()
    : ChronoTimeGTestBase("ChronoTimeTester", ChronoTimeTester::MAX_HISTORY_SIZE), component("ChronoTime") {
    this->initComponents();
    this->connectPorts();
}

ChronoTimeTester ::~ChronoTimeTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ChronoTimeTester ::test_basic_time() {
    const U32 SECONDS_SINCE_EPOCH_2024_11_25 = 1732492800ull; // Time since epoch as of date of writing
    const std::string expected_epoch_time("Thu Jan  1 00:00:00 1970\n");  //std::asctime adds \n

    // Calculate the system_clock epoch. This test is only valid when the system_clock uses unix epoch.
    const auto epoch = std::chrono::time_point<std::chrono::system_clock>{};
    std::time_t epoch_time = std::chrono::system_clock::to_time_t(epoch);
    const std::string reported_epoch_time(std::asctime(std::gmtime(&epoch_time)));

    // Skip test when epoch is not the unix epoch
    if (expected_epoch_time != reported_epoch_time) {
        GTEST_SKIP() << "Cannot run std::chrono test with non-unix epoch of: " << reported_epoch_time;
    }

    REQUIREMENT("SVC_CHRONO_TIME_002");
    REQUIREMENT("SVC_CHRONO_TIME_003");

    // Invoke port
    Fw::Time time;
    this->invoke_to_timeGetPort(0, time);
    ASSERT_GT(time.getSeconds(), SECONDS_SINCE_EPOCH_2024_11_25);
    // Check for correct use of milliseconds
    ASSERT_GE(time.getUSeconds(), 0U);
    ASSERT_LE(time.getUSeconds(), 999999U);
}

}  // namespace Svc
