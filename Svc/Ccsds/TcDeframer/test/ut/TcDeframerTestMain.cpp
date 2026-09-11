// ======================================================================
// \title  TcDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component test main function
// ======================================================================

#include "TcDeframerTester.hpp"

TEST(TcDeframer, testDataReturn) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testDataReturn();
}

TEST(TcDeframer, testNominalDeframing) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(TcDeframer, testInvalidScId) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidScId();
}

TEST(TcDeframer, testInvalidVcId) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidVcId();
}

TEST(TcDeframer, testInvalidLengthToken) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidLengthToken();
}

TEST(TcDeframer, testInvalidCrc) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testInvalidCrc();
}

TEST(TcDeframer, testSegmentedUnsegmentedPassthrough) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedUnsegmentedPassthrough();
}

TEST(TcDeframer, testSegmentedTwoSegments) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedTwoSegments();
}

TEST(TcDeframer, testSegmentedManySegments) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedManySegments();
}

TEST(TcDeframer, testSegmentedUnexpectedSegment) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedUnexpectedSegment();
}

TEST(TcDeframer, testSegmentedFirstRestartsPacket) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedFirstRestartsPacket();
}

TEST(TcDeframer, testSegmentedUnsegmentedAbandonsPacket) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedUnsegmentedAbandonsPacket();
}

TEST(TcDeframer, testSegmentedInvalidMapId) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedInvalidMapId();
}

TEST(TcDeframer, testSegmentedAllocFailure) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedAllocFailure();
}

TEST(TcDeframer, testSegmentedOverflow) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedOverflow();
}

TEST(TcDeframer, testSegmentedInFlightLimit) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedInFlightLimit();
}

TEST(TcDeframer, testSegmentedEmptySegment) {
    Svc::Ccsds::TcDeframerTester tester;
    tester.testSegmentedEmptySegment();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
