// ======================================================================
// \title  DataStructuresTestMain.cpp
// \author bocchino
// \brief  cpp file for DataStructures tests
// ======================================================================

#include <gtest/gtest.h>

#include "STest/Random/Random.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
