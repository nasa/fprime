// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, TestRepeater) {
    Svc::Tester tester;
    tester.testRepeater();
}

TEST(OffNominal, NoMemoryResponse) {
    Svc::Tester tester;
    tester.testFailure(Svc::BufferRepeater::NO_RESPONSE_ON_OUT_OF_MEMORY);
}

TEST(OffNominal, WarningMemoryResponse) {
    Svc::Tester tester;
    tester.testFailure(Svc::BufferRepeater::WARNING_ON_OUT_OF_MEMORY);
}

TEST(OffNominal, FatalMemoryResponse) {
    Svc::Tester tester;
    tester.testFailure(Svc::BufferRepeater::FATAL_ON_OUT_OF_MEMORY);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
