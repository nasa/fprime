// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "SeqDispatcherTester.hpp"

TEST(Nominal, testDispatch) {
    Svc::Tester tester;
    tester.testDispatch();
}

TEST(Nominal, testLogStatus) {
    Svc::Tester tester;
    tester.testLogStatus();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
