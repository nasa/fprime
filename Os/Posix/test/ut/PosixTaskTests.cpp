#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
