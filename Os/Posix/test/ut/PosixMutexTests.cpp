// ======================================================================
// \title Os/Posix/test/ut/PosixMutexTests.cpp
// \brief tests for posix implementation for Os::Mutex
// ======================================================================
#include "Os/test/ut/mutex/RulesHeaders.hpp"
#include "Os/test/ut/mutex/CommonTests.hpp"
#include "Os/Posix/Task.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"
#include "Fw/Types/String.hpp"


static void testTaskRoutine(void* pointer) {
    // Os::Test::Mutex::Tester::LockMutex lock_rule;
    // Os::Test::Mutex::Tester::UnlockMutex unlock_rule;

    Os::Test::Mutex::Tester* tester = reinterpret_cast<Os::Test::Mutex::Tester*>(pointer);
    
    for (FwIndexType i = 0; i < 100000; i++) {
        tester->m_mutex.lock();
        tester->m_state = Os::Test::Mutex::Tester::MutexState::LOCKED;

        tester->m_value = 1;
        ASSERT_EQ(tester->m_value, 1);

        tester->m_state = Os::Test::Mutex::Tester::MutexState::UNLOCKED;
        tester->m_mutex.unLock();
    }

}

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// Attempt to delete a locked mutex - expect an assertion
TEST_F(FunctionalityTester, PosixDeleteLockedMutex) {
    Os::Test::Mutex::Tester::LockMutex lock_rule;
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;
    lock_rule.apply(*tester);
    ASSERT_DEATH_IF_SUPPORTED(delete tester.release(), Os::Test::Mutex::Tester::ASSERT_IN_MUTEX_CPP);
}

// Test behavior of the mutex - two threads (main and test_task) using a mutex to protect a shared variable
TEST_F(FunctionalityTester, PosixMutexDataProtection) {
    // start a task that will lock the mutex, change the value and assert, then unlock mutex
    Os::Task test_task;
    Os::Task::Arguments arguments(Fw::String("MutexTestLockTask"), testTaskRoutine, static_cast<void*>(tester.get()));
    Os::Task::Status stat = test_task.start(arguments);
    FW_ASSERT(Os::Task::OP_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));

    Os::Test::Mutex::Tester::ProtectDataCheck protect_data_rule;

    for (FwIndexType i = 0; i < 100000; i++) {
        protect_data_rule.apply(*tester);
    }

    test_task.join();
}


int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
