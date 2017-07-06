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

    printf("Usec: %d\n",timer.getDiffUsec());
}
