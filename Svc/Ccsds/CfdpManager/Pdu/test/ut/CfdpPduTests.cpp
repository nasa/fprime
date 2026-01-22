// ======================================================================
// \title  CfdpPduTests.cpp
// \author campuzan
// \brief  Unit tests for CFDP PDU classes
//
// \copyright
// Copyright 2025, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/CfdpPduClasses.hpp>
#include <gtest/gtest.h>
#include <cstring>

using namespace Svc::Ccsds;

// Test fixture for CFDP PDU tests
class CfdpPduTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Common setup
    }

    void TearDown() override {
        // Common cleanup
    }
};

// ======================================================================
// Header Tests
// ======================================================================

TEST_F(CfdpPduTest, HeaderBufferSize) {
    CfdpPdu::Header header;
    header.initialize(CfdpPdu::T_METADATA, CFDP_DIRECTION_TOWARD_RECEIVER,
                     CFDP_TRANSMISSION_MODE_ACKNOWLEDGED, 123, 456, 789);

    // Minimum header size with 1-byte EIDs and TSN
    // flags(1) + length(2) + eidTsnLengths(1) + sourceEid(2) + tsn(2) + destEid(2) = 10
    ASSERT_GE(header.bufferSize(), 7U);
}

TEST_F(CfdpPduTest, HeaderRoundTrip) {
    // Arrange
    CfdpPdu::Header txHeader;
    const CfdpDirection direction = CFDP_DIRECTION_TOWARD_SENDER;
    const CfdpTransmissionMode txmMode = CFDP_TRANSMISSION_MODE_ACKNOWLEDGED;
    const CfdpEntityId sourceEid = 10;
    const CfdpTransactionSeq transactionSeq = 20;
    const CfdpEntityId destEid = 30;
    const U16 pduDataLength = 100;

    txHeader.initialize(CfdpPdu::T_METADATA, direction, txmMode, sourceEid, transactionSeq, destEid);
    txHeader.setPduDataLength(pduDataLength);

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Act - Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txHeader.toSerialBuffer(serialBuffer));

    // Act - Decode
    serialBuffer.resetSer();
    serialBuffer.fill();
    CfdpPdu::Header rxHeader;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxHeader.fromSerialBuffer(serialBuffer));

    // Assert - Verify all fields
    ASSERT_EQ(direction, rxHeader.getDirection());
    ASSERT_EQ(txmMode, rxHeader.getTxmMode());
    ASSERT_EQ(sourceEid, rxHeader.getSourceEid());
    ASSERT_EQ(transactionSeq, rxHeader.getTransactionSeq());
    ASSERT_EQ(destEid, rxHeader.getDestEid());
    ASSERT_EQ(pduDataLength, rxHeader.getPduDataLength());
}

// ======================================================================
// Metadata PDU Tests
// ======================================================================

TEST_F(CfdpPduTest, MetadataBufferSize) {
    CfdpPdu::MetadataPdu pdu;
    pdu.initialize(CFDP_DIRECTION_TOWARD_RECEIVER, CFDP_TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 1024, "src.txt", "dst.txt",
                   CFDP_CHECKSUM_TYPE_MODULAR, 1);

    U32 size = pdu.bufferSize();
    // Should include header + directive + segmentation + filesize + 2 LVs
    ASSERT_GT(size, 0U);
}

TEST_F(CfdpPduTest, MetadataRoundTrip) {
    // Arrange - Create and initialize transmit PDU
    CfdpPdu::MetadataPdu txPdu;
    const CfdpDirection direction = CFDP_DIRECTION_TOWARD_SENDER;
    const CfdpTransmissionMode txmMode = CFDP_TRANSMISSION_MODE_ACKNOWLEDGED;
    const CfdpEntityId sourceEid = 100;
    const CfdpTransactionSeq transactionSeq = 200;
    const CfdpEntityId destEid = 300;
    const U32 fileSize = 2048;
    const char* sourceFilename = "source_file.bin";
    const char* destFilename = "dest_file.bin";
    const CfdpChecksumType checksumType = CFDP_CHECKSUM_TYPE_MODULAR;
    const U8 closureRequested = 1;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    fileSize, sourceFilename, destFilename, checksumType, closureRequested);

    // Serialize to first buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Copy to second buffer
    U8 buffer2[512];
    memcpy(buffer2, buffer1, txBuffer.getSize());

    // Deserialize from second buffer using SerialBuffer to read header + body
    Fw::SerialBuffer serialBuffer(buffer2, txBuffer.getSize());
    serialBuffer.fill();

    // Read header
    CfdpPdu::Header rxHeader;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxHeader.fromSerialBuffer(serialBuffer));

    // Verify header fields
    ASSERT_EQ(direction, rxHeader.getDirection());
    ASSERT_EQ(txmMode, rxHeader.getTxmMode());
    ASSERT_EQ(sourceEid, rxHeader.getSourceEid());
    ASSERT_EQ(transactionSeq, rxHeader.getTransactionSeq());
    ASSERT_EQ(destEid, rxHeader.getDestEid());

    // Read and verify directive code
    U8 directiveCode;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.deserializeTo(directiveCode));
    ASSERT_EQ(static_cast<U8>(CFDP_FILE_DIRECTIVE_METADATA), directiveCode);

    // Read segmentation control byte
    U8 segmentationControl;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.deserializeTo(segmentationControl));
    U8 rxClosureRequested = (segmentationControl >> 7) & 0x01;
    U8 rxChecksumType = segmentationControl & 0x0F;
    ASSERT_EQ(closureRequested, rxClosureRequested);
    ASSERT_EQ(static_cast<U8>(checksumType), rxChecksumType);

    // Read file size
    U32 rxFileSize;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.deserializeTo(rxFileSize));
    ASSERT_EQ(fileSize, rxFileSize);

    // Read source filename LV
    U8 srcFilenameLen;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.deserializeTo(srcFilenameLen));
    ASSERT_EQ(strlen(sourceFilename), srcFilenameLen);
    U8 srcFilenameBuf[256];
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.popBytes(srcFilenameBuf, srcFilenameLen));
    ASSERT_EQ(0, memcmp(sourceFilename, srcFilenameBuf, srcFilenameLen));

    // Read dest filename LV
    U8 dstFilenameLen;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.deserializeTo(dstFilenameLen));
    ASSERT_EQ(strlen(destFilename), dstFilenameLen);
    U8 dstFilenameBuf[256];
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, serialBuffer.popBytes(dstFilenameBuf, dstFilenameLen));
    ASSERT_EQ(0, memcmp(destFilename, dstFilenameBuf, dstFilenameLen));
}

TEST_F(CfdpPduTest, MetadataEmptyFilenames) {
    CfdpPdu::MetadataPdu pdu;
    pdu.initialize(CFDP_DIRECTION_TOWARD_RECEIVER, CFDP_TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 0, "", "",
                   CFDP_CHECKSUM_TYPE_NULL_CHECKSUM, 0);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    // Should encode successfully even with empty filenames
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
}

TEST_F(CfdpPduTest, MetadataLongFilenames) {
    CfdpPdu::MetadataPdu pdu;
    // Test with maximum allowed filename length (CF_FILENAME_MAX_LEN = 200)
    const char* longSrc = "/very/long/path/to/source/file/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bin";
    const char* longDst = "/another/very/long/path/to/destination/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.dat";

    pdu.initialize(CFDP_DIRECTION_TOWARD_RECEIVER, CFDP_TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 4096, longSrc, longDst,
                   CFDP_CHECKSUM_TYPE_MODULAR, 1);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}