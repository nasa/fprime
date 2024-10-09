// ======================================================================
// \title Os/Generic/test/ut/PriorityQueueTests.cpp
// \brief tests using generic priority implementation for Os::Queue interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Generic/PriorityQueue.hpp"
#include "STest/Random/Random.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
