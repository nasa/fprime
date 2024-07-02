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
    // Ensure the mutex is unlocked for safe destruction
    if (this->tester->m_state == Os::Test::Mutex::Tester::MutexState::LOCKED) {
        this->tester->m_state = Os::Test::Mutex::Tester::MutexState::UNLOCKED;
        this->tester->m_mutex.unLock();
    }
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

