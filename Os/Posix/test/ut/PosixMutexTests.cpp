// ======================================================================
// \title Os/Posix/test/ut/PosixMutexTests.cpp
// \brief tests for posix implementation for Os::Mutex
// ======================================================================
#include "Os/test/ut/mutex/RulesHeaders.hpp"
#include "Os/test/ut/mutex/CommonTests.hpp"
#include "Os/Posix/Task.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"

// A routine that modifies the internal state of the MutexTester to test that the mutex
// protects the shared variable successfully when ran in parallel with the main task
static void testTaskRoutine(void* pointer) {
    Os::Test::Mutex::Tester* tester = reinterpret_cast<Os::Test::Mutex::Tester*>(pointer);
    
    for (FwIndexType i = 0; i < 100000; i++) {
        tester->m_mutex.lock();
        tester->m_state = Os::Test::Mutex::Tester::MutexState::LOCKED;

        U32 randomValue = STest::Pick::any();
        tester->m_value = randomValue;
        ASSERT_EQ(tester->m_value, randomValue);

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
    lock_rule.apply(*tester);
    // tester is a unique_ptr, retrieve the raw pointer and attempt to delete the Mutex
    ASSERT_DEATH_IF_SUPPORTED(delete tester.get(), Os::Test::Mutex::Tester::ASSERT_IN_MUTEX_CPP);
}

// Test behavior of the mutex - two threads (main and test_task) using a mutex to protect a shared variable
TEST_F(FunctionalityTester, PosixMutexDataProtection) {
    // start a task that will lock the mutex, change the value and assert, then unlock mutex
    Os::Task test_task;
    Os::Task::Arguments arguments(Fw::String("MutexTestLockTask"), testTaskRoutine, static_cast<void*>(tester.get()));
    Os::Task::Status stat = test_task.start(arguments);
    FW_ASSERT(Os::Task::OP_OK == stat, static_cast<FwAssertArgType>(stat));

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
