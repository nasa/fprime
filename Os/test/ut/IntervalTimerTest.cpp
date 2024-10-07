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
    Os::Task::delay(Fw::TimeInterval(1, 0));
    timer.stop();
    ASSERT_GE(timer.getDiffUsec(), 1000000);
    ASSERT_LT(timer.getDiffUsec(), 1005000);
}
