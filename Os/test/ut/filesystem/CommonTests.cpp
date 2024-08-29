// ======================================================================
// \title Os/test/ut/filesystem/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/filesystem/CommonTests.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

// std::unique_ptr<Os::Test::FileSystem::Tester> get_tester_implementation() {
//     return std::unique_ptr<Os::Test::FileSystem::Tester>(new Os::Test::FileSystem::Tester());
// }

FunctionalityTester::FunctionalityTester() : tester() {}

void FunctionalityTester::SetUp() {
    // No setup required
}

void FunctionalityTester::TearDown() {
    // No teardown required
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Lock then unlock filesystem
TEST_F(FunctionalityTester, LockAndUnlockFileSystem) {
    // Os::Test::FileSystem::Tester::LockFileSystem lock_rule;
}

