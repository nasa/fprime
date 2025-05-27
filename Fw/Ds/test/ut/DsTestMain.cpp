// ======================================================================
// \title  DsTestMain.cpp
// \author bocchino
// \brief  cpp file for Ds tests
// ======================================================================

#include <gtest/gtest.h>

#include "STest/Random/Random.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
