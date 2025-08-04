// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/initial/BasicTester.hpp"
#include "FppTest/state_machine/internal_instance/initial/ChoiceTester.hpp"
#include "FppTest/state_machine/internal_instance/initial/NestedTester.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmInstanceInitial::BasicTester basic("basic");
    basic.test();
}

TEST(Choice, False) {
    FppTest::SmInstanceInitial::ChoiceTester choice("choice");
    choice.testFalse();
}

TEST(Choice, True) {
    FppTest::SmInstanceInitial::ChoiceTester choice("choice");
    choice.testTrue();
}

TEST(Nested, Test) {
    FppTest::SmInstanceInitial::NestedTester nested("nested");
    nested.test();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
