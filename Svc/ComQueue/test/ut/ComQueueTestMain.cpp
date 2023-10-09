// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "ComQueueTester.hpp"

TEST(Nominal, Send) {
    Svc::ComQueueTester tester;
    tester.testQueueSend();
}

TEST(Nominal, Pause) {
    Svc::ComQueueTester tester;
    tester.testQueuePause();
}

TEST(Nominal, Priority) {
    Svc::ComQueueTester tester;
    tester.testPrioritySend();
}

TEST(Nominal, Full) {
    Svc::ComQueueTester tester;
    tester.testQueueOverflow();
}

TEST(Nominal, ReadyFirst) {
    Svc::ComQueueTester tester;
    tester.testReadyFirst();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
