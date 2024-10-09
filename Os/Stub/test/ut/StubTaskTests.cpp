// ======================================================================
// \title Os/Stub/test/ut/StubTaskTests.cpp
// \brief tests using stub implementation for Os::Task interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Stub/test/Task.hpp"
#include "Os/test/ut/task/CommonTests.hpp"
#include "Os/test/ut/task/RulesHeaders.hpp"

using namespace Os::Stub::Task::Test;

void testMethod(void* test) {

}

// Basic file tests
class Interface : public ::testing::Test {
public:
    //! Setup function delegating to UT setUp function
    void SetUp() override {
        StaticData::data = StaticData();
    }

    //! Setup function delegating to UT tearDown function
    void TearDown() override {
        StaticData::data = StaticData();
    }
};

struct QuickRegistry : Os::TaskRegistry {

    void addTask(Os::Task* task) override {
        this->m_task = task;
    }

    void removeTask(Os::Task* task) override {
        this->m_task = nullptr;
    }
    Os::Task* m_task = nullptr;
};


// Ensure that Os::Task properly calls the implementation constructor
TEST_F(Interface, Construction) {
    Os::Task task;
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
}

// Ensure that Os::Task properly calls the implementation destructor
TEST_F(Interface, Destruction) {
    delete (new Os::Task);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
}

// Ensure that Os::Task properly calls the implementation start
TEST_F(Interface, Start) {
    Os::Task task;
    StaticData::data.startStatus = Os::Task::Status::UNKNOWN_ERROR;
    Os::Task::Arguments arguments(Fw::String("Task"), &testMethod, nullptr);
    ASSERT_EQ(task.start(arguments), StaticData::data.startStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::START_FN);
}

// Ensure that Os::Task properly calls the implementation start with arguments
TEST_F(Interface, StartOptionalArguments) {
    Os::Task task;
    StaticData::data.startStatus = Os::Task::Status::INVALID_STACK;
    Os::Task::Arguments arguments(Fw::String("Task"), &testMethod, nullptr);
    ASSERT_EQ(task.start(arguments), StaticData::data.startStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::START_FN);
}

// Ensure that Os::Task properly calls the implementation onStart before calling start
TEST_F(Interface, OnStart) {
    Os::Task task;
    StaticData::data.startStatus = Os::Task::Status::UNKNOWN_ERROR;
    Os::Task::Arguments arguments(Fw::String("Task"), &testMethod, nullptr);
    ASSERT_EQ(task.start(arguments), StaticData::data.startStatus);

}

// Ensure that Os::Task properly calls the implementation suspend
TEST_F(Interface, Suspend) {
    Os::Task task;
    task.suspend();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::SUSPEND_FN);
}

// Ensure that Os::Task properly calls the implementation resume
TEST_F(Interface, Resume) {
    Os::Task task;
    StaticData::data.startStatus = Os::Task::Status::UNKNOWN_ERROR;
    task.resume();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RESUME_FN);
}

// Ensure that Os::Task properly calls the implementation join
TEST_F(Interface, Join) {
    Os::Task task;
    StaticData::data.joinStatus = Os::Task::Status::JOIN_ERROR;
    StaticData::data.startStatus = Os::Task::Status::JOIN_ERROR;
    Os::Task::Arguments arguments(Fw::String("Task"), &testMethod, nullptr);
    ASSERT_EQ(task.start(arguments), StaticData::data.startStatus);
    task.invokeRoutine();
    ASSERT_EQ(task.join(), StaticData::data.joinStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::JOIN_FN);
}

// Ensure that Os::Task properly calls the implementation getHandle
TEST_F(Interface, GetHandle) {
    Os::Task task;
}

// Ensure that Os::Task properly calls the implementation delay
TEST_F(Interface, Delay) {
    Os::Task task;
    StaticData::data.delayStatus = Os::Task::Status::DELAY_ERROR;
    ASSERT_EQ(Os::Task::delay(Fw::Time(0, 1)), StaticData::data.delayStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DELAY_FN);
}

// Ensure that Os::Task properly calls the registry removal function
TEST_F(Interface, RegistryRemove) {
    QuickRegistry registry;
    Os::Task::registerTaskRegistry(&registry);
    Os::Task* task = new Os::Task;
    StaticData::data.startStatus = Os::Task::Status::OP_OK;
    Os::Task::Arguments arguments(Fw::String("Task"), &testMethod, nullptr);
    ASSERT_EQ(task->start(arguments), StaticData::data.startStatus);
    Os::Task::s_numTasks = 0; // Reset task count for rules-based testing
    ASSERT_EQ(registry.m_task, task);
    delete task;
    ASSERT_EQ(registry.m_task, nullptr);


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
