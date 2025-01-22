// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/initial/Basic.hpp"
#include "FppTest/state_machine/internal/initial/Choice.hpp"
#include "FppTest/state_machine/internal/initial/Nested.hpp"
#include "STest/STest/Random/Random.hpp"

TEST(Basic, Test) {
    FppTest::SmInitial::Basic basic;
    basic.test();
}

TEST(Choice, False) {
    FppTest::SmInitial::Choice choice;
    choice.testFalse();
}

TEST(Choice, True) {
    FppTest::SmInitial::Choice choice;
    choice.testTrue();
}

TEST(Nested, Test) {
    FppTest::SmInitial::Nested nested;
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
