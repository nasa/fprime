// ======================================================================
// \title Os/Stub/test/ut/StubConditionTests.cpp
// \brief tests using stub implementation for Os::ConditionVariable interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Condition.hpp"
#include "Os/Stub/test/ConditionVariable.hpp"
#include "Os/Stub/test/Mutex.hpp"

// Construction test
TEST(Interface, Construction) {
    Os::ConditionVariable variable;
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::CONSTRUCT_FN);
}

// Destruct test
TEST(Interface, Destruction) {
    delete (new Os::ConditionVariable);
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::DESTRUCT_FN);
}

// Wait test
TEST(Interface, WaitNotHeld) {
    Os::Mutex mutex;
    Os::ConditionVariable variable;
    auto status = variable.pend(mutex);
    ASSERT_EQ(Os::ConditionVariable::Status::OP_OK, status);
    ASSERT_EQ(Os::Stub::Mutex::Test::StaticData::data.lastCalled, Os::Stub::Mutex::Test::StaticData::CONSTRUCT_FN);
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::WAIT_FN);
}

// Wait test
TEST(Interface, WaitHeld) {
    Os::Mutex mutex;
    Os::ConditionVariable variable;
    mutex.lock();
    auto status = variable.pend(mutex);
    ASSERT_EQ(Os::ConditionVariable::Status::OP_OK, status);
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::WAIT_FN);
}

// Notify test
TEST(Interface, Notify) {
    Os::ConditionVariable variable;
    variable.notify();
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::NOTIFY_FN);
}

// NotifyAll test
TEST(Interface, NotifyAll) {
    Os::ConditionVariable variable;
    variable.notifyAll();
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::NOTIFY_ALL_FN);
}

TEST(Interface, Handle) {
    Os::ConditionVariable variable;
    ASSERT_EQ(variable.getHandle(), Os::Stub::ConditionVariable::Test::StaticData::data.handle);
    ASSERT_EQ(Os::Stub::ConditionVariable::Test::StaticData::data.lastCalled,
              Os::Stub::ConditionVariable::Test::StaticData::HANDLE_FN);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
