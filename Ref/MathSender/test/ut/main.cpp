// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, AddCommand) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, SubCommand) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, MultCommand) {
    Ref::Tester tester;
    tester.testMultCommand();
}

TEST(Nominal, DivideCommand) {
    Ref::Tester tester;
    tester.testDivCommand();
}

TEST(Nominal, Result) {
    Ref::Tester tester;
    tester.testResult();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
