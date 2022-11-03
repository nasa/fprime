// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Svc/ComQueue/test/ut/Tester.hpp"

TEST(Nominal, Send) {
    Svc::Tester tester;
    tester.testQueueSend();
}

TEST(Nominal, Pause) {
    Svc::Tester tester;
    tester.testQueuePause();
}

TEST(Nominal, Priority) {
    Svc::Tester tester;
    tester.testPrioritySend();
}

TEST(Nominal, Full) {
    Svc::Tester tester;
    tester.testQueueOverflow();
}

TEST(Nominal, ReadyFirst) {
    Svc::Tester tester;
    tester.testReadyFirst();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
