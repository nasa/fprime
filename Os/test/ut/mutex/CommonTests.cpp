// ======================================================================
// \title Os/test/ut/mutex/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/mutex/CommonTests.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

std::unique_ptr<Os::Test::Mutex::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::Mutex::Tester>(new Os::Test::Mutex::Tester());
}

FunctionalityTester::FunctionalityTester() : tester(get_tester_implementation()) {}

void FunctionalityTester::SetUp() {
    // No setup required
}

void FunctionalityTester::TearDown() {
    // tester->m_mutex.unLock(); // Ensure the mutex is unlocked for safe destruction
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;
    unlock_rule.apply(*tester);

}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Lock then unlock mutex
TEST_F(FunctionalityTester, LockAndUnlockMutex) {
    Os::Test::Mutex::Tester::LockMutex lock_rule;
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;
    lock_rule.apply(*tester);
    unlock_rule.apply(*tester);
}

// Take then release mutex
TEST_F(FunctionalityTester, TakeAndReleaseMutex) {
    Os::Test::Mutex::Tester::TakeMutex take_rule;
    Os::Test::Mutex::Tester::ReleaseMutex release_rule;
    take_rule.apply(*tester);
    release_rule.apply(*tester);
}

// Attempt to lock a busy mutex
// QUESTION: is it good to start a Os::Task (or raw thread?) and wait for a second or something?
// TEST_F(FunctionalityTester, LockBusyMutex) {
//     Os::Test::Mutex::Tester tester;
//     Os::Test::Mutex::Tester::LockMutex lock_rule;
//     Os::Test::Mutex::Tester::LockBusyMutex lock_busy_rule;
//     lock_rule.apply(tester);
//     lock_busy_rule.apply(tester);
    // ASSERT_DEATH_IF_SUPPORTED(delete &tester, Os::Test::Mutex::Tester::ASSERT_IN_FILE_CPP);
    // EXPECT_CALL(tester.m_mutex, lock()).Times(1);
// }

// Unlock a free mutex 
// QUESTION: does not error on MacOS, but does on Linux/RHEL8 - how should this be handled?
TEST_F(FunctionalityTester, UnlockFreeMutex) {
    Os::Test::Mutex::Tester::LockMutex lock_rule;
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;
    Os::Test::Mutex::Tester::UnlockFreeMutex unlock_free_rule;
    lock_rule.apply(*tester);
    unlock_rule.apply(*tester);
    // unlock_free_rule.apply(*tester);
}

// Randomized sequence of conditioned take/release/lock/unlock
TEST_F(FunctionalityTester, RandomizedInterfaceTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::Mutex::Tester::TakeMutex take_rule;
    Os::Test::Mutex::Tester::ReleaseMutex release_rule;
    Os::Test::Mutex::Tester::LockMutex lock_rule;
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::Mutex::Tester>* rules[] = {
            &take_rule,
            &release_rule,
            &lock_rule,
            &unlock_rule,
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::Mutex::Tester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::Mutex::Tester> bounded(
            "Bounded Random Rules Scenario",
            random,
            100
    );
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);
    // add one run of unlock for safe destruction
}

