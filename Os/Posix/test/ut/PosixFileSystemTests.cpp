// ======================================================================
// \title Os/Posix/test/ut/PosixFileSystemTests.cpp
// \brief tests for posix implementation for Os::FileSystem
// ======================================================================
#include "Os/test/ut/filesystem/RulesHeaders.hpp"
#include "Os/test/ut/filesystem/CommonTests.hpp"
#include "Os/Posix/Task.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// All tests are inherited from the common tests in CommonTests.hpp

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
