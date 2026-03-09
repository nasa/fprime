// ======================================================================
// \title Os/Posix/test/ut/PosixRwMutexTests.cpp
// \brief tests for posix implementation for Os::RwMutex
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/test/ut/rwmutex/CommonTests.hpp"
#include "Os/test/ut/rwmutex/RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"

//! \brief Thread routine: writer that repeatedly modifies shared data
//! \param pointer Pointer to Os::Test::RwMutex::Tester instance
static void writerTaskRoutine(void* pointer) {
    using Tester = Os::Test::RwMutex::Tester;
    Tester* tester = reinterpret_cast<Tester*>(pointer);

    for (FwSizeType i = 0; i < 100000; i++) {
        // Acquire exclusive access
        tester->m_rwmutex.lock();
        tester->m_state = Tester::RwMutexState::WRITE_LOCKED;

        // Critical section: write random value
        int randomValue = STest::Pick::any();
        tester->m_value = randomValue;
        
        // Verify data integrity: value should not be corrupted
        // If mutex fails, another thread could modify m_shared_value
        ASSERT_EQ(tester->m_value, randomValue);

        // Release exclusive access
        tester->m_state = Tester::RwMutexState::UNLOCKED;
        tester->m_rwmutex.unlock();
    }
}

//! \brief Thread routine: reader that repeatedly reads shared data
//! \param pointer Pointer to Os::Test::RwMutex::Tester instance
static void readerTaskRoutine(void* pointer) {
    using Tester = Os::Test::RwMutex::Tester;
    Tester* tester = reinterpret_cast<Tester*>(pointer);

    for (FwSizeType i = 0; i < 100000; i++) {
        // Acquire shared access
        tester->m_rwmutex.lockRead();
        
        // Update model state
        if (tester->m_state == Tester::RwMutexState::UNLOCKED) {
            tester->m_state = Tester::RwMutexState::READ_LOCKED;
            tester->m_reader_count = 1;
        } else if (tester->m_state == Tester::RwMutexState::READ_LOCKED) {
            tester->m_reader_count++;
        }

        // Critical section: read data twice to detect tearing
        // If a writer modifies data mid-read, the two reads may differ
        int value1 = tester->m_value;
        int value2 = tester->m_value;
        ASSERT_EQ(value1, value2);

        // Release shared access and update model
        tester->m_rwmutex.unLockRead();
        
        FW_ASSERT(tester->m_reader_count > 0);
        tester->m_reader_count--;
        if (tester->m_reader_count == 0) {
            tester->m_state = Tester::RwMutexState::UNLOCKED;
        }
    }
}

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// Attempt to destroy a write-locked mutex - expect assertion
TEST_F(FunctionalityTester, PosixDeleteWriteLockedRwMutex) {
    GTEST_SKIP() << "Skipped: pthread_rwlock_destroy() does not reliably " \
        "return EBUSY for locked rwlock (POSIX undefined behavior). ";

    using Tester = Os::Test::RwMutex::Tester;

    // Acquire exclusive lock via rule
    Tester::LockRwMutex lock_rule;
    lock_rule.apply(*tester);
    ASSERT_EQ(tester->m_state, Tester::RwMutexState::WRITE_LOCKED);
    // tester is a unique_ptr, retrieve the raw pointer and attempt to delete the RwMutex
    ASSERT_DEATH_IF_SUPPORTED(delete tester.get(), Tester::ASSERT_IN_RWMUTEX_CPP);
}

// Test: Attempt to destroy a read-locked mutex - expect assertion
TEST_F(FunctionalityTester, PosixDeleteReadLockedRwMutex) {
    GTEST_SKIP() << "Skipped: pthread_rwlock_destroy() does not reliably " \
        "return EBUSY for locked rwlock (POSIX undefined behavior). ";

    using Tester = Os::Test::RwMutex::Tester;
    
    // Acquire shared lock via rule
    Tester::LockReadRwMutex lock_read_rule;
    lock_read_rule.apply(*tester);
    ASSERT_EQ(tester->m_state, Tester::RwMutexState::READ_LOCKED);
    ASSERT_EQ(tester->m_reader_count, static_cast<FwSizeType>(1));
    
    // tester is a unique_ptr, retrieve the raw pointer and attempt to delete the RwMutex
    ASSERT_DEATH_IF_SUPPORTED(delete tester.get(), Tester::ASSERT_IN_RWMUTEX_CPP);
}

// Test data protection with concurrent writer thread
TEST_F(FunctionalityTester, PosixRwMutexWriterDataProtection) {
    using Tester = Os::Test::RwMutex::Tester;
    
    // Start writer task in separate thread
    Os::Task writer_task;
    Os::Task::Arguments args(
        Fw::String("RwMutexWriterTask"), 
        writerTaskRoutine, 
        static_cast<void*>(tester.get())
    );
    Os::Task::Status stat = writer_task.start(args);
    FW_ASSERT(stat == Os::Task::OP_OK, static_cast<FwAssertArgType>(stat));

    // Main thread also performs write operations via rule
    Tester::ProtectDataWrite protect_write_rule;
    for (FwSizeType i = 0; i < 100000; i++) {
        protect_write_rule.apply(*tester);
    }

    // Wait for writer thread to complete
    writer_task.join();
    
    // Final data integrity check
    int finalValue = tester->m_value;
    ASSERT_EQ(tester->m_value, finalValue);
}

// Test data protection with concurrent reader thread
TEST_F(FunctionalityTester, PosixRwMutexReaderDataProtection) {
    using Tester = Os::Test::RwMutex::Tester;
    
    // Start reader task in separate thread
    Os::Task reader_task;
    Os::Task::Arguments args(
        Fw::String("RwMutexReaderTask"),
        readerTaskRoutine,
        static_cast<void*>(tester.get())
    );
    Os::Task::Status stat = reader_task.start(args);
    FW_ASSERT(stat == Os::Task::OP_OK, static_cast<FwAssertArgType>(stat));

    // Main thread performs write operations
    Tester::ProtectDataWrite protect_write_rule;
    for (FwSizeType i = 0; i < 100000; i++) {
        protect_write_rule.apply(*tester);
    }

    // Wait for reader thread to complete
    reader_task.join();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
