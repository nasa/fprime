// ======================================================================
// \title Os/Posix/test/ut/PosixRawTimeTests.cpp
// \brief tests for posix implementation for Os::RawTime
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Posix/RawTime.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/test/ut/rawtime/CommonTests.hpp"
#include "Os/test/ut/rawtime/RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"

// ----------------------------------------------------------------------
// Required platform-specific helper function
// ----------------------------------------------------------------------

void Os::Test::RawTime::assert_and_update_now(const Os::RawTime& raw_time_under_test,
                                              const std::chrono::system_clock::time_point& lower_time,
                                              const std::chrono::system_clock::time_point& upper_time,
                                              std::chrono::system_clock::time_point& shadow_time) {
    // Extract POSIX timespec from raw_time_under_test
    const timespec& timespec_handle = static_cast<const Os::Posix::RawTime::PosixRawTimeHandle*>(
                                          const_cast<Os::RawTime&>(raw_time_under_test).getHandle())
                                          ->m_timespec;
    // Ensure timespec_handle is between lower_time and upper_time
    auto lower_time_sec = std::chrono::duration_cast<std::chrono::seconds>(lower_time.time_since_epoch()).count();
    auto upper_time_sec = std::chrono::duration_cast<std::chrono::seconds>(upper_time.time_since_epoch()).count();
    auto lower_time_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(lower_time.time_since_epoch()).count() % 1000000000;
    auto upper_time_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(upper_time.time_since_epoch()).count() % 1000000000;

    EXPECT_GE(timespec_handle.tv_sec, lower_time_sec);
    EXPECT_LE(timespec_handle.tv_sec, upper_time_sec);
    if (timespec_handle.tv_sec == lower_time_sec) {
        EXPECT_GE(timespec_handle.tv_nsec, lower_time_nsec);
    }
    if (timespec_handle.tv_sec == upper_time_sec) {
        EXPECT_LE(timespec_handle.tv_nsec, upper_time_nsec);
    }
    // Update shadow time with values of raw_time_under_test
    auto duration = std::chrono::seconds{timespec_handle.tv_sec} + std::chrono::nanoseconds{timespec_handle.tv_nsec};
    shadow_time = std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(duration));
}

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
