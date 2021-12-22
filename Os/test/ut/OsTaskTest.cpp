#include "gtest/gtest.h"
#include <Os/Task.hpp>
#include <cstdio>

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
    Os::Task::TaskStatus stat = testTask.start(name,someTask,const_cast<bool*>(&taskRan));
    ASSERT_EQ(stat, Os::Task::TASK_OK);
    testTask.join(nullptr);
    ASSERT_EQ(taskRan, true);
}
