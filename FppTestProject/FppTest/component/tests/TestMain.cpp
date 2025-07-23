// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
