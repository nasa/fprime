#include "gtest/gtest.h"
#include <Os/IntervalTimer.hpp>
#include <Os/Task.hpp>
#include <cstdio>

extern "C" {
    void intervalTimerTest();
}

void intervalTimerTest() {
    Os::IntervalTimer timer;
    timer.start();
    Os::Task::delay(Fw::TimeInterval(0, 20000));
    timer.stop();
    ASSERT_GE(timer.getDiffUsec(), 20000);
    ASSERT_LT(timer.getDiffUsec(), 25000);

    Fw::TimeInterval interval;
    ASSERT_TRUE(timer.getTimeInterval(interval));
    ASSERT_EQ(interval.getSeconds(), 0);
    ASSERT_GE(interval.getUSeconds(), 20000);
    ASSERT_LT(interval.getUSeconds(), 25000);

}
