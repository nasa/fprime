// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "ComStubTester.hpp"

TEST(Common, Initial) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::UNSPECIFIED);
    tester.test_initial();
}

TEST(Sync, BasicIo) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::SYNC);
    tester.test_basic();
}

TEST(Async, BasicIo) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::ASYNC);
    tester.test_basic();
}

TEST(Sync, Fail) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::SYNC);
    tester.test_fail();
}
TEST(Async, Fail) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::ASYNC);
    tester.test_fail();
}

TEST(Nominal, BufferReturn) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::UNSPECIFIED);
    tester.test_buffer_return();
}

TEST(Sync, Retry) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::SYNC);
    tester.test_retry_sync();
}

TEST(Sync, RetryReset) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::SYNC);
    tester.test_retry_reset_sync();
}

TEST(Async, Retry) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::ASYNC);
    tester.test_retry_async();
}

TEST(Async, RetryReset) {
    Svc::ComStubTester tester(Svc::ComStubTester::TestMode::ASYNC);
    tester.test_retry_reset_async();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
