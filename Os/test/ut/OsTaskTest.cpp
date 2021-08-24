#include "gtest/gtest.h"
#include <Os/Task.hpp>
#include <stdio.h>

extern "C" {
    void startTestTask();
}

void someTask(void* ptr) {
    bool* ran = static_cast<bool*>(ptr);
    *ran = true;
}

void startTestTask() {
    volatile bool taskRan = false;
    Os::Task testTask;
    Os::TaskString name("ATestTask");
    Os::Task::TaskStatus stat = testTask.start(name,12,100,10*1024,someTask,const_cast<bool*>(&taskRan));
    ASSERT_EQ(stat, Os::Task::TASK_OK);
    testTask.join(NULL);
    ASSERT_EQ(taskRan, true);
}
