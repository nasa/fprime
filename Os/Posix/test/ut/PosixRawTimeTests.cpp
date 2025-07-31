// ======================================================================
// \title Os/Posix/test/ut/PosixRawTimeTests.cpp
// \brief tests for posix implementation for Os::RawTime
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/test/ut/rawtime/CommonTests.hpp"
#include "Os/test/ut/rawtime/RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
