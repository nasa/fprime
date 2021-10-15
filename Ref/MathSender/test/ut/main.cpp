// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "STest/Random/Random.hpp"
#include "Tester.hpp"

TEST(Nominal, AddCommand) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, SubCommand) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, MulCommand) {
    Ref::Tester tester;
    tester.testMulCommand();
}

TEST(Nominal, DivCommand) {
    Ref::Tester tester;
    tester.testDivCommand();
}

TEST(Nominal, Result) {
    Ref::Tester tester;
    tester.testResult();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  STest::Random::seed();
  return RUN_ALL_TESTS();
}
