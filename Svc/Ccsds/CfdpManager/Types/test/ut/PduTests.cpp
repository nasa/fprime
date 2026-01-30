// ======================================================================
// \title  PduTests.cpp
// \author campuzan
// \brief  Unit tests for CFDP PDU classes
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Pdu.hpp>
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
                     Cfdp::Class::CLASS_2, 123, 456, 789);

    // Minimum header size with 1-byte EIDs and TSN
    // flags(1) + length(2) + eidTsnLengths(1) + sourceEid(2) + tsn(2) + destEid(2) = 10
    ASSERT_GE(header.getBufferSize(), 7U);
}

TEST_F(PduTest, HeaderRoundTrip) {
    // Arrange
    Pdu::Header txHeader;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, 1024, "src.txt", "dst.txt",
                   CHECKSUM_TYPE_MODULAR, 1);

    U32 size = pdu.getBufferSize();
    // Should include header + directive + segmentation + filesize + 2 LVs
    ASSERT_GT(size, 0U);
}

TEST_F(PduTest, MetadataRoundTrip) {
    // Arrange - Create and initialize transmit PDU
    Pdu::MetadataPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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

    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, 100, sizeof(testData), testData);

    U32 size = pdu.getBufferSize();
    // Should include header + offset(4) + data(5)
    ASSERT_GT(size, 0U);
    // Verify expected size
    U32 expectedSize = pdu.asHeader().getBufferSize() + 4 + sizeof(testData);
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, FileDataRoundTrip) {
    // Arrange - Create transmit PDU with test data
    Pdu::FileDataPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_RECEIVER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_1;
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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
    pdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0x12345678, 4096);

    U32 size = pdu.getBufferSize();
    // Should include header + directive(1) + condition(1) + checksum(4) + filesize(sizeof(CfdpFileSize))
    ASSERT_GT(size, 0U);
    U32 expectedSize = pdu.asHeader().getBufferSize() + sizeof(U8) + sizeof(U8) + sizeof(U32) + sizeof(CfdpFileSize);
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, EofRoundTrip) {
    // Arrange - Create transmit PDU
    Pdu::EofPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_RECEIVER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_1;
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
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
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

// ======================================================================
// FIN PDU Tests
// ======================================================================

TEST_F(PduTest, FinBufferSize) {
    Pdu::FinPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_RETAINED);

    U32 size = pdu.getBufferSize();
    // Should include header + directive(1) + flags(1) = header + 2
    ASSERT_GT(size, 0U);
    U32 expectedSize = pdu.asHeader().getBufferSize() + 2;
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, FinRoundTrip) {
    // Arrange - Create transmit PDU
    Pdu::FinPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const ConditionCode conditionCode = CONDITION_CODE_NO_ERROR;
    const FinDeliveryCode deliveryCode = FIN_DELIVERY_CODE_COMPLETE;
    const FinFileStatus fileStatus = FIN_FILE_STATUS_RETAINED;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    conditionCode, deliveryCode, fileStatus);

    // Serialize to buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Deserialize from buffer
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer1, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header fields
    const Pdu::Header& header = rxPdu.asHeader();
    EXPECT_EQ(Pdu::T_FIN, header.getType());
    EXPECT_EQ(direction, header.getDirection());
    EXPECT_EQ(txmMode, header.getTxmMode());
    EXPECT_EQ(sourceEid, header.getSourceEid());
    EXPECT_EQ(transactionSeq, header.getTransactionSeq());
    EXPECT_EQ(destEid, header.getDestEid());

    // Verify FIN-specific fields
    EXPECT_EQ(conditionCode, rxPdu.getConditionCode());
    EXPECT_EQ(deliveryCode, rxPdu.getDeliveryCode());
    EXPECT_EQ(fileStatus, rxPdu.getFileStatus());
}

TEST_F(PduTest, FinWithError) {
    // Test with error condition code
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILE_CHECKSUM_FAILURE,
                   FIN_DELIVERY_CODE_INCOMPLETE, FIN_FILE_STATUS_DISCARDED);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    // Should encode successfully even with error condition
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(CONDITION_CODE_FILE_CHECKSUM_FAILURE, rxPdu.getConditionCode());
    EXPECT_EQ(FIN_DELIVERY_CODE_INCOMPLETE, rxPdu.getDeliveryCode());
    EXPECT_EQ(FIN_FILE_STATUS_DISCARDED, rxPdu.getFileStatus());
}

TEST_F(PduTest, FinDeliveryIncomplete) {
    // Test with incomplete delivery
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_INCOMPLETE, FIN_FILE_STATUS_RETAINED);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(FIN_DELIVERY_CODE_INCOMPLETE, rxPdu.getDeliveryCode());
    EXPECT_EQ(FIN_FILE_STATUS_RETAINED, rxPdu.getFileStatus());
}

TEST_F(PduTest, FinFileStatusDiscarded) {
    // Test with file discarded
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_DISCARDED);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(FIN_DELIVERY_CODE_COMPLETE, rxPdu.getDeliveryCode());
    EXPECT_EQ(FIN_FILE_STATUS_DISCARDED, rxPdu.getFileStatus());
}

TEST_F(PduTest, FinFileStatusDiscardedFilestore) {
    // Test with file discarded by filestore
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILESTORE_REJECTION,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_DISCARDED_FILESTORE);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(CONDITION_CODE_FILESTORE_REJECTION, rxPdu.getConditionCode());
    EXPECT_EQ(FIN_DELIVERY_CODE_COMPLETE, rxPdu.getDeliveryCode());
    EXPECT_EQ(FIN_FILE_STATUS_DISCARDED_FILESTORE, rxPdu.getFileStatus());
}

TEST_F(PduTest, FinBitPackingValidation) {
    // Test all combinations to verify bit packing is correct
    const FinDeliveryCode deliveryCodes[] = {FIN_DELIVERY_CODE_COMPLETE, FIN_DELIVERY_CODE_INCOMPLETE};
    const FinFileStatus fileStatuses[] = {FIN_FILE_STATUS_DISCARDED, FIN_FILE_STATUS_DISCARDED_FILESTORE,
                                          FIN_FILE_STATUS_RETAINED, FIN_FILE_STATUS_UNREPORTED};

    for (const auto& deliveryCode : deliveryCodes) {
        for (const auto& fileStatus : fileStatuses) {
            Pdu::FinPdu txPdu;
            txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                           1, 2, 3, CONDITION_CODE_NO_ERROR, deliveryCode, fileStatus);

            U8 buffer[512];
            Fw::Buffer txBuffer(buffer, sizeof(buffer));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

            Pdu::FinPdu rxPdu;
            const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

            EXPECT_EQ(deliveryCode, rxPdu.getDeliveryCode())
                << "Delivery code mismatch for combination: delivery="
                << static_cast<int>(deliveryCode) << " fileStatus=" << static_cast<int>(fileStatus);
            EXPECT_EQ(fileStatus, rxPdu.getFileStatus())
                << "File status mismatch for combination: delivery="
                << static_cast<int>(deliveryCode) << " fileStatus=" << static_cast<int>(fileStatus);
        }
    }
}

// ======================================================================
// ACK PDU Tests
// ======================================================================

TEST_F(PduTest, AckBufferSize) {
    Pdu::AckPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, FILE_DIRECTIVE_END_OF_FILE, 0,
                   CONDITION_CODE_NO_ERROR, ACK_TXN_STATUS_ACTIVE);

    U32 size = pdu.getBufferSize();
    // Should include header + directive(1) + directive_and_subtype(1) + cc_and_status(1) = header + 3
    ASSERT_GT(size, 0U);
    U32 expectedSize = pdu.asHeader().getBufferSize() + 3;
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, AckRoundTrip) {
    // Arrange - Create transmit PDU
    Pdu::AckPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const FileDirective directiveCode = FILE_DIRECTIVE_END_OF_FILE;
    const U8 directiveSubtypeCode = 0;
    const ConditionCode conditionCode = CONDITION_CODE_NO_ERROR;
    const AckTxnStatus transactionStatus = ACK_TXN_STATUS_ACTIVE;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    directiveCode, directiveSubtypeCode, conditionCode, transactionStatus);

    // Serialize to buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Deserialize from buffer
    Pdu::AckPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer1, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header fields
    const Pdu::Header& header = rxPdu.asHeader();
    EXPECT_EQ(Pdu::T_ACK, header.getType());
    EXPECT_EQ(direction, header.getDirection());
    EXPECT_EQ(txmMode, header.getTxmMode());
    EXPECT_EQ(sourceEid, header.getSourceEid());
    EXPECT_EQ(transactionSeq, header.getTransactionSeq());
    EXPECT_EQ(destEid, header.getDestEid());

    // Verify ACK-specific fields
    EXPECT_EQ(directiveCode, rxPdu.getDirectiveCode());
    EXPECT_EQ(directiveSubtypeCode, rxPdu.getDirectiveSubtypeCode());
    EXPECT_EQ(conditionCode, rxPdu.getConditionCode());
    EXPECT_EQ(transactionStatus, rxPdu.getTransactionStatus());
}

TEST_F(PduTest, AckForEof) {
    // Test ACK for EOF directive
    Pdu::AckPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, FILE_DIRECTIVE_END_OF_FILE, 0,
                   CONDITION_CODE_NO_ERROR, ACK_TXN_STATUS_ACTIVE);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::AckPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(FILE_DIRECTIVE_END_OF_FILE, rxPdu.getDirectiveCode());
    EXPECT_EQ(CONDITION_CODE_NO_ERROR, rxPdu.getConditionCode());
    EXPECT_EQ(ACK_TXN_STATUS_ACTIVE, rxPdu.getTransactionStatus());
}

TEST_F(PduTest, AckForFin) {
    // Test ACK for FIN directive
    Pdu::AckPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, FILE_DIRECTIVE_FIN, 0,
                   CONDITION_CODE_NO_ERROR, ACK_TXN_STATUS_TERMINATED);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::AckPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(FILE_DIRECTIVE_FIN, rxPdu.getDirectiveCode());
    EXPECT_EQ(ACK_TXN_STATUS_TERMINATED, rxPdu.getTransactionStatus());
}

TEST_F(PduTest, AckWithError) {
    // Test ACK with error condition code
    Pdu::AckPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, FILE_DIRECTIVE_END_OF_FILE, 0,
                   CONDITION_CODE_FILE_CHECKSUM_FAILURE, ACK_TXN_STATUS_TERMINATED);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::AckPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(CONDITION_CODE_FILE_CHECKSUM_FAILURE, rxPdu.getConditionCode());
    EXPECT_EQ(ACK_TXN_STATUS_TERMINATED, rxPdu.getTransactionStatus());
}

TEST_F(PduTest, AckWithSubtype) {
    // Test ACK with non-zero subtype code
    Pdu::AckPdu txPdu;
    const U8 subtypeCode = 5;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, FILE_DIRECTIVE_FIN, subtypeCode,
                   CONDITION_CODE_NO_ERROR, ACK_TXN_STATUS_ACTIVE);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::AckPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(subtypeCode, rxPdu.getDirectiveSubtypeCode());
}

TEST_F(PduTest, AckBitPackingValidation) {
    // Test various combinations to verify bit packing is correct
    const FileDirective directives[] = {FILE_DIRECTIVE_END_OF_FILE, FILE_DIRECTIVE_FIN};
    const AckTxnStatus statuses[] = {ACK_TXN_STATUS_UNDEFINED, ACK_TXN_STATUS_ACTIVE,
                                     ACK_TXN_STATUS_TERMINATED, ACK_TXN_STATUS_UNRECOGNIZED};
    const ConditionCode conditions[] = {CONDITION_CODE_NO_ERROR, CONDITION_CODE_FILE_CHECKSUM_FAILURE};

    for (const auto& directive : directives) {
        for (const auto& status : statuses) {
            for (const auto& condition : conditions) {
                Pdu::AckPdu txPdu;
                txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                               1, 2, 3, directive, 0, condition, status);

                U8 buffer[512];
                Fw::Buffer txBuffer(buffer, sizeof(buffer));
                ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

                Pdu::AckPdu rxPdu;
                const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
                ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

                EXPECT_EQ(directive, rxPdu.getDirectiveCode())
                    << "Directive mismatch for combination: dir="
                    << static_cast<int>(directive) << " status=" << static_cast<int>(status)
                    << " condition=" << static_cast<int>(condition);
                EXPECT_EQ(status, rxPdu.getTransactionStatus())
                    << "Status mismatch for combination: dir="
                    << static_cast<int>(directive) << " status=" << static_cast<int>(status)
                    << " condition=" << static_cast<int>(condition);
                EXPECT_EQ(condition, rxPdu.getConditionCode())
                    << "Condition mismatch for combination: dir="
                    << static_cast<int>(directive) << " status=" << static_cast<int>(status)
                    << " condition=" << static_cast<int>(condition);
            }
        }
    }
}

// ======================================================================
// NAK PDU Tests
// ======================================================================

TEST_F(PduTest, NakBufferSize) {
    Pdu::NakPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, 100, 500);

    U32 size = pdu.getBufferSize();
    // Should include header + directive(1) + scope_start(4) + scope_end(4) = header + 9
    ASSERT_GT(size, 0U);
    U32 expectedSize = pdu.asHeader().getBufferSize() + 9;
    ASSERT_EQ(expectedSize, size);
}

TEST_F(PduTest, NakRoundTrip) {
    // Arrange - Create transmit PDU
    Pdu::NakPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const CfdpFileSize scopeStart = 1024;
    const CfdpFileSize scopeEnd = 8192;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    scopeStart, scopeEnd);

    // Serialize to buffer
    U8 buffer1[512];
    Fw::Buffer txBuffer(buffer1, sizeof(buffer1));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Deserialize from buffer
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer1, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header fields
    const Pdu::Header& header = rxPdu.asHeader();
    EXPECT_EQ(Pdu::T_NAK, header.getType());
    EXPECT_EQ(direction, header.getDirection());
    EXPECT_EQ(txmMode, header.getTxmMode());
    EXPECT_EQ(sourceEid, header.getSourceEid());
    EXPECT_EQ(transactionSeq, header.getTransactionSeq());
    EXPECT_EQ(destEid, header.getDestEid());

    // Verify NAK-specific fields
    EXPECT_EQ(scopeStart, rxPdu.getScopeStart());
    EXPECT_EQ(scopeEnd, rxPdu.getScopeEnd());
}

TEST_F(PduTest, NakZeroScope) {
    // Test NAK with zero scope (start of file)
    Pdu::NakPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, 0, 1024);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(0U, rxPdu.getScopeStart());
    EXPECT_EQ(1024U, rxPdu.getScopeEnd());
}

TEST_F(PduTest, NakLargeScope) {
    // Test NAK with large file offsets
    Pdu::NakPdu txPdu;
    const CfdpFileSize largeStart = 0xFFFF0000;
    const CfdpFileSize largeEnd = 0xFFFFFFFF;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, largeStart, largeEnd);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));
    ASSERT_GT(txBuffer.getSize(), 0U);

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(largeStart, rxPdu.getScopeStart());
    EXPECT_EQ(largeEnd, rxPdu.getScopeEnd());
}

TEST_F(PduTest, NakSingleByte) {
    // Test NAK for single byte gap
    Pdu::NakPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, 1000, 1001);

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(1000U, rxPdu.getScopeStart());
    EXPECT_EQ(1001U, rxPdu.getScopeEnd());
}

TEST_F(PduTest, NakMultipleCombinations) {
    // Test various scope combinations
    const CfdpFileSize testScopes[][2] = {
        {0, 100},
        {512, 1024},
        {4096, 8192},
        {0x10000, 0x20000},
        {0x80000000, 0x90000000}
    };

    for (const auto& scope : testScopes) {
        Pdu::NakPdu txPdu;
        txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                       10, 20, 30, scope[0], scope[1]);

        U8 buffer[512];
        Fw::Buffer txBuffer(buffer, sizeof(buffer));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

        Pdu::NakPdu rxPdu;
        const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

        EXPECT_EQ(scope[0], rxPdu.getScopeStart())
            << "Scope start mismatch for range: " << scope[0] << "-" << scope[1];
        EXPECT_EQ(scope[1], rxPdu.getScopeEnd())
            << "Scope end mismatch for range: " << scope[0] << "-" << scope[1];
    }
}

TEST_F(PduTest, NakWithSingleSegment) {
    // Test NAK PDU with one segment request
    Pdu::NakPdu txPdu;
    const CfdpFileSize scopeStart = 0;
    const CfdpFileSize scopeEnd = 4096;
    const CfdpFileSize segStart = 1024;
    const CfdpFileSize segEnd = 2048;

    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, scopeStart, scopeEnd);

    ASSERT_TRUE(txPdu.addSegment(segStart, segEnd));
    EXPECT_EQ(1, txPdu.getNumSegments());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(scopeStart, rxPdu.getScopeStart());
    EXPECT_EQ(scopeEnd, rxPdu.getScopeEnd());
    EXPECT_EQ(1, rxPdu.getNumSegments());
    EXPECT_EQ(segStart, rxPdu.getSegment(0).offsetStart);
    EXPECT_EQ(segEnd, rxPdu.getSegment(0).offsetEnd);
}

TEST_F(PduTest, NakWithMultipleSegments) {
    // Test NAK PDU with multiple segment requests
    Pdu::NakPdu txPdu;
    const CfdpFileSize scopeStart = 0;
    const CfdpFileSize scopeEnd = 10000;

    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, scopeStart, scopeEnd);

    // Add 5 segments representing gaps in received data
    ASSERT_TRUE(txPdu.addSegment(100, 200));
    ASSERT_TRUE(txPdu.addSegment(500, 750));
    ASSERT_TRUE(txPdu.addSegment(1000, 1500));
    ASSERT_TRUE(txPdu.addSegment(3000, 4000));
    ASSERT_TRUE(txPdu.addSegment(8000, 9000));
    EXPECT_EQ(5, txPdu.getNumSegments());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(scopeStart, rxPdu.getScopeStart());
    EXPECT_EQ(scopeEnd, rxPdu.getScopeEnd());
    EXPECT_EQ(5, rxPdu.getNumSegments());

    // Verify each segment
    EXPECT_EQ(100, rxPdu.getSegment(0).offsetStart);
    EXPECT_EQ(200, rxPdu.getSegment(0).offsetEnd);
    EXPECT_EQ(500, rxPdu.getSegment(1).offsetStart);
    EXPECT_EQ(750, rxPdu.getSegment(1).offsetEnd);
    EXPECT_EQ(1000, rxPdu.getSegment(2).offsetStart);
    EXPECT_EQ(1500, rxPdu.getSegment(2).offsetEnd);
    EXPECT_EQ(3000, rxPdu.getSegment(3).offsetStart);
    EXPECT_EQ(4000, rxPdu.getSegment(3).offsetEnd);
    EXPECT_EQ(8000, rxPdu.getSegment(4).offsetStart);
    EXPECT_EQ(9000, rxPdu.getSegment(4).offsetEnd);
}

TEST_F(PduTest, NakWithMaxSegments) {
    // Test NAK PDU with maximum number of segments (58)
    Pdu::NakPdu txPdu;
    const CfdpFileSize scopeStart = 0;
    const CfdpFileSize scopeEnd = 100000;

    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, scopeStart, scopeEnd);

    // Add 58 segments (CF_NAK_MAX_SEGMENTS)
    for (U8 i = 0; i < 58; i++) {
        CfdpFileSize start = i * 1000;
        CfdpFileSize end = start + 500;
        ASSERT_TRUE(txPdu.addSegment(start, end)) << "Failed to add segment " << static_cast<int>(i);
    }
    EXPECT_EQ(58, txPdu.getNumSegments());

    // Try to add one more - should fail
    EXPECT_FALSE(txPdu.addSegment(60000, 61000));
    EXPECT_EQ(58, txPdu.getNumSegments());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::NakPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(scopeStart, rxPdu.getScopeStart());
    EXPECT_EQ(scopeEnd, rxPdu.getScopeEnd());
    EXPECT_EQ(58, rxPdu.getNumSegments());

    // Spot check a few segments
    EXPECT_EQ(0, rxPdu.getSegment(0).offsetStart);
    EXPECT_EQ(500, rxPdu.getSegment(0).offsetEnd);
    EXPECT_EQ(10000, rxPdu.getSegment(10).offsetStart);
    EXPECT_EQ(10500, rxPdu.getSegment(10).offsetEnd);
    EXPECT_EQ(57000, rxPdu.getSegment(57).offsetStart);
    EXPECT_EQ(57500, rxPdu.getSegment(57).offsetEnd);
}

TEST_F(PduTest, NakClearSegments) {
    // Test clearSegments() functionality
    Pdu::NakPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                 1, 2, 3, 0, 4096);

    // Add segments
    ASSERT_TRUE(pdu.addSegment(100, 200));
    ASSERT_TRUE(pdu.addSegment(300, 400));
    EXPECT_EQ(2, pdu.getNumSegments());

    // Clear and verify
    pdu.clearSegments();
    EXPECT_EQ(0, pdu.getNumSegments());

    // Should be able to add new segments
    ASSERT_TRUE(pdu.addSegment(500, 600));
    EXPECT_EQ(1, pdu.getNumSegments());
}

TEST_F(PduTest, NakBufferSizeWithSegments) {
    // Test that bufferSize() correctly accounts for segments
    Pdu::NakPdu pdu;
    pdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                 1, 2, 3, 0, 4096);

    U32 baseSizeNoSegments = pdu.getBufferSize();

    // Add one segment
    ASSERT_TRUE(pdu.addSegment(100, 200));
    U32 sizeWithOneSegment = pdu.getBufferSize();
    EXPECT_EQ(baseSizeNoSegments + 8, sizeWithOneSegment);  // 2 * sizeof(CfdpFileSize) = 8

    // Add another segment
    ASSERT_TRUE(pdu.addSegment(300, 400));
    U32 sizeWithTwoSegments = pdu.getBufferSize();
    EXPECT_EQ(baseSizeNoSegments + 16, sizeWithTwoSegments);  // 4 * sizeof(CfdpFileSize) = 16
}

// ======================================================================
// TLV Tests
// ======================================================================

TEST_F(PduTest, TlvCreateWithEntityId) {
    // Test creating TLV with entity ID
    Tlv tlv;
    const CfdpEntityId testEid = 42;

    tlv.initialize(testEid);

    EXPECT_EQ(TLV_TYPE_ENTITY_ID, tlv.getType());
    EXPECT_EQ(sizeof(CfdpEntityId), tlv.getData().getLength());
    EXPECT_EQ(testEid, tlv.getData().getEntityId());
}

TEST_F(PduTest, TlvCreateWithRawData) {
    // Test creating TLV with raw data
    Tlv tlv;
    const U8 testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    const U8 testDataLen = sizeof(testData);

    tlv.initialize(TLV_TYPE_MESSAGE_TO_USER, testData, testDataLen);

    EXPECT_EQ(TLV_TYPE_MESSAGE_TO_USER, tlv.getType());
    EXPECT_EQ(testDataLen, tlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, tlv.getData().getData(), testDataLen));
}

TEST_F(PduTest, TlvEncodedSize) {
    // Test TLV encoded size calculation
    Tlv tlv;
    const U8 testData[] = {0xAA, 0xBB, 0xCC};

    tlv.initialize(TLV_TYPE_FLOW_LABEL, testData, sizeof(testData));

    // Type(1) + Length(1) + Data(3) = 5
    EXPECT_EQ(5U, tlv.getEncodedSize());
}

TEST_F(PduTest, TlvEncodeDecodeEntityId) {
    // Test encoding and decoding entity ID TLV
    Tlv txTlv;
    const CfdpEntityId testEid = 123;
    txTlv.initialize(testEid);

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    // Decode
    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    // Verify
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxTlv.getType());
    EXPECT_EQ(testEid, rxTlv.getData().getEntityId());
}

TEST_F(PduTest, TlvEncodeDecodeRawData) {
    // Test encoding and decoding raw data TLV
    Tlv txTlv;
    const U8 testData[] = {0xDE, 0xAD, 0xBE, 0xEF};
    txTlv.initialize(TLV_TYPE_MESSAGE_TO_USER, testData, sizeof(testData));

    U8 buffer[256];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    // Decode
    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    // Verify
    EXPECT_EQ(TLV_TYPE_MESSAGE_TO_USER, rxTlv.getType());
    EXPECT_EQ(sizeof(testData), rxTlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, rxTlv.getData().getData(), sizeof(testData)));
}

TEST_F(PduTest, TlvEncodeDecodeMaxData) {
    // Test TLV with maximum data length (255 bytes)
    Tlv txTlv;
    U8 testData[255];
    for (U16 i = 0; i < 255; i++) {
        testData[i] = static_cast<U8>(i);
    }
    txTlv.initialize(TLV_TYPE_MESSAGE_TO_USER, testData, 255);

    U8 buffer[512];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txTlv.toSerialBuffer(serialBuffer));

    // Decode
    serialBuffer.resetSer();
    serialBuffer.fill();
    Tlv rxTlv;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxTlv.fromSerialBuffer(serialBuffer));

    // Verify
    EXPECT_EQ(255, rxTlv.getData().getLength());
    EXPECT_EQ(0, memcmp(testData, rxTlv.getData().getData(), 255));
}

// ======================================================================
// TlvList Tests
// ======================================================================

TEST_F(PduTest, TlvListAppendUpToMax) {
    // Test appending TLVs up to maximum (4)
    TlvList list;

    for (U8 i = 0; i < CFDP_MAX_TLV; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<CfdpEntityId>(100 + i));
        ASSERT_TRUE(list.appendTlv(tlv)) << "Failed to append TLV " << static_cast<int>(i);
    }

    EXPECT_EQ(CFDP_MAX_TLV, list.getNumTlv());
}

TEST_F(PduTest, TlvListRejectWhenFull) {
    // Test that appending fails when list is full
    TlvList list;

    // Fill the list
    for (U8 i = 0; i < CFDP_MAX_TLV; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<CfdpEntityId>(i));
        ASSERT_TRUE(list.appendTlv(tlv));
    }

    // Try to add one more - should fail
    Tlv extraTlv;
    extraTlv.initialize(999);
    EXPECT_FALSE(list.appendTlv(extraTlv));
    EXPECT_EQ(CFDP_MAX_TLV, list.getNumTlv());
}

TEST_F(PduTest, TlvListClear) {
    // Test clearing TLV list
    TlvList list;

    // Add some TLVs
    for (U8 i = 0; i < 3; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<CfdpEntityId>(i));
        ASSERT_TRUE(list.appendTlv(tlv));
    }
    EXPECT_EQ(3, list.getNumTlv());

    // Clear and verify
    list.clear();
    EXPECT_EQ(0, list.getNumTlv());

    // Should be able to add new TLVs
    Tlv tlv;
    tlv.initialize(100);
    ASSERT_TRUE(list.appendTlv(tlv));
    EXPECT_EQ(1, list.getNumTlv());
}

TEST_F(PduTest, TlvListEncodedSize) {
    // Test TLV list encoded size calculation
    TlvList list;

    // Add TLVs of different sizes
    Tlv tlv1;
    tlv1.initialize(42);  // Entity ID TLV
    ASSERT_TRUE(list.appendTlv(tlv1));

    const U8 data[] = {0x01, 0x02, 0x03};
    Tlv tlv2;
    tlv2.initialize(TLV_TYPE_MESSAGE_TO_USER, data, sizeof(data));
    ASSERT_TRUE(list.appendTlv(tlv2));

    U32 expectedSize = tlv1.getEncodedSize() + tlv2.getEncodedSize();
    EXPECT_EQ(expectedSize, list.getEncodedSize());
}

TEST_F(PduTest, TlvListEncodeDecode) {
    // Test encoding and decoding TLV list
    TlvList txList;

    // Add multiple TLVs
    Tlv tlv1;
    tlv1.initialize(123);
    ASSERT_TRUE(txList.appendTlv(tlv1));

    const U8 data2[] = {0xAA, 0xBB};
    Tlv tlv2;
    tlv2.initialize(TLV_TYPE_MESSAGE_TO_USER, data2, sizeof(data2));
    ASSERT_TRUE(txList.appendTlv(tlv2));

    const U8 data3[] = {0xDE, 0xAD, 0xBE, 0xEF};
    Tlv tlv3;
    tlv3.initialize(TLV_TYPE_FLOW_LABEL, data3, sizeof(data3));
    ASSERT_TRUE(txList.appendTlv(tlv3));

    U8 buffer[512];
    Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));

    // Encode
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txList.toSerialBuffer(serialBuffer));
    U32 encodedSize = static_cast<U32>(serialBuffer.getSize());

    // Decode
    Fw::SerialBuffer decodeBuffer(buffer, encodedSize);
    decodeBuffer.fill();
    TlvList rxList;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxList.fromSerialBuffer(decodeBuffer));

    // Verify
    EXPECT_EQ(3, rxList.getNumTlv());
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxList.getTlv(0).getType());
    EXPECT_EQ(123, rxList.getTlv(0).getData().getEntityId());
    EXPECT_EQ(TLV_TYPE_MESSAGE_TO_USER, rxList.getTlv(1).getType());
    EXPECT_EQ(TLV_TYPE_FLOW_LABEL, rxList.getTlv(2).getType());
}

// ======================================================================
// EOF PDU with TLV Tests
// ======================================================================

TEST_F(PduTest, EofWithNoTlvs) {
    // Verify existing EOF tests work with TLV support (backward compatible)
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0x12345678, 4096);

    EXPECT_EQ(0, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(0, rxPdu.getNumTlv());
}

TEST_F(PduTest, EofWithOneTlv) {
    // Test EOF PDU with one TLV
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILE_CHECKSUM_FAILURE, 0, 0);

    // Add entity ID TLV
    Tlv tlv;
    tlv.initialize(42);
    ASSERT_TRUE(txPdu.appendTlv(tlv));
    EXPECT_EQ(1, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(CONDITION_CODE_FILE_CHECKSUM_FAILURE, rxPdu.getConditionCode());
    EXPECT_EQ(1, rxPdu.getNumTlv());
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxPdu.getTlvList().getTlv(0).getType());
    EXPECT_EQ(42, rxPdu.getTlvList().getTlv(0).getData().getEntityId());
}

TEST_F(PduTest, EofWithMultipleTlvs) {
    // Test EOF PDU with multiple TLVs
    Pdu::EofPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILESTORE_REJECTION, 0xABCDEF, 2048);

    // Add entity ID TLV
    Tlv tlv1;
    tlv1.initialize(123);
    ASSERT_TRUE(txPdu.appendTlv(tlv1));

    // Add message to user TLV
    const U8 message[] = "Error: File rejected";
    Tlv tlv2;
    tlv2.initialize(TLV_TYPE_MESSAGE_TO_USER, message, sizeof(message) - 1);
    ASSERT_TRUE(txPdu.appendTlv(tlv2));

    EXPECT_EQ(2, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(2, rxPdu.getNumTlv());
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxPdu.getTlvList().getTlv(0).getType());
    EXPECT_EQ(TLV_TYPE_MESSAGE_TO_USER, rxPdu.getTlvList().getTlv(1).getType());
}

TEST_F(PduTest, EofTlvBufferSize) {
    // Verify buffer size calculation includes TLVs
    Pdu::EofPdu pdu1, pdu2;
    pdu1.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0, 0);
    pdu2.initialize(DIRECTION_TOWARD_RECEIVER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR, 0, 0);

    U32 sizeWithoutTlv = pdu1.getBufferSize();

    // Add TLV to second PDU
    Tlv tlv;
    tlv.initialize(42);
    ASSERT_TRUE(pdu2.appendTlv(tlv));

    U32 sizeWithTlv = pdu2.getBufferSize();
    EXPECT_EQ(sizeWithoutTlv + tlv.getEncodedSize(), sizeWithTlv);
}

TEST_F(PduTest, EofTlvRoundTripComplete) {
    // Comprehensive round-trip test with TLVs
    Pdu::EofPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_RECEIVER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
    const CfdpEntityId sourceEid = 10;
    const CfdpTransactionSeq transactionSeq = 20;
    const CfdpEntityId destEid = 30;
    const ConditionCode conditionCode = CONDITION_CODE_FILE_SIZE_ERROR;
    const U32 checksum = 0xDEADBEEF;
    const CfdpFileSize fileSize = 8192;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    conditionCode, checksum, fileSize);

    // Add TLVs
    Tlv tlv1;
    tlv1.initialize(sourceEid);
    ASSERT_TRUE(txPdu.appendTlv(tlv1));

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Decode
    Pdu::EofPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header
    EXPECT_EQ(direction, rxPdu.asHeader().getDirection());
    EXPECT_EQ(txmMode, rxPdu.asHeader().getTxmMode());
    EXPECT_EQ(sourceEid, rxPdu.asHeader().getSourceEid());
    EXPECT_EQ(transactionSeq, rxPdu.asHeader().getTransactionSeq());
    EXPECT_EQ(destEid, rxPdu.asHeader().getDestEid());

    // Verify EOF fields
    EXPECT_EQ(conditionCode, rxPdu.getConditionCode());
    EXPECT_EQ(checksum, rxPdu.getChecksum());
    EXPECT_EQ(fileSize, rxPdu.getFileSize());

    // Verify TLVs
    EXPECT_EQ(1, rxPdu.getNumTlv());
    EXPECT_EQ(sourceEid, rxPdu.getTlvList().getTlv(0).getData().getEntityId());
}

// ======================================================================
// FIN PDU with TLV Tests
// ======================================================================

TEST_F(PduTest, FinWithNoTlvs) {
    // Verify existing FIN tests work with TLV support (backward compatible)
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_RETAINED);

    EXPECT_EQ(0, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));
    EXPECT_EQ(0, rxPdu.getNumTlv());
}

TEST_F(PduTest, FinWithOneTlv) {
    // Test FIN PDU with one TLV
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILE_CHECKSUM_FAILURE,
                   FIN_DELIVERY_CODE_INCOMPLETE, FIN_FILE_STATUS_DISCARDED);

    // Add entity ID TLV
    Tlv tlv;
    tlv.initialize(99);
    ASSERT_TRUE(txPdu.appendTlv(tlv));
    EXPECT_EQ(1, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(CONDITION_CODE_FILE_CHECKSUM_FAILURE, rxPdu.getConditionCode());
    EXPECT_EQ(FIN_DELIVERY_CODE_INCOMPLETE, rxPdu.getDeliveryCode());
    EXPECT_EQ(FIN_FILE_STATUS_DISCARDED, rxPdu.getFileStatus());
    EXPECT_EQ(1, rxPdu.getNumTlv());
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxPdu.getTlvList().getTlv(0).getType());
    EXPECT_EQ(99, rxPdu.getTlvList().getTlv(0).getData().getEntityId());
}

TEST_F(PduTest, FinWithMultipleTlvs) {
    // Test FIN PDU with multiple TLVs
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_FILESTORE_REJECTION,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_DISCARDED_FILESTORE);

    // Add entity ID TLV
    Tlv tlv1;
    tlv1.initialize(456);
    ASSERT_TRUE(txPdu.appendTlv(tlv1));

    // Add message to user TLV
    const U8 message[] = "Transaction failed";
    Tlv tlv2;
    tlv2.initialize(TLV_TYPE_MESSAGE_TO_USER, message, sizeof(message) - 1);
    ASSERT_TRUE(txPdu.appendTlv(tlv2));

    // Add flow label TLV
    const U8 flowLabel[] = {0x01, 0x02};
    Tlv tlv3;
    tlv3.initialize(TLV_TYPE_FLOW_LABEL, flowLabel, sizeof(flowLabel));
    ASSERT_TRUE(txPdu.appendTlv(tlv3));

    EXPECT_EQ(3, txPdu.getNumTlv());

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Verify round-trip
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(3, rxPdu.getNumTlv());
    EXPECT_EQ(TLV_TYPE_ENTITY_ID, rxPdu.getTlvList().getTlv(0).getType());
    EXPECT_EQ(TLV_TYPE_MESSAGE_TO_USER, rxPdu.getTlvList().getTlv(1).getType());
    EXPECT_EQ(TLV_TYPE_FLOW_LABEL, rxPdu.getTlvList().getTlv(2).getType());
}

TEST_F(PduTest, FinTlvBufferSize) {
    // Verify buffer size calculation includes TLVs
    Pdu::FinPdu pdu1, pdu2;
    pdu1.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_RETAINED);
    pdu2.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_RETAINED);

    U32 sizeWithoutTlv = pdu1.getBufferSize();

    // Add TLV to second PDU
    Tlv tlv;
    tlv.initialize(789);
    ASSERT_TRUE(pdu2.appendTlv(tlv));

    U32 sizeWithTlv = pdu2.getBufferSize();
    EXPECT_EQ(sizeWithoutTlv + tlv.getEncodedSize(), sizeWithTlv);
}

TEST_F(PduTest, FinTlvRoundTripComplete) {
    // Comprehensive round-trip test with TLVs
    Pdu::FinPdu txPdu;
    const Direction direction = DIRECTION_TOWARD_SENDER;
    const Cfdp::Class::T txmMode = Cfdp::Class::CLASS_2;
    const CfdpEntityId sourceEid = 50;
    const CfdpTransactionSeq transactionSeq = 100;
    const CfdpEntityId destEid = 75;
    const ConditionCode conditionCode = CONDITION_CODE_INACTIVITY_DETECTED;
    const FinDeliveryCode deliveryCode = FIN_DELIVERY_CODE_INCOMPLETE;
    const FinFileStatus fileStatus = FIN_FILE_STATUS_RETAINED;

    txPdu.initialize(direction, txmMode, sourceEid, transactionSeq, destEid,
                    conditionCode, deliveryCode, fileStatus);

    // Add TLVs
    Tlv tlv1;
    tlv1.initialize(destEid);
    ASSERT_TRUE(txPdu.appendTlv(tlv1));

    const U8 msg[] = "Timeout";
    Tlv tlv2;
    tlv2.initialize(TLV_TYPE_MESSAGE_TO_USER, msg, sizeof(msg) - 1);
    ASSERT_TRUE(txPdu.appendTlv(tlv2));

    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    // Decode
    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    // Verify header
    EXPECT_EQ(direction, rxPdu.asHeader().getDirection());
    EXPECT_EQ(txmMode, rxPdu.asHeader().getTxmMode());
    EXPECT_EQ(sourceEid, rxPdu.asHeader().getSourceEid());
    EXPECT_EQ(transactionSeq, rxPdu.asHeader().getTransactionSeq());
    EXPECT_EQ(destEid, rxPdu.asHeader().getDestEid());

    // Verify FIN fields
    EXPECT_EQ(conditionCode, rxPdu.getConditionCode());
    EXPECT_EQ(deliveryCode, rxPdu.getDeliveryCode());
    EXPECT_EQ(fileStatus, rxPdu.getFileStatus());

    // Verify TLVs
    EXPECT_EQ(2, rxPdu.getNumTlv());
    EXPECT_EQ(destEid, rxPdu.getTlvList().getTlv(0).getData().getEntityId());
    EXPECT_EQ(0, memcmp(msg, rxPdu.getTlvList().getTlv(1).getData().getData(), sizeof(msg) - 1));
}

TEST_F(PduTest, FinWithMaxTlvs) {
    // Test FIN PDU with maximum number of TLVs (4)
    Pdu::FinPdu txPdu;
    txPdu.initialize(DIRECTION_TOWARD_SENDER, Cfdp::Class::CLASS_2,
                   1, 2, 3, CONDITION_CODE_NO_ERROR,
                   FIN_DELIVERY_CODE_COMPLETE, FIN_FILE_STATUS_RETAINED);

    // Add 4 TLVs
    for (U8 i = 0; i < CFDP_MAX_TLV; i++) {
        Tlv tlv;
        tlv.initialize(static_cast<CfdpEntityId>(100 + i));
        ASSERT_TRUE(txPdu.appendTlv(tlv)) << "Failed to append TLV " << static_cast<int>(i);
    }
    EXPECT_EQ(CFDP_MAX_TLV, txPdu.getNumTlv());

    // Try to add one more - should fail
    Tlv extraTlv;
    extraTlv.initialize(999);
    EXPECT_FALSE(txPdu.appendTlv(extraTlv));

    // Verify round-trip with 4 TLVs
    U8 buffer[512];
    Fw::Buffer txBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, txPdu.toBuffer(txBuffer));

    Pdu::FinPdu rxPdu;
    const Fw::Buffer rxBuffer(buffer, txBuffer.getSize());
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, rxPdu.fromBuffer(rxBuffer));

    EXPECT_EQ(CFDP_MAX_TLV, rxPdu.getNumTlv());
    for (U8 i = 0; i < CFDP_MAX_TLV; i++) {
        EXPECT_EQ(100 + i, rxPdu.getTlvList().getTlv(i).getData().getEntityId());
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}