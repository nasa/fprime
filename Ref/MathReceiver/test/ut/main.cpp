// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Add) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, Sub) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, Mul) {
    Ref::Tester tester;
    tester.testMulCommand();
}

TEST(Nominal, Div) {
    Ref::Tester tester;
    tester.testDivCommand();
}

TEST(Nominal, Throttle) {
    Ref::Tester tester;
    tester.testThrottle();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
