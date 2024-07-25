// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, testDispatch) {
    components::Tester tester;
    tester.testDispatch();
}

TEST(Nominal, testLogStatus) {
    components::Tester tester;
    tester.testLogStatus();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
