#include "gtest/gtest.h"
#include <Os/Task.hpp>
#include <stdio.h>

extern "C" {
    void startTestTask(int iters);
}

Os::Task* testTask = 0;

void someTask(void* ptr) {

    long iters = (long) ptr;

    while (iters--) {
        Os::Task::delay(1000);
        printf("Tick %ld!\n",iters);
    }
}

void startTestTask(int iters) {
    long localIter = iters;
    testTask = new Os::Task();
    Os::TaskString name("ATestTask");
    Os::Task::TaskStatus stat = testTask->start(name,12,100,10*1024,someTask,(void*) localIter);
    ASSERT_EQ(stat, Os::Task::TASK_OK);
}
