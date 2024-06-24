// ======================================================================
// \title Os/Posix/test/ut/PosixMutexTests.cpp
// \brief tests for posix implementation for Os::Mutex
// ======================================================================
#include "Os/test/ut/mutex/RulesHeaders.hpp"
#include "Os/test/ut/mutex/CommonTests.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// Attempt to delete a locked mutex - expect an assertion
TEST_F(FunctionalityTester, PosixDeleteLockedMutex) {
    Os::Test::Mutex::Tester::LockMutex lock_rule;
    Os::Test::Mutex::Tester::UnlockMutex unlock_rule;
    lock_rule.apply(*tester);
    ASSERT_DEATH_IF_SUPPORTED(delete &tester, Os::Test::Mutex::Tester::ASSERT_IN_MUTEX_CPP);
}


int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
