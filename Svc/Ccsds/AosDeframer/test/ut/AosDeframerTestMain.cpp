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

TEST(AosDeframer, testSpanningPacketAbandonedOnVcGap) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketAbandonedOnVcGap();
}

TEST(AosDeframer, testSpanningPacketMaintainedOnOIDFrame) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketMaintainedOnOIDFrame();
}

TEST(AosDeframer, testSpanningPacketAbandonedOnPrematureFhp) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSpanningPacketAbandonedOnPrematureFhp();
}

TEST(AosDeframer, testSppHeaderSpansFrame) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSppHeaderSpansFrame();
}

TEST(AosDeframer, testEppHeaderSpansFrame) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppHeaderSpansFrame();
}

TEST(AosDeframer, testAllocFailureNextPacketExtracted) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testAllocFailureNextPacketExtracted();
}

// ----------------------------------------------------------------------
// Tests - SPP Extraction
// ----------------------------------------------------------------------

TEST(AosDeframer, testSppIdlePacketFiltering) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testSppIdlePacketFiltering();
}

// ----------------------------------------------------------------------
// Tests - EPP Extraction
// ----------------------------------------------------------------------

TEST(AosDeframer, testEppExtraction) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppExtraction();
}

TEST(AosDeframer, testEppLengthOfLength) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppLengthOfLength();
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

// ----------------------------------------------------------------------
// Tests - Robustness against untrusted inputs
// ----------------------------------------------------------------------

TEST(AosDeframer, testUntrustedFhp) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testUntrustedFhp();
}

// ----------------------------------------------------------------------
// Tests - Security regression (CVE: EPP integer overflow → heap buffer overflow)
// ----------------------------------------------------------------------

// Regression test: single-frame delivery of the CVE PoC payload.
// Verifies that the integer overflow guard in sizeEppPacket prevents the
// heap buffer overflow on 32-bit targets and that the component handles
// the resulting ~4 GB size gracefully on 64-bit hosts.
TEST(AosDeframer, testEppSizeOverflowRejected) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppSizeOverflowRejected();
}

// Regression test: same CVE attack with the EPP header split across two frames.
// Exercises the header accumulation path in appendToSpanningPacket together with
// the overflow guard so that neither the direct nor the spanning delivery vector
// can bypass the fix.
TEST(AosDeframer, testEppSizeOverflowHeaderSpansFrame) {
    Svc::Ccsds::AosDeframerTester tester;
    tester.testEppSizeOverflowHeaderSpansFrame();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
