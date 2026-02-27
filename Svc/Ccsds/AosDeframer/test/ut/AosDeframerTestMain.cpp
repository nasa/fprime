// ======================================================================
// \title  AosDeframerTestMain.cpp
// \author Auto-generated
// \brief  cpp file for AosDeframer component test main function
// ======================================================================

#include "AosDeframerTester.hpp"

// ----------------------------------------------------------------------
// Tests - Basic Validation
// ----------------------------------------------------------------------

TEST(AosDeframer, testNominalDeframing) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(AosDeframer, testDataReturn) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testDataReturn();
}

TEST(AosDeframer, testInvalidScId) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidScId();
}

TEST(AosDeframer, testInvalidVcId) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidVcId();
}

TEST(AosDeframer, testInvalidFrameLength) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidFrameLength();
}

TEST(AosDeframer, testInvalidFecf) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidFecf();
}

TEST(AosDeframer, testInvalidTfvn) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidTfvn();
}

TEST(AosDeframer, testVcFrameCountGap) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testVcFrameCountGap();
}

// testAcceptAllVcid removed: accept-all-VCID mode is not supported.

// ----------------------------------------------------------------------
// Tests - M_PDU Processing
// ----------------------------------------------------------------------

TEST(AosDeframer, testFhpAtOffset) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testFhpAtOffset();
}

TEST(AosDeframer, testFhpNoPacketStart) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testFhpNoPacketStart();
}

TEST(AosDeframer, testFhpIdleDataOnly) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testFhpIdleDataOnly();
}

TEST(AosDeframer, testMultiplePacketsInFrame) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testMultiplePacketsInFrame();
}

// ----------------------------------------------------------------------
// Tests - Spanning Packets
// ----------------------------------------------------------------------

TEST(AosDeframer, testSpanningPacketTwoFrames) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketTwoFrames();
}

TEST(AosDeframer, testSpanningPacketFourFrames) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketFourFrames();
}

TEST(AosDeframer, testSpanningPacketContinuation) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketContinuation();
}

TEST(AosDeframer, testSpanningPacketAllocFailureEvent) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketAllocFailureEvent();
}

// ----------------------------------------------------------------------
// Tests - SPP Extraction
// ----------------------------------------------------------------------

TEST(AosDeframer, testSppExtraction) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSppExtraction();
}

TEST(AosDeframer, testSppIdlePacketFiltering) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSppIdlePacketFiltering();
}

TEST(AosDeframer, testSppSequenceCount) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSppSequenceCount();
}

// ----------------------------------------------------------------------
// Tests - EPP Extraction
// ----------------------------------------------------------------------

TEST(AosDeframer, testEppExtraction) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppExtraction();
}

TEST(AosDeframer, testEppIdlePacket) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppIdlePacket();
}

TEST(AosDeframer, testEppFillPacket) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppFillPacket();
}

TEST(AosDeframer, testInvalidPvnVersion) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testInvalidPvnVersion();
}

// ----------------------------------------------------------------------
// Tests - Configuration
// ----------------------------------------------------------------------

TEST(AosDeframer, testFecfDisabled) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testFecfDisabled();
}

TEST(AosDeframer, testPvnMaskSppOnly) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testPvnMaskSppOnly();
}

TEST(AosDeframer, testPvnMaskEppOnly) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testPvnMaskEppOnly();
}

// ----------------------------------------------------------------------
// Tests - Telemetry
// ----------------------------------------------------------------------

TEST(AosDeframer, testFrameCountTelemetry) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testFrameCountTelemetry();
}

TEST(AosDeframer, testCrcErrorCountTelemetry) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testCrcErrorCountTelemetry();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
