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

namespace {

const Os::Posix::RawTime::PosixRawTimeHandle& get_posix_handle(const Os::RawTime& raw_time) {
    return *static_cast<const Os::Posix::RawTime::PosixRawTimeHandle*>(const_cast<Os::RawTime&>(raw_time).getHandle());
}

bool timespec_less_equal(const timespec& lhs, const timespec& rhs) {
    return (lhs.tv_sec < rhs.tv_sec) or (lhs.tv_sec == rhs.tv_sec and lhs.tv_nsec <= rhs.tv_nsec);
}

//! Check that now() on `source` reads `expected_clock` by bounding it between two direct reads of that clock
void assert_now_reads_clock(Os::RawTimeSource source, clockid_t expected_clock) {
    Os::RawTime raw_time(source);
    ASSERT_EQ(raw_time.getSource(), source);
    ASSERT_EQ(get_posix_handle(raw_time).m_clock_id, expected_clock);

    timespec lower = {0, 0};
    timespec upper = {0, 0};
    ASSERT_EQ(clock_gettime(expected_clock, &lower), 0);
    ASSERT_EQ(raw_time.now(), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(clock_gettime(expected_clock, &upper), 0);

    const timespec& read = get_posix_handle(raw_time).m_timespec;
    EXPECT_TRUE(timespec_less_equal(lower, read)) << "now() read precedes clock lower bound";
    EXPECT_TRUE(timespec_less_equal(read, upper)) << "now() read follows clock upper bound";
}

}  // namespace

TEST(PosixRawTime, DefaultConstructorUsesDefaultSource) {
    Os::RawTime raw_time;
    ASSERT_EQ(raw_time.getSource(), Os::RAWTIME_DEFAULT);
    ASSERT_EQ(get_posix_handle(raw_time).m_clock_id, static_cast<clockid_t>(Os::RAWTIME_DEFAULT));
    ASSERT_EQ(static_cast<clockid_t>(Os::RAWTIME_DEFAULT), static_cast<clockid_t>(CLOCK_REALTIME));
}

TEST(PosixRawTime, NowRealtime) {
    assert_now_reads_clock(Os::RAWTIME_REALTIME, CLOCK_REALTIME);
}

TEST(PosixRawTime, NowMonotonic) {
    assert_now_reads_clock(Os::RAWTIME_MONOTONIC, CLOCK_MONOTONIC);
}

#ifdef CLOCK_BOOTTIME
TEST(PosixRawTime, NowBoottime) {
    assert_now_reads_clock(Os::RAWTIME_BOOTTIME, CLOCK_BOOTTIME);
}
#endif

TEST(PosixRawTime, NowInvalidClockReturnsInvalidParams) {
    // Clock ids beyond those defined by the platform are rejected by clock_gettime with EINVAL
    const Os::RawTimeSource invalid_source = static_cast<Os::RawTimeSource>(1000);
    Os::RawTime raw_time(invalid_source);
    ASSERT_EQ(raw_time.now(), Os::RawTime::Status::INVALID_PARAMS);
}

TEST(PosixRawTime, CopyPreservesSource) {
    Os::RawTime original(Os::RAWTIME_MONOTONIC);
    ASSERT_EQ(original.now(), Os::RawTime::Status::OP_OK);

    Os::RawTime copied(original);
    ASSERT_EQ(copied.getSource(), Os::RAWTIME_MONOTONIC);
    ASSERT_EQ(get_posix_handle(copied).m_clock_id, static_cast<clockid_t>(CLOCK_MONOTONIC));
    ASSERT_TRUE(copied == original);

    Os::RawTime assigned;
    assigned = original;
    ASSERT_EQ(assigned.getSource(), Os::RAWTIME_MONOTONIC);
    ASSERT_EQ(get_posix_handle(assigned).m_clock_id, static_cast<clockid_t>(CLOCK_MONOTONIC));
    ASSERT_TRUE(assigned == original);
}

TEST(PosixRawTime, IntervalSameSource) {
    Os::RawTime first(Os::RAWTIME_MONOTONIC);
    Os::RawTime second(Os::RAWTIME_MONOTONIC);
    ASSERT_EQ(first.now(), Os::RawTime::Status::OP_OK);
    Os::Task::delay(Fw::TimeInterval(0, 10000));
    ASSERT_EQ(second.now(), Os::RawTime::Status::OP_OK);

    Fw::TimeInterval interval;
    ASSERT_EQ(second.getTimeInterval(first, interval), Os::RawTime::Status::OP_OK);
    EXPECT_TRUE(interval.getSeconds() > 0 or interval.getUSeconds() >= 10000) << "Interval shorter than delay";
    Fw::TimeInterval reversed;
    ASSERT_EQ(first.getTimeInterval(second, reversed), Os::RawTime::Status::OP_OK);
    EXPECT_EQ(interval, reversed);
}

TEST(PosixRawTime, IntervalDifferentSourcesRejected) {
    Os::RawTime realtime(Os::RAWTIME_REALTIME);
    Os::RawTime monotonic(Os::RAWTIME_MONOTONIC);
    ASSERT_EQ(realtime.now(), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(monotonic.now(), Os::RawTime::Status::OP_OK);

    Fw::TimeInterval interval;
    EXPECT_EQ(realtime.getTimeInterval(monotonic, interval), Os::RawTime::Status::INVALID_PARAMS);
    EXPECT_EQ(monotonic.getTimeInterval(realtime, interval), Os::RawTime::Status::INVALID_PARAMS);
    U32 usec = 0;
    EXPECT_EQ(realtime.getDiffUsec(monotonic, usec), Os::RawTime::Status::INVALID_PARAMS);
    EXPECT_FALSE(realtime == monotonic);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
