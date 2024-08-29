// ======================================================================
// \title Os/Posix/test/ut/PosixDirectoryTests.cpp
// \brief tests for posix implementation for Os::Directory
// ======================================================================
#include "Os/test/ut/directory/RulesHeaders.hpp"
#include "Os/test/ut/directory/CommonTests.hpp"
#include "Os/Posix/Task.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// Test 1
TEST_F(FunctionalityTester, Directory) {

}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
