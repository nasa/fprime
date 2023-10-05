#include "testgenTester.hpp"

TEST(Nominal, TestAddCommand) {
    Ref::testgenTester tester;
    tester.testAddCommand();
}
TEST(Nominal, TestSubCommand) {
    Ref::testgenTester tester;
    tester.testSubCommand();
}
TEST(Nominal, TestMultCommand) {
    Ref::testgenTester tester;
    tester.testMultCommand();
}
TEST(Nominal, TestDivCommand) {
    Ref::testgenTester tester;
    tester.testDivCommand();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
