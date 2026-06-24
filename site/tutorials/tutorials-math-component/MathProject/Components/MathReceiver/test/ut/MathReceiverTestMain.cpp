// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "MathReceiverTester.hpp"
#include "STest/Random/Random.hpp"

/*
TEST(Nominal, ToDo) {
    MathProject::MathReceiverTester tester;
    tester.toDo();
}
*/

TEST(Nominal, AddCommand)
{
    MathProject::MathReceiverTester tester;
    tester.testAdd();
}

TEST(Nominal, SubCommand)
{
    MathProject::MathReceiverTester tester;
    tester.testSub();
}

TEST(Nominal, Throttle)
{
    MathProject::MathReceiverTester tester;
    tester.testThrottle();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
