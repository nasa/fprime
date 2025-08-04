// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "ComStubTester.hpp"

TEST(Nominal, Initial) {
    Svc::ComStubTester tester;
    tester.test_initial();
}

TEST(Nominal, BasicIo) {
    Svc::ComStubTester tester;
    tester.test_basic();
}

TEST(Nominal, Fail) {
    Svc::ComStubTester tester;
    tester.test_fail();
}

TEST(Nominal, BufferReturn) {
    Svc::ComStubTester tester;
    tester.test_buffer_return();
}

TEST(OffNominal, Retry) {
    Svc::ComStubTester tester;
    tester.test_retry();
}

TEST(OffNominal, RetryReset) {
    Svc::ComStubTester tester;
    tester.test_retry_reset();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
