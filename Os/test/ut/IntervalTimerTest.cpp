#include "gtest/gtest.h"
#include <Os/IntervalTimer.hpp>
#include <Os/Task.hpp>
#include <cstdio>

extern "C" {
    void intervalTimerTest(void);
}

void intervalTimerTest(void) {
    Os::IntervalTimer timer;
    timer.start();
    Os::Task::delay(1000);
    timer.stop();
    ASSERT_GE(timer.getDiffUsec(), 1000000);
    ASSERT_LT(timer.getDiffUsec(), 1005000);
}
