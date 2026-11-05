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

TEST(CancelName, testCancelName) {
    Svc::SeqDispatcherTester tester;
    tester.testCancelName();
}

TEST(CancelName, testCancelNameNotFound) {
    Svc::SeqDispatcherTester tester;
    tester.testCancelNameNotFound();
}

TEST(CancelAll, testCancelAll) {
    Svc::SeqDispatcherTester tester;
    tester.testCancelAll();
}

TEST(CancelAll, testCancelAllNoneRunning) {
    Svc::SeqDispatcherTester tester;
    tester.testCancelAllNoneRunning();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
