// ======================================================================
// \title  ComRetryTestMain.cpp
// \author valdaarhun
// \brief  cpp file for ComRetry test main function
// ======================================================================

#include "ComRetryTester.hpp"

TEST(Nominal, Send) {
    Svc::ComRetryTester tester;
    tester.testBufferSend();
}

TEST(Nominal, Retry) {
    Svc::ComRetryTester tester;
    tester.testBufferRetry();
}

TEST(Nominal, RetryTillFailure) {
    Svc::ComRetryTester tester;
    tester.testBufferRetryTillFailure();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
