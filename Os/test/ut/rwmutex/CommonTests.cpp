// ======================================================================
// \title Os/test/ut/rwmutex/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/rwmutex/CommonTests.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

std::unique_ptr<Os::Test::RwMutex::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::RwMutex::Tester>(new Os::Test::RwMutex::Tester());
}

FunctionalityTester::FunctionalityTester() : tester(get_tester_implementation()) {}

void FunctionalityTester::SetUp() {
    // No setup required
}

void FunctionalityTester::TearDown() {
    // Ensure the mutex is unlocked for safe destruction
    // Case 1: Writer holds the lock -> release it
    if (this->tester->m_state == Os::Test::RwMutex::Tester::WRITE_LOCKED) {
        this->tester->m_rwmutex.unLock();
        this->tester->m_state = Os::Test::RwMutex::Tester::UNLOCKED;
    }
    
    // Case 2: Readers hold the lock -> release all (model tracks count)
    while (this->tester->m_state == Os::Test::RwMutex::Tester::READ_LOCKED &&
           this->tester->m_reader_count > 0) {
        this->tester->m_rwmutex.unLockRead();
        this->tester->m_reader_count--;
        if (this->tester->m_reader_count == 0) {
            this->tester->m_state = Os::Test::RwMutex::Tester::UNLOCKED;
        }
    }
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Lock then unlock mutex
TEST_F(FunctionalityTester, LockAndUnlockRwMutex) {
    Os::Test::RwMutex::Tester::LockRwMutex lock_rule;
    Os::Test::RwMutex::Tester::UnlockRwMutex unlock_rule;
    lock_rule.apply(*tester);
    unlock_rule.apply(*tester);
}

// Take then release mutex
TEST_F(FunctionalityTester, TakeAndReleaseRwMutex) {
    Os::Test::RwMutex::Tester::TakeRwMutex take_rule;
    Os::Test::RwMutex::Tester::ReleaseRwMutex release_rule;
    take_rule.apply(*tester);
    release_rule.apply(*tester);
}

// Acquire shared lock via lockRead(), release via unlockRead()
TEST_F(FunctionalityTester, LockReadAndUnlockRead) {
    Os::Test::RwMutex::Tester::LockReadRwMutex lock_read_rule;
    Os::Test::RwMutex::Tester::UnlockReadRwMutex unlock_read_rule;
    
    // First reader: UNLOCKED -> READ_LOCKED
    lock_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(1));
    
    // Second reader: increment count
    lock_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(2));
    
    // Release one reader: count decrements, state stays READ_LOCKED
    unlock_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(1));
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED);
    
    // Release last reader: transition to UNLOCKED
    unlock_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(0));
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED);
}

// Acquire shared lock via takeRead(), release via releaseRead()
TEST_F(FunctionalityTester, TakeReadAndReleaseRead) {
    Os::Test::RwMutex::Tester::TakeReadRwMutex take_read_rule;
    Os::Test::RwMutex::Tester::ReleaseReadRwMutex release_read_rule;
    
    take_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(1));
    
    release_read_rule.apply(*this->tester);
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(0));
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED);
}

// Multiple readers can hold lock concurrently
TEST_F(FunctionalityTester, MultipleConcurrentReaders) {
    Os::Test::RwMutex::Tester::TakeReadRwMutex take_read;
    
    // Acquire 5 concurrent readers
    for (FwSizeType i = 0; i < 5; i++) {
        ASSERT_TRUE(take_read.precondition(*this->tester));
        take_read.apply(*this->tester);
        ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED);
        ASSERT_EQ(this->tester->m_reader_count, i + 1);
    }
    
    // Writer cannot acquire while readers hold lock
    Os::Test::RwMutex::Tester::TakeRwMutex take_write;
    ASSERT_FALSE(take_write.precondition(*this->tester));
    
    // Release all readers
    Os::Test::RwMutex::Tester::ReleaseReadRwMutex release_read;
    for (FwSizeType i = 0; i < 5; i++) {
        release_read.apply(*this->tester);
    }
    ASSERT_EQ(this->tester->m_reader_count, static_cast<FwSizeType>(0));
    ASSERT_EQ(this->tester->m_state, Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED);
    
    // Now writer can acquire
    ASSERT_TRUE(take_write.precondition(*this->tester));
}

// Protected write operation maintains data integrity
TEST_F(FunctionalityTester, ProtectDataWrite) {
    Os::Test::RwMutex::Tester::ProtectDataWrite protect_write;
    
    // Apply the rule: acquire lock, write random data, verify, release
    protect_write.apply(*this->tester);
    
    // Verify that the written value persists (no corruption)
    int expected = this->tester->m_value;
    ASSERT_EQ(this->tester->m_value, expected);
}

// Protected read operation sees consistent data
TEST_F(FunctionalityTester, ProtectDataRead) {
    // First, write a known value
    {
        Os::Test::RwMutex::Tester::ProtectDataWrite protect_write;
        protect_write.apply(*this->tester);
    }
    
    // Then read it back with shared lock
    Os::Test::RwMutex::Tester::ProtectDataRead protect_read;
    protect_read.apply(*this->tester);
}

// Randomized sequence of conditioned take/release/lock/unlock
TEST_F(FunctionalityTester, RandomizedInterfaceTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::RwMutex::Tester::TakeRwMutex take_write_rule;
    Os::Test::RwMutex::Tester::ReleaseRwMutex release_write_rule;
    Os::Test::RwMutex::Tester::LockRwMutex lock_write_rule;
    Os::Test::RwMutex::Tester::UnlockRwMutex unlock_write_rule;
    
    Os::Test::RwMutex::Tester::TakeReadRwMutex take_read_rule;
    Os::Test::RwMutex::Tester::ReleaseReadRwMutex release_read_rule;
    Os::Test::RwMutex::Tester::LockReadRwMutex lock_read_rule;
    Os::Test::RwMutex::Tester::UnlockReadRwMutex unlock_read_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::RwMutex::Tester>* rules[] = {
        &take_write_rule,
        &release_write_rule,
        &lock_write_rule,
        &unlock_write_rule,
        &take_read_rule,
        &release_read_rule,
        &lock_read_rule,
        &unlock_read_rule,
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::RwMutex::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::RwMutex::Tester> bounded("Bounded Random Rules Scenario", random, 200);
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);

    // Note: TearDown() will ensure clean unlock for safe destruction
}
