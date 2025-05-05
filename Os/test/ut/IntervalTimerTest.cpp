#include "gtest/gtest.h"
#include <Os/IntervalTimer.hpp>
#include <Os/Task.hpp>
#include <cstdio>

extern "C" {
    void intervalTimerTest();
}

void intervalTimerTest() {
    // Os::RawTime is already tested thoroughly, here we only test the IntervalTimer

    // We are using RawTimes to get upper and lower bounds for what the interval should be
    // This test expects functional RawTime (i.e. not Stubs)

    Os::IntervalTimer timer;

    Os::RawTime minStart;
    Os::RawTime minStop;
    Os::RawTime maxStart;
    Os::RawTime maxStop;

    maxStart.now(); // max bound
    timer.start();  // actual timer under test
    minStart.now(); // low bound

    // Delay 5ms (5000us) for good measure's sake, value should not affect test
    Os::Task::delay(Fw::TimeInterval(0, 5000));

    minStop.now(); // low bound
    timer.stop();  // actual timer under test
    maxStop.now(); // max bound

    Fw::TimeInterval testInterval;
    Fw::TimeInterval minInterval;
    Fw::TimeInterval maxInterval;

    ASSERT_EQ(maxStop.getTimeInterval(maxStart, maxInterval), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(minStop.getTimeInterval(minStart, minInterval), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(timer.getTimeInterval(testInterval), Os::RawTime::Status::OP_OK);

    // Compare intervals to make sure the IntervalTimer is returning within the bounds
    // Note: this also tests the comparison operators of Fw::TimeInterval
    ASSERT_TRUE(testInterval >= minInterval) << "Test interval: " << testInterval << " min interval: " << minInterval;
    ASSERT_TRUE(testInterval <= maxInterval) << "Test interval: " << testInterval << " max interval: " << maxInterval;
}
