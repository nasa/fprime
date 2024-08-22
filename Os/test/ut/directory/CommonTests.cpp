// ======================================================================
// \title Os/test/ut/directory/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/directory/CommonTests.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

std::unique_ptr<Os::Test::Directory::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::Directory::Tester>(new Os::Test::Directory::Tester());
}

FunctionalityTester::FunctionalityTester() : tester(get_tester_implementation()) {}

void FunctionalityTester::SetUp() {
    // No setup required
}

void FunctionalityTester::TearDown() {
    // No teardown required
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Lock then unlock directory
TEST_F(FunctionalityTester, LockAndUnlockDirectory) {
    // Os::Test::Directory::Tester::LockDirectory lock_rule;
}

