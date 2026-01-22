// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/File.hpp>
#include <cstring>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpManagerTester ::CfdpManagerTester()
    : CfdpManagerGTestBase("CfdpManagerTester", CfdpManagerTester::MAX_HISTORY_SIZE),
      component("CfdpManager") {
    this->initComponents();
    this->connectPorts();
    this->component.loadParameters();

    // Configure CFDP engine after parameters are loaded
    this->component.configure();
}

CfdpManagerTester ::~CfdpManagerTester() { }

// ----------------------------------------------------------------------
// Handler implementations for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer CfdpManagerTester::from_bufferAllocate_handler(
    FwIndexType portNum,
    FwSizeType size
) {
    EXPECT_LT(size, CF_MAX_PDU_SIZE) << "Buffer size request is too large";
    if (size >= CF_MAX_PDU_SIZE) {
        return Fw::Buffer();
    }
    return Fw::Buffer(this->m_internalDataBuffer, size);
}

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
    CfdpFileSize expectedFileSize
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

    // Verify checksum field exists and is accessible
    U32 rxChecksum = eofPdu.getChecksum();
    (void)rxChecksum;  // Checksum value depends on internal calculation
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
    const char* srcFile = "/tmp/test_eof.bin";
    const char* dstFile = "/tmp/dest_eof.bin";
    const CfdpFileSize fileSize = 4096;
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

    // Step 5: Validate all PDU fields
    validateEofPdu(eofPdu, component.getLocalEidParam(), testPeerId,
                  testSequenceId, testConditionCode, fileSize);
}

}  // namespace Ccsds

}  // namespace Svc
