// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include "gtest/gtest.h"
#include "gtest/gtest-death-test.h"

TEST(Nominal, SingleChild) {
    Common::Tester tester;
    tester.test_nominal_child();
}

TEST(Nominal, MultipleChildren) {
    Common::Tester tester;
    tester.test_nominal_children();
}

TEST(Nominal, UnrulyChildren) {
    Common::Tester tester;
    tester.test_unruly_children();
}

TEST(Lethargic, SingleChild) {
    Common::Tester tester;
    tester.test_lethargic_child();
}

TEST(Lethargic, MultipleChildren) {
    Common::Tester tester;
    tester.test_lethargic_children();
}

TEST(Lethargic, MultipleNonInitialChildren) {
    Common::Tester tester;
    tester.test_lethargic_children(true);
}

TEST(Stress, YiWan) {
    Common::Tester tester;
    tester.test_lethargic_children(false, 10000llu);
}

TEST(Stress, Rollover) {

    Common::Tester tester;
    tester.test_rollover();

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
