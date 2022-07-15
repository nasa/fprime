// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Svc/ComQueue/test/ut/Tester.hpp"

TEST(Nominal, Send) {
    Svc::Tester tester;
    tester.testQueueSend();
}

TEST(Nominal, Retry) {
    Svc::Tester tester;
    tester.testRetrySend();
}

TEST(Nominal, Priority) {
    Svc::Tester tester;
    tester.testPrioritySend();
}

TEST(Nominal, Full) {
    Svc::Tester tester;
    tester.testQueueFull();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
