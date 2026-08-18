// ======================================================================
// \title  AosFramerTestMain.cpp
// \author Will MacCormack (Aos Modifications)
// \brief  cpp file for AosFramer component test main function
// \details modified from thomas-bc's TmFramer
// ======================================================================

#include "AosFramerTester.hpp"

TEST(AosFramer, testComStatusPassthrough) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(AosFramer, testDataReturn) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testDataReturn();
}

TEST(AosFramer, testNominalFraming) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testNominalFraming();
}

TEST(AosFramer, testSeqCountWrapAround) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testSeqCountWrapAround();
}

TEST(AosFramer, testBufferOwnershipState) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testBufferOwnershipState();
}

TEST(AosFramer, testLongPacket) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testLongPacket();
}

TEST(AosFramer, testShortPackets) {
    Svc::Ccsds::AosFramerTester tester;
    tester.testShortPackets();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
