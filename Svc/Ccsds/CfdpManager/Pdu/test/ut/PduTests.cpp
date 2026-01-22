// ======================================================================
// \title  PduTests.cpp
// \author campuzan
// \brief  Unit tests for CFDP PDU classes
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <gtest/gtest.h>
#include <cstring>

using namespace Svc::Ccsds;
using namespace Svc::Ccsds::Cfdp;

// Test fixture for CFDP PDU tests
class PduTest : public ::testing::Test {
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

TEST_F(PduTest, HeaderBufferSize) {
    Pdu::Header header;
    header.initialize(Pdu::T_METADATA, DIRECTION_TOWARD_RECEIVER,
                     TRANSMISSION_MODE_ACKNOWLEDGED, 123, 456, 789);

    // Minimum header size with 1-byte EIDs and TSN
    // flags(1) + length(2) + eidTsnLengths(1) + sourceEid(2) + tsn(2) + destEid(2) = 10
    ASSERT_GE(header.bufferSize(), 7U);
}

TEST_F(PduTest, HeaderRoundTrip) {
    // Arrange
    Pdu::Header txHeader;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const TransmissionMode txmMode = TRANSMISSION_MODE_ACKNOWLEDGED;
    const CfdpEntityId sourceEid = 10;
    const CfdpTransactionSeq transactionSeq = 20;
    const CfdpEntityId destEid = 30;
    const U16 pduDataLength = 100;

    txHeader.initialize(Pdu::T_METADATA, direction, txmMode, sourceEid, transactionSeq, destEid);
    txHeader.setPduDataLength(pduDataLength);

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Act - Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txHeader.toSerialBuffer(serialBuffer));

    // Act - Decode
    serialBuffer.resetSer();
    serialBuffer.fill();
    Pdu::Header rxHeader;
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

TEST_F(PduTest, MetadataBufferSize) {
    Pdu::MetadataPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 1024, "src.txt", "dst.txt",
                   CHECKSUM_TYPE_MODULAR, 1);

    U32 size = pdu.bufferSize();
    // Should include header + directive + segmentation + filesize + 2 LVs
    ASSERT_GT(size, 0U);
}

TEST_F(PduTest, MetadataRoundTrip) {
    // Arrange - Create and initialize transmit PDU
    Pdu::MetadataPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const TransmissionMode txmMode = TRANSMISSION_MODE_ACKNOWLEDGED;
    const CfdpEntityId sourceEid = 100;
    const CfdpTransactionSeq transactionSeq = 200;
    const CfdpEntityId destEid = 300;
    const CfdpFileSize fileSize = 2048;
    const char* sourceFilename = "source_file.bin";
    const char* destFilename = "dest_file.bin";
    const ChecksumType checksumType = CHECKSUM_TYPE_MODULAR;
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
    Pdu::Header rxHeader;
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
    ASSERT_EQ(static_cast<U8>(FILE_DIRECTIVE_METADATA), directiveCode);

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

TEST_F(PduTest, MetadataEmptyFilenames) {
    Pdu::MetadataPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 0, "", "",
                   CHECKSUM_TYPE_NULL_CHECKSUM, 0);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    // Should encode successfully even with empty filenames
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
}

TEST_F(PduTest, MetadataLongFilenames) {
    Pdu::MetadataPdu pdu;
    // Test with maximum allowed filename length (CF_FILENAME_MAX_LEN = 200)
    const char* longSrc = "/very/long/path/to/source/file/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.bin";
    const char* longDst = "/another/very/long/path/to/destination/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.dat";

    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 4096, longSrc, longDst,
                   CHECKSUM_TYPE_MODULAR, 1);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
}

// ======================================================================
// File Data PDU Tests
// ======================================================================

TEST_F(PduTest, FileDataBufferSize) {
    Pdu::FileDataPdu pdu;
    const U8 testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 100, sizeof(testData), testData);

    U32 size = pdu.bufferSize();
    // Should include header + offset(4) + data(5)
    ASSERT_GT(size, 0U);
    // Verify expected size
    U32 expectedSize = pdu.asHeader().bufferSize() + 4 + sizeof(testData);
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, FileDataRoundTrip) {
    // Arrange - Create transmit PDU with test data
    Pdu::FileDataPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_RECEIVER;
    const TransmissionMode txmMode = TRANSMISSION_MODE_UNACKNOWLEDGED;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const U32 fileOffset = 1024;
    const U8 testData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};
    const U16 dataSize = sizeof(testData);

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    fileOffset, dataSize, testData);

    // Serialize to buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Deserialize from buffer
    Pdu::FileDataPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer1, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header fields
    const Pdu::Header& header = rxPdu.asHeader();
    EXPECT_EQ(Pdu::T_FILE_DATA, header.getType());
    EXPECT_EQ(direction, header.getDirection());
    EXPECT_EQ(txmMode, header.getTxmMode());
    EXPECT_EQ(sourceEid, header.getSourceEid());
    EXPECT_EQ(transactionSeq, header.getTransactionSeq());
    EXPECT_EQ(destEid, header.getDestEid());

    // Verify file data fields
    EXPECT_EQ(fileOffset, rxPdu.getOffset());
    EXPECT_EQ(dataSize, rxPdu.getDataSize());
    ASSERT_NE(nullptr, rxPdu.getData());
    EXPECT_EQ(0, memcmp(testData, rxPdu.getData(), dataSize));
}

TEST_F(PduTest, FileDataEmptyPayload) {
    // Test with zero-length data
    Pdu::FileDataPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 0, 0, nullptr);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    // Should encode successfully even with no data
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);
}

TEST_F(PduTest, FileDataLargePayload) {
    // Test with maximum reasonable payload
    const U16 largeSize = 1024;
    U8 largeData[largeSize];
    for (U16 i = 0; i < largeSize; ++i) {
        largeData[i] = static_cast<U8>(i & 0xFF);
    }

    Pdu::FileDataPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, 999999, largeSize, largeData);

    U8 buffer[2048];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, pdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::FileDataPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(largeSize, rxPdu.getDataSize());
    EXPECT_EQ(0, memcmp(largeData, rxPdu.getData(), largeSize));
}

// ======================================================================
// EOF PDU Tests
// ======================================================================

TEST_F(PduTest, EofBufferSize) {
    Pdu::EofPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0x12345678, 4096);

    U32 size = pdu.bufferSize();
    // Should include header + directive(1) + condition(1) + checksum(4) + filesize(sizeof(CfdpFileSize))
    ASSERT_GT(size, 0U);
    U32 expectedSize = pdu.asHeader().bufferSize() + sizeof(U8) + sizeof(U8) + sizeof(U32) + sizeof(CfdpFileSize);
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, EofRoundTrip) {
    // Arrange - Create transmit PDU
    Pdu::EofPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_RECEIVER;
    const TransmissionMode txmMode = TRANSMISSION_MODE_UNACKNOWLEDGED;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const ConditionCode conditionCode = CONDITION_CODE_NO_ERROR;
    const U32 checksum = 0xDEADBEEF;
    const CfdpFileSize fileSize = 65536;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    conditionCode, checksum, fileSize);

    // Serialize to buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Deserialize from buffer
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer1, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header fields
    const Pdu::Header& header = rxPdu.asHeader();
    EXPECT_EQ(Pdu::T_EOF, header.getType());
    EXPECT_EQ(direction, header.getDirection());
    EXPECT_EQ(txmMode, header.getTxmMode());
    EXPECT_EQ(sourceEid, header.getSourceEid());
    EXPECT_EQ(transactionSeq, header.getTransactionSeq());
    EXPECT_EQ(destEid, header.getDestEid());

    // Verify EOF-specific fields
    EXPECT_EQ(conditionCode, rxPdu.getConditionCode());
    EXPECT_EQ(checksum, rxPdu.getChecksum());
    EXPECT_EQ(fileSize, rxPdu.getFileSize());
}

TEST_F(PduTest, EofWithError) {
    // Test with error condition code
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, CONDITION_CODE_FILE_CHECKSUM_FAILURE, 0, 0);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    // Should encode successfully even with error condition
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(CONDITION_CODE_FILE_CHECKSUM_FAILURE, rxPdu.getConditionCode());
}

TEST_F(PduTest, EofZeroValues) {
    // Test with all zero values
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0, 0);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(0U, rxPdu.getChecksum());
    EXPECT_EQ(0U, rxPdu.getFileSize());
}

TEST_F(PduTest, EofLargeValues) {
    // Test with maximum U32 values
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, TRANSMISSION_MODE_ACKNOWLEDGED,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0xFFFFFFFF, 0xFFFFFFFF);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(0xFFFFFFFFU, rxPdu.getChecksum());
    EXPECT_EQ(0xFFFFFFFFU, rxPdu.getFileSize());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}