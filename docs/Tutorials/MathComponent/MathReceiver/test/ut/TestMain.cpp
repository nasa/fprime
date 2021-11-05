// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, AddOperationTest) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, SubOperationTest) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, MultOperationTest) {
    Ref::Tester tester;
    tester.testMultCommand();
}

TEST(Nominal, DivideOperationTest) {
    Ref::Tester tester;
    tester.testDivCommand();
}

TEST(Nominal, ThrottleTest) {
    Ref::Tester tester;
    tester.testThrottle();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
