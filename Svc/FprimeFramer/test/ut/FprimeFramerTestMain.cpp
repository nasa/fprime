// ======================================================================
// \title  FprimeFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FprimeFramer component test main function
// ======================================================================

#include "FprimeFramerTester.hpp"

TEST(Nominal, testComStatusPassThrough) {
    Svc::FprimeFramerTester tester;
    tester.testComStatusPassThrough();
}

TEST(Nominal, testFrameDeallocation) {
    Svc::FprimeFramerTester tester;
    tester.testFrameDeallocation();
}

TEST(Nominal, testNominalFraming) {
    Svc::FprimeFramerTester tester;
    tester.testNominalFraming();
}

TEST(OffNominal, OversizedAllocatorBufferIsTrimmed) {
    Svc::FprimeFramerTester tester;
    tester.testOversizedAllocatorBufferIsTrimmed();
}

TEST(OffNominal, InvalidAllocationEmitsComStatus) {
    Svc::FprimeFramerTester tester;
    tester.testInvalidAllocationEmitsComStatus();
}

TEST(OffNominal, UndersizedAllocationEmitsComStatus) {
    Svc::FprimeFramerTester tester;
    tester.testUndersizedAllocationEmitsComStatus();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
