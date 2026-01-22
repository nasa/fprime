// ======================================================================
// \title  PduTester.cpp
// \author campuzan
// \brief  cpp file for PDU test implementations
//
// This file contains PDU test function implementations for CfdpManagerTester.
// The declarations remain in CfdpManagerTester.hpp.
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/File.hpp>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdio>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// PDU Test Helper Implementations
// ----------------------------------------------------------------------

CF_Transaction_t* CfdpManagerTester::setupTestTransaction(
    CF_TxnState_t state,
    U8 channelId,
    const char* srcFilename,
    const char* dstFilename,
    U32 fileSize,
    U32 sequenceId,
    U32 peerId
) {
    // For white box testing, directly use the first transaction for the specified channel
    U32 txnIndex = channelId * CF_NUM_TRANSACTIONS_PER_CHANNEL;
    CF_Transaction_t* txn = &cfdpEngine.transactions[txnIndex];

    // Use the first history for the specified channel
    U32 histIndex = channelId * CF_NUM_HISTORIES_PER_CHANNEL;
    CF_History_t* history = &cfdpEngine.histories[histIndex];

    // Initialize transaction state
    txn->state = state;
    txn->fsize = fileSize;
    txn->chan_num = channelId;
    txn->cfdpManager = &this->component;
    txn->history = history;

    // Initialize history
    history->peer_eid = peerId;
    history->seq_num = sequenceId;
    history->fnames.src_filename = Fw::String(srcFilename);
    history->fnames.dst_filename = Fw::String(dstFilename);
    history->dir = CF_Direction_TX;

    return txn;
}

const Fw::Buffer& CfdpManagerTester::getSentPduBuffer(FwIndexType index) {
    // Retrieve PDU buffer from dataOut port history
    EXPECT_GT(this->fromPortHistory_dataOut->size(), index);
    static Fw::Buffer emptyBuffer;
    if (this->fromPortHistory_dataOut->size() <= static_cast<FwSizeType>(index)) {
        return emptyBuffer;
    }

    // Extract buffer from history entry
    const FromPortEntry_dataOut& entry =
        this->fromPortHistory_dataOut->at(index);
    return entry.fwBuffer;
}

bool CfdpManagerTester::deserializePduHeader(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::Header& header
) {
    // Copy buffer data for deserialization
    U8 buffer[CF_MAX_PDU_SIZE];
    FwSizeType copySize = (pduBuffer.getSize() < CF_MAX_PDU_SIZE) ? pduBuffer.getSize() : CF_MAX_PDU_SIZE;
    memcpy(buffer, pduBuffer.getData(), copySize);

    Fw::SerialBuffer serialBuffer(buffer, copySize);
    serialBuffer.fill();

    Fw::SerializeStatus status = header.fromSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializePduHeader failed with status: " << status << std::endl;
    }
    return (status == Fw::FW_SERIALIZE_OK);
}

bool CfdpManagerTester::deserializeMetadataPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::MetadataPdu& metadataPdu
) {
    // Use the MetadataPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = metadataPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializeMetadataPdu failed with status: " << status << std::endl;
        return false;
    }

    return true;
}

bool CfdpManagerTester::deserializeFileDataPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::FileDataPdu& fileDataPdu
) {
    // Use the FileDataPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = fileDataPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializeFileDataPdu failed with status: " << status << std::endl;
        return false;
    }

    return true;
}

bool CfdpManagerTester::deserializeEofPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::EofPdu& eofPdu
) {
    // Use the EofPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = eofPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializeEofPdu failed with status: " << status << std::endl;
        return false;
    }

    return true;
}

bool CfdpManagerTester::deserializeFinPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::FinPdu& finPdu
) {
    // Use the FinPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = finPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return false;
    }

    return true;
}

bool CfdpManagerTester::deserializeAckPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::AckPdu& ackPdu
) {
    // Use the AckPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = ackPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializeAckPdu failed with status: " << status << std::endl;
        return false;
    }

    return true;
}

bool CfdpManagerTester::deserializeNakPdu(
    const Fw::Buffer& pduBuffer,
    Cfdp::Pdu::NakPdu& nakPdu
) {
    // Use the NakPdu's fromBuffer() method to deserialize everything
    Fw::SerializeStatus status = nakPdu.fromBuffer(pduBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializeNakPdu failed with status: " << status << std::endl;
        return false;
    }

    return true;
}

void CfdpManagerTester::validateMetadataPdu(
    const Cfdp::Pdu::MetadataPdu& metadataPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    CfdpFileSize expectedFileSize,
    const char* expectedSourceFilename,
    const char* expectedDestFilename
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = metadataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_METADATA, header.getType()) << "Expected T_METADATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_UNACKNOWLEDGED, header.getTxmMode()) << "Expected unacknowledged mode for class 1";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate metadata-specific fields
    EXPECT_EQ(expectedFileSize, metadataPdu.getFileSize()) << "File size mismatch";
    EXPECT_EQ(Cfdp::CHECKSUM_TYPE_MODULAR, metadataPdu.getChecksumType()) << "Expected modular checksum type";
    EXPECT_EQ(0, metadataPdu.getClosureRequested()) << "Class 1 should not request closure";

    // Validate source filename
    const char* rxSrcFilename = metadataPdu.getSourceFilename();
    ASSERT_NE(nullptr, rxSrcFilename) << "Source filename is null";
    FwSizeType srcLen = strlen(expectedSourceFilename);
    EXPECT_EQ(0, memcmp(rxSrcFilename, expectedSourceFilename, srcLen)) << "Source filename mismatch";

    // Validate destination filename
    const char* rxDstFilename = metadataPdu.getDestFilename();
    ASSERT_NE(nullptr, rxDstFilename) << "Destination filename is null";
    FwSizeType dstLen = strlen(expectedDestFilename);
    EXPECT_EQ(0, memcmp(rxDstFilename, expectedDestFilename, dstLen)) << "Destination filename mismatch";
}

void CfdpManagerTester::validateFileDataPdu(
    const Cfdp::Pdu::FileDataPdu& fileDataPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    U32 expectedOffset,
    U16 expectedDataSize,
    const char* filename
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = fileDataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_FILE_DATA, header.getType()) << "Expected T_FILE_DATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_UNACKNOWLEDGED, header.getTxmMode()) << "Expected unacknowledged mode for class 1";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate file data fields
    U32 offset = fileDataPdu.getOffset();
    U16 dataSize = fileDataPdu.getDataSize();
    const U8* pduData = fileDataPdu.getData();

    EXPECT_EQ(expectedOffset, offset) << "File offset mismatch";
    EXPECT_EQ(expectedDataSize, dataSize) << "Data size mismatch";
    ASSERT_NE(nullptr, pduData) << "Data pointer is null";
    ASSERT_GT(dataSize, 0U) << "Data size is zero";

    // Read expected data from file at the offset specified in the PDU
    U8* expectedData = new U8[dataSize];
    Os::File file;

    Os::File::Status fileStatus = file.open(filename, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open file: " << filename;

    fileStatus = file.seek(static_cast<FwSignedSizeType>(offset), Os::File::ABSOLUTE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to seek in file";

    FwSizeType bytesRead = dataSize;
    fileStatus = file.read(expectedData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read from file";
    ASSERT_EQ(dataSize, bytesRead) << "Failed to read expected data from file";

    // Validate data content
    EXPECT_EQ(0, memcmp(expectedData, pduData, dataSize))
        << "Data content mismatch at offset " << offset;

    delete[] expectedData;
}

void CfdpManagerTester::validateEofPdu(
    const Cfdp::Pdu::EofPdu& eofPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::ConditionCode expectedConditionCode,
    CfdpFileSize expectedFileSize,
    const char* sourceFilename
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = eofPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_EOF, header.getType()) << "Expected T_EOF type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_ACKNOWLEDGED, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate EOF-specific fields
    EXPECT_EQ(expectedConditionCode, eofPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedFileSize, eofPdu.getFileSize()) << "File size mismatch";

    // Compute file CRC and validate against EOF PDU checksum
    U32 rxChecksum = eofPdu.getChecksum();

    // Open and read the source file to compute CRC
    Os::File file;
    Os::File::Status fileStatus = file.open(sourceFilename, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open source file: " << sourceFilename;

    // Allocate buffer for file content
    U8* fileData = new U8[expectedFileSize];
    FwSizeType bytesRead = expectedFileSize;
    fileStatus = file.read(fileData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read source file";
    ASSERT_EQ(expectedFileSize, bytesRead) << "Failed to read complete file";

    // Compute CRC using CFDP Checksum
    CFDP::Checksum computedChecksum;
    computedChecksum.update(fileData, 0, expectedFileSize);
    U32 expectedCrc = computedChecksum.getValue();

    delete[] fileData;

    // Validate checksum matches
    EXPECT_EQ(expectedCrc, rxChecksum) << "File CRC mismatch";
}

void CfdpManagerTester::validateFinPdu(
    const Cfdp::Pdu::FinPdu& finPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::ConditionCode expectedConditionCode,
    Cfdp::FinDeliveryCode expectedDeliveryCode,
    Cfdp::FinFileStatus expectedFileStatus
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = finPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_FIN, header.getType()) << "Expected T_FIN type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_SENDER, header.getDirection()) << "Expected direction toward sender";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_ACKNOWLEDGED, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate FIN-specific fields
    EXPECT_EQ(expectedConditionCode, finPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedDeliveryCode, finPdu.getDeliveryCode()) << "Delivery code mismatch";
    EXPECT_EQ(expectedFileStatus, finPdu.getFileStatus()) << "File status mismatch";
}

void CfdpManagerTester::validateAckPdu(
    const Cfdp::Pdu::AckPdu& ackPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::FileDirective expectedDirectiveCode,
    U8 expectedDirectiveSubtypeCode,
    Cfdp::ConditionCode expectedConditionCode,
    Cfdp::AckTxnStatus expectedTransactionStatus
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = ackPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_ACK, header.getType()) << "Expected T_ACK type";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_ACKNOWLEDGED, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate ACK-specific fields
    EXPECT_EQ(expectedDirectiveCode, ackPdu.getDirectiveCode()) << "Directive code mismatch";
    EXPECT_EQ(expectedDirectiveSubtypeCode, ackPdu.getDirectiveSubtypeCode()) << "Directive subtype code mismatch";
    EXPECT_EQ(expectedConditionCode, ackPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedTransactionStatus, ackPdu.getTransactionStatus()) << "Transaction status mismatch";
}

void CfdpManagerTester::validateNakPdu(
    const Cfdp::Pdu::NakPdu& nakPdu,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    CfdpFileSize expectedScopeStart,
    CfdpFileSize expectedScopeEnd,
    U8 expectedNumSegments,
    const Cfdp::Pdu::SegmentRequest* expectedSegments
) {
    // Validate header fields
    const Cfdp::Pdu::Header& header = nakPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_NAK, header.getType()) << "Expected T_NAK type";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_ACKNOWLEDGED, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate NAK-specific fields
    EXPECT_EQ(expectedScopeStart, nakPdu.getScopeStart()) << "Scope start mismatch";
    EXPECT_EQ(expectedScopeEnd, nakPdu.getScopeEnd()) << "Scope end mismatch";

    // Validate segment requests if expectedNumSegments > 0
    if (expectedNumSegments > 0) {
        EXPECT_EQ(expectedNumSegments, nakPdu.getNumSegments())
            << "Expected " << static_cast<int>(expectedNumSegments) << " segment requests";

        // Validate each segment if expectedSegments array is provided
        if (expectedSegments != nullptr) {
            for (U8 i = 0; i < expectedNumSegments; i++) {
                EXPECT_EQ(expectedSegments[i].offsetStart, nakPdu.getSegment(i).offsetStart)
                    << "Segment " << static_cast<int>(i) << " start offset mismatch";
                EXPECT_EQ(expectedSegments[i].offsetEnd, nakPdu.getSegment(i).offsetEnd)
                    << "Segment " << static_cast<int>(i) << " end offset mismatch";
            }
        }
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testMetaDataPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CF_CFDP_SendMd()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Step 1: Configure transaction for Metadata PDU emission
    const char* srcFile = "/tmp/test_source.bin";
    const char* dstFile = "/tmp/test_dest.bin";
    const CfdpFileSize fileSize = 1024;
    const U8 channelId = 0;
    const U32 testSequenceId = 98;
    const U32 testPeerId = 100;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_S1,  // Sender, class 1
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Clear port history before test
    this->clearHistory();

    // Step 2: Invoke sender to emit Metadata PDU
    CfdpStatus::T status = CF_CFDP_SendMd(txn);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendMd failed";

    // Step 3: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Step 4: Deserialize complete Metadata PDU (header + body)
    Cfdp::Pdu::MetadataPdu metadataPdu;
    bool metadataOk = deserializeMetadataPdu(pduBuffer, metadataPdu);
    ASSERT_TRUE(metadataOk) << "Failed to deserialize Metadata PDU";

    // Step 5: Validate all PDU fields
    validateMetadataPdu(metadataPdu, component.getLocalEidParam(), testPeerId,
                       testSequenceId, fileSize, srcFile, dstFile);
}

void CfdpManagerTester::testFileDataPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Read test file and construct File Data PDU
    // 3. Invoke CF_CFDP_SendFd()
    // 4. Capture PDU from dataOut and validate

    // Test file configuration
    const char* testFilePath = "test/ut/data/test_file.bin";
    const U32 fileOffset = 50;   // Read from offset 50
    const U16 readSize = 64;     // Read 64 bytes

    // Step 1: Configure transaction for File Data PDU emission
    const char* srcFile = testFilePath;
    const char* dstFile = "/tmp/dest_file.bin";
    const U32 fileSize = 256;  // Approximate file size
    const U8 channelId = 0;
    const U32 testSequenceId = 42;
    const U32 testPeerId = 200;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_S1,  // Sender, class 1
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Clear port history before test
    this->clearHistory();

    // Step 2: Read test data from file
    U8 testData[readSize];
    Os::File file;

    Os::File::Status fileStatus = file.open(testFilePath, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open test file: " << testFilePath;

    fileStatus = file.seek(static_cast<FwSignedSizeType>(fileOffset), Os::File::ABSOLUTE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to seek in test file";

    FwSizeType bytesRead = readSize;
    fileStatus = file.read(testData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read from test file";
    ASSERT_EQ(readSize, bytesRead) << "Failed to read test data from file";

    // Step 3: Construct PDU buffer with File Data header
    CF_Logical_PduBuffer_t* ph = CF_CFDP_ConstructPduHeader(
        txn,
        CF_CFDP_FileDirective_INVALID_MIN,  // File data PDU has invalid directive
        component.getLocalEidParam(),
        testPeerId,
        0,  // towards receiver
        testSequenceId,
        1   // file data flag
    );
    ASSERT_NE(ph, nullptr) << "Failed to construct PDU header";

    // Setup file data header
    CF_Logical_PduFileDataHeader_t* fd = &ph->int_header.fd;
    fd->offset = fileOffset;

    // Encode file data header
    CF_CFDP_EncodeFileDataHeader(ph->penc, ph->pdu_header.segment_meta_flag, fd);

    // Get pointer to data area and copy test data
    size_t actual_bytes = CF_CODEC_GET_REMAIN(ph->penc);
    ASSERT_GE(actual_bytes, readSize) << "Insufficient space in PDU buffer";

    U8* data_ptr = CF_CFDP_DoEncodeChunk(ph->penc, readSize);
    ASSERT_NE(data_ptr, nullptr) << "Failed to get data pointer";

    // Copy test data into PDU
    memcpy(data_ptr, testData, readSize);
    fd->data_len = readSize;
    fd->data_ptr = data_ptr;

    // Step 4: Invoke CF_CFDP_SendFd to emit File Data PDU
    CfdpStatus::T status = CF_CFDP_SendFd(txn, ph);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendFd failed";

    // Step 5: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Deserialize complete File Data PDU (header + body)
    Cfdp::Pdu::FileDataPdu fileDataPdu;
    bool fileDataOk = deserializeFileDataPdu(pduBuffer, fileDataPdu);
    ASSERT_TRUE(fileDataOk) << "Failed to deserialize File Data PDU";

    // Step 6: Validate all PDU fields (validateFileDataPdu reads file to verify content)
    validateFileDataPdu(fileDataPdu, component.getLocalEidParam(), testPeerId,
                       testSequenceId, fileOffset, readSize, testFilePath);
}

void CfdpManagerTester::testEofPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CF_CFDP_SendEof()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Step 1: Configure transaction for EOF PDU emission
    const char* srcFile = "test/ut/data/test_file.bin";
    const char* dstFile = "/tmp/dest_eof.bin";
    const CfdpFileSize fileSize = 242;  // Actual size of test_file.bin
    const U8 channelId = 0;
    const U32 testSequenceId = 55;
    const U32 testPeerId = 150;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_S2,  // Sender, class 2 (acknowledged mode)
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Setup transaction to simulate file transfer complete
    const Cfdp::ConditionCode testConditionCode = Cfdp::CONDITION_CODE_NO_ERROR;
    txn->state_data.send.cached_pos = fileSize;  // Simulate file transfer complete

    // Read test file and compute CRC
    Os::File file;
    Os::File::Status fileStatus = file.open(srcFile, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open test file: " << srcFile;

    U8* fileData = new U8[fileSize];
    FwSizeType bytesRead = fileSize;
    fileStatus = file.read(fileData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read test file";
    ASSERT_EQ(fileSize, bytesRead) << "Failed to read complete test file";

    // Compute and set CRC in transaction (matches what CF_CFDP_SendEof expects)
    txn->crc.update(fileData, 0, fileSize);
    delete[] fileData;

    // Clear port history before test
    this->clearHistory();

    // Step 2: Invoke sender to emit EOF PDU
    CfdpStatus::T status = CF_CFDP_SendEof(txn);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendEof failed";

    // Step 3: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Step 4: Deserialize complete EOF PDU (header + body)
    Cfdp::Pdu::EofPdu eofPdu;
    bool eofOk = deserializeEofPdu(pduBuffer, eofPdu);
    ASSERT_TRUE(eofOk) << "Failed to deserialize EOF PDU";

    // Step 5: Validate all PDU fields including CRC
    validateEofPdu(eofPdu, component.getLocalEidParam(), testPeerId,
                  testSequenceId, testConditionCode, fileSize, srcFile);
}

void CfdpManagerTester::testFinPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CF_CFDP_SendFin()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Step 1: Configure transaction for FIN PDU emission
    const char* srcFile = "/tmp/test_fin.bin";
    const char* dstFile = "/tmp/dest_fin.bin";
    const CfdpFileSize fileSize = 8192;
    const U8 channelId = 0;
    const U32 testSequenceId = 77;
    const U32 testPeerId = 200;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_R2,  // Receiver, class 2 (acknowledged mode)
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Setup transaction to simulate file reception complete
    const CF_CFDP_ConditionCode_t testConditionCode = CF_CFDP_ConditionCode_NO_ERROR;
    const CF_CFDP_FinDeliveryCode_t testDeliveryCode = CF_CFDP_FinDeliveryCode_COMPLETE;
    const CF_CFDP_FinFileStatus_t testFileStatus = CF_CFDP_FinFileStatus_RETAINED;

    // Clear port history before test
    this->clearHistory();

    // Step 2: Invoke receiver to emit FIN PDU
    CfdpStatus::T status = CF_CFDP_SendFin(txn, testDeliveryCode, testFileStatus, testConditionCode);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendFin failed";

    // Step 3: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Step 4: Deserialize complete FIN PDU (header + body)
    Cfdp::Pdu::FinPdu finPdu;
    bool finOk = deserializeFinPdu(pduBuffer, finPdu);
    ASSERT_TRUE(finOk) << "Failed to deserialize FIN PDU";

    // Step 5: Validate all PDU fields
    // FIN PDU is sent from receiver (testPeerId) to sender (component.getLocalEidParam())
    // So source=testPeerId, dest=component.getLocalEidParam()
    validateFinPdu(finPdu, testPeerId, component.getLocalEidParam(),
                  testSequenceId,
                  static_cast<Cfdp::ConditionCode>(testConditionCode),
                  static_cast<Cfdp::FinDeliveryCode>(testDeliveryCode),
                  static_cast<Cfdp::FinFileStatus>(testFileStatus));
}

void CfdpManagerTester::testAckPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CF_CFDP_SendAck()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Step 1: Configure transaction for ACK PDU emission
    const char* srcFile = "/tmp/test_ack.bin";
    const char* dstFile = "/tmp/dest_ack.bin";
    const CfdpFileSize fileSize = 2048;
    const U8 channelId = 0;
    const U32 testSequenceId = 88;
    const U32 testPeerId = 175;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_R2,  // Receiver, class 2 (acknowledged mode)
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Setup test parameters for ACK PDU
    const CF_CFDP_AckTxnStatus_t testTransactionStatus = CF_CFDP_AckTxnStatus_ACTIVE;
    const CF_CFDP_FileDirective_t testDirectiveCode = CF_CFDP_FileDirective_EOF;
    const CF_CFDP_ConditionCode_t testConditionCode = CF_CFDP_ConditionCode_NO_ERROR;

    // Clear port history before test
    this->clearHistory();

    // Step 2: Invoke CF_CFDP_SendAck to emit ACK PDU
    CfdpStatus::T status = CF_CFDP_SendAck(txn, testTransactionStatus, testDirectiveCode,
                                           testConditionCode, testPeerId, testSequenceId);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendAck failed";

    // Step 3: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Step 4: Deserialize complete ACK PDU (header + body)
    Cfdp::Pdu::AckPdu ackPdu;
    bool ackOk = deserializeAckPdu(pduBuffer, ackPdu);
    ASSERT_TRUE(ackOk) << "Failed to deserialize ACK PDU";

    // Step 5: Validate all PDU fields
    // ACK PDU is sent from receiver (component.getLocalEidParam()) to sender (testPeerId)
    // acknowledging the EOF directive
    // Note: Legacy engine sets subtype code to 1 for non-extended features (see CfdpEngine.cpp:433)
    const U8 expectedSubtypeCode = 1;
    validateAckPdu(ackPdu, component.getLocalEidParam(), testPeerId,
                  testSequenceId,
                  static_cast<Cfdp::FileDirective>(testDirectiveCode),
                  expectedSubtypeCode,
                  static_cast<Cfdp::ConditionCode>(testConditionCode),
                  static_cast<Cfdp::AckTxnStatus>(testTransactionStatus));
}

void CfdpManagerTester::testNakPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Construct NAK PDU with scope_start and scope_end
    // 3. Invoke CF_CFDP_SendNak()
    // 4. Capture PDU from dataOut and validate

    // Step 1: Configure transaction for NAK PDU emission
    const char* srcFile = "/tmp/test_nak.bin";
    const char* dstFile = "/tmp/dest_nak.bin";
    const CfdpFileSize fileSize = 4096;
    const U8 channelId = 0;
    const U32 testSequenceId = 99;
    const U32 testPeerId = 200;

    CF_Transaction_t* txn = setupTestTransaction(
        CF_TxnState_R2,  // Receiver, class 2 (acknowledged mode)
        channelId,
        srcFile,
        dstFile,
        fileSize,
        testSequenceId,
        testPeerId
    );
    ASSERT_NE(txn, nullptr) << "Failed to create test transaction";

    // Clear port history before test
    this->clearHistory();

    // Step 2: Construct PDU buffer with NAK header
    CF_Logical_PduBuffer_t* ph = CF_CFDP_ConstructPduHeader(
        txn,
        CF_CFDP_FileDirective_NAK,
        component.getLocalEidParam(),  // NAK sent from receiver (local)
        testPeerId,                     // to sender (peer)
        1,  // towards sender
        testSequenceId,
        0   // directive PDU (not file data)
    );
    ASSERT_NE(ph, nullptr) << "Failed to construct PDU header";

    // Step 3: Setup NAK-specific fields
    CF_Logical_PduNak_t* nak = &ph->int_header.nak;
    const CfdpFileSize testScopeStart = 0;      // Scope covers entire file
    const CfdpFileSize testScopeEnd = fileSize; // Scope covers entire file
    nak->scope_start = testScopeStart;
    nak->scope_end = testScopeEnd;

    // Add segment requests indicating specific missing data ranges
    // Simulates receiver requesting retransmission of 3 gaps
    nak->segment_list.num_segments = 3;

    // Gap 1: Missing data from 512-1024
    nak->segment_list.segments[0].offset_start = 512;
    nak->segment_list.segments[0].offset_end = 1024;

    // Gap 2: Missing data from 2048-2560
    nak->segment_list.segments[1].offset_start = 2048;
    nak->segment_list.segments[1].offset_end = 2560;

    // Gap 3: Missing data from 3584-4096
    nak->segment_list.segments[2].offset_start = 3584;
    nak->segment_list.segments[2].offset_end = 4096;

    // Step 4: Invoke CF_CFDP_SendNak to emit NAK PDU
    CfdpStatus::T status = CF_CFDP_SendNak(txn, ph);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendNak failed";

    // Step 5: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Step 6: Deserialize complete NAK PDU (header + body)
    Cfdp::Pdu::NakPdu nakPdu;
    bool nakOk = deserializeNakPdu(pduBuffer, nakPdu);
    ASSERT_TRUE(nakOk) << "Failed to deserialize NAK PDU";

    // Step 7: Validate all PDU fields including segment requests
    // NAK PDU is sent from receiver (component.getLocalEidParam()) to sender (testPeerId)
    // requesting retransmission of missing data

    // Define expected segment requests
    Cfdp::Pdu::SegmentRequest expectedSegments[3];
    expectedSegments[0].offsetStart = 512;
    expectedSegments[0].offsetEnd = 1024;
    expectedSegments[1].offsetStart = 2048;
    expectedSegments[1].offsetEnd = 2560;
    expectedSegments[2].offsetStart = 3584;
    expectedSegments[2].offsetEnd = 4096;

    // Validate all fields including segments
    validateNakPdu(nakPdu, component.getLocalEidParam(), testPeerId,
                  testSequenceId, testScopeStart, testScopeEnd,
                  3, expectedSegments);
}

}  // namespace Ccsds

}  // namespace Svc
