// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "BufferRepeaterTester.hpp"

TEST(Nominal, TestRepeater) {
    Svc::BufferRepeaterTester tester;
    tester.testRepeater();
}

TEST(OffNominal, NoMemoryResponse) {
    Svc::BufferRepeaterTester tester;
    tester.testFailure(Svc::BufferRepeater::NO_RESPONSE_ON_OUT_OF_MEMORY);
}

TEST(OffNominal, WarningMemoryResponse) {
    Svc::BufferRepeaterTester tester;
    tester.testFailure(Svc::BufferRepeater::WARNING_ON_OUT_OF_MEMORY);
}

TEST(OffNominal, FatalMemoryResponse) {
    Svc::BufferRepeaterTester tester;
    tester.testFailure(Svc::BufferRepeater::FATAL_ON_OUT_OF_MEMORY);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
