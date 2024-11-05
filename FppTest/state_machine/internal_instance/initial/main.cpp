// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/initial/Basic.hpp"
#include "FppTest/state_machine/internal_instance/initial/Choice.hpp"
#include "FppTest/state_machine/internal_instance/initial/Nested.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmInstanceInitial::Basic basic("basic");
    basic.test();
}

TEST(Choice, False) {
    FppTest::SmInstanceInitial::Choice choice("choice");
    choice.testFalse();
}

TEST(Choice, True) {
    FppTest::SmInstanceInitial::Choice choice("choice");
    choice.testTrue();
}

TEST(Nested, Test) {
    FppTest::SmInstanceInitial::Nested nested("nested");
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
