#include "Tester.hpp"

TEST(Nominal, TestAddCommand) {
    Ref::Tester tester;
    tester.testAddCommand();
}
TEST(Nominal, TestSubCommand) {
    Ref::Tester tester;
    tester.testSubCommand();
}
TEST(Nominal, TestMultCommand) {
    Ref::Tester tester;
    tester.testMultCommand();
}
TEST(Nominal, TestDivCommand) {
    Ref::Tester tester;
    tester.testDivCommand();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
