// ======================================================================
// \title  ActivePhaserTestMain.cpp
// \author lestarch
// \brief  cpp file for ActivePhaser component test main function
// ======================================================================

#include "ActivePhaserTester.hpp"
#include "gtest/gtest-death-test.h"
#include "gtest/gtest.h"

TEST(Nominal, SingleChild) {
    Svc::ActivePhaserTester tester;
    tester.test_nominal_child();
}

TEST(Nominal, MultipleChildren) {
    Svc::ActivePhaserTester tester;
    tester.test_nominal_children();
}

TEST(Nominal, UnrulyChildren) {
    Svc::ActivePhaserTester tester;
    tester.test_unruly_children();
}

TEST(Lethargic, SingleChild) {
    Svc::ActivePhaserTester tester;
    tester.test_lethargic_child();
}

TEST(Lethargic, MultipleChildren) {
    Svc::ActivePhaserTester tester;
    tester.test_lethargic_children();
}

TEST(Lethargic, MultipleNonInitialChildren) {
    Svc::ActivePhaserTester tester;
    tester.test_lethargic_children(true);
}

TEST(Stress, YiWan) {
    Svc::ActivePhaserTester tester;
    tester.test_lethargic_children(false, 10000llu);
}

TEST(Stress, Rollover) {
    Svc::ActivePhaserTester tester;
    tester.test_rollover();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
