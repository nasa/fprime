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

TEST(RunArgs, testRunArgsWithValidArguments) {
    Svc::SeqDispatcherTester tester;
    tester.testRunArgsWithValidArguments();
}

TEST(RunArgs, testRunArgsWithMaxSizedArguments) {
    Svc::SeqDispatcherTester tester;
    tester.testRunArgsWithMaxSizedArguments();
}

TEST(RunArgs, testRunArgsNoSequencersAvailable) {
    Svc::SeqDispatcherTester tester;
    tester.testRunArgsNoSequencersAvailable();
}

TEST(RunArgs, testRunArgsBlockingVsNonBlocking) {
    Svc::SeqDispatcherTester tester;
    tester.testRunArgsBlockingVsNonBlocking();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
