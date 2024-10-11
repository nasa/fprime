// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "SeqDispatcherTester.hpp"

TEST(Nominal, testDispatch) {
    Svc::SeqDispatcherTester tester;
    tester.testDispatch();
}

TEST(Nominal, testLogStatus) {
    Svc::SeqDispatcherTester tester;
    tester.testLogStatus();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
