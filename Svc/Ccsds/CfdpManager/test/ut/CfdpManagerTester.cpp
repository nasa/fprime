// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/SerialBuffer.hpp>
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
    const U32 fileSize = 1024;
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

    // Validate header fields using getters (no manual bit extraction needed)
    const Cfdp::Pdu::Header& header = metadataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_METADATA, header.getType()) << "Expected T_METADATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_UNACKNOWLEDGED, header.getTxmMode()) << "Expected unacknowledged mode for class 1";
    EXPECT_EQ(component.getLocalEidParam(), header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(testPeerId, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(testSequenceId, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Step 5: Validate metadata fields using getters
    EXPECT_EQ(fileSize, metadataPdu.getFileSize()) << "File size mismatch";
    EXPECT_EQ(Cfdp::CHECKSUM_TYPE_MODULAR, metadataPdu.getChecksumType()) << "Expected modular checksum type";
    EXPECT_EQ(0, metadataPdu.getClosureRequested()) << "Class 1 should not request closure";

    // Validate source filename (use memcmp with length, not STREQ, since CFDP uses LV not null-terminated)
    const char* rxSrcFilename = metadataPdu.getSourceFilename();
    ASSERT_NE(nullptr, rxSrcFilename) << "Source filename is null";
    FwSizeType srcLen = strlen(srcFile);
    EXPECT_EQ(0, memcmp(rxSrcFilename, srcFile, srcLen)) << "Source filename mismatch";

    // Validate destination filename (use memcmp with length, not STREQ, since CFDP uses LV not null-terminated)
    const char* rxDstFilename = metadataPdu.getDestFilename();
    ASSERT_NE(nullptr, rxDstFilename) << "Destination filename is null";
    FwSizeType dstLen = strlen(dstFile);
    EXPECT_EQ(0, memcmp(rxDstFilename, dstFile, dstLen)) << "Destination filename mismatch";
}

void CfdpManagerTester::testFileDataPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Manually construct CF_Logical_PduBuffer_t with File Data header (mimicking CF_CFDP_S_SendFileData)
    // 3. Invoke CF_CFDP_SendFd()
    // 4. Capture PDU from dataOut and validate

    // Step 1: Configure transaction for File Data PDU emission
    const char* srcFile = "/tmp/test_file.bin";
    const char* dstFile = "/tmp/dest_file.bin";
    const U32 fileSize = 2048;
    const U8 channelId = 0;
    const U32 testSequenceId = 42;
    const U32 testPeerId = 200;
    const U32 fileOffset = 512;

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

    // Step 2: Construct PDU buffer with File Data header (mimicking CF_CFDP_S_SendFileData)
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

    // Get pointer to data area and write test data
    size_t actual_bytes = CF_CODEC_GET_REMAIN(ph->penc);
    const U16 testDataSize = (actual_bytes > 256) ? 256 : static_cast<U16>(actual_bytes);
    U8* data_ptr = CF_CFDP_DoEncodeChunk(ph->penc, testDataSize);
    ASSERT_NE(data_ptr, nullptr) << "Failed to get data pointer";

    // Fill with test pattern
    for (U16 i = 0; i < testDataSize; ++i) {
        data_ptr[i] = static_cast<U8>(i & 0xFF);
    }

    fd->data_len = testDataSize;
    fd->data_ptr = data_ptr;

    // Step 3: Invoke CF_CFDP_SendFd to emit File Data PDU
    CfdpStatus::T status = CF_CFDP_SendFd(txn, ph);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "CF_CFDP_SendFd failed";

    // Step 4: Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Deserialize complete File Data PDU (header + body)
    Cfdp::Pdu::FileDataPdu fileDataPdu;
    bool fileDataOk = deserializeFileDataPdu(pduBuffer, fileDataPdu);
    ASSERT_TRUE(fileDataOk) << "Failed to deserialize File Data PDU";

    // Validate header fields using getters
    const Cfdp::Pdu::Header& header = fileDataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_FILE_DATA, header.getType()) << "Expected T_FILE_DATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(Cfdp::TRANSMISSION_MODE_UNACKNOWLEDGED, header.getTxmMode()) << "Expected unacknowledged mode for class 1";
    EXPECT_EQ(component.getLocalEidParam(), header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(testPeerId, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(testSequenceId, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate file data fields using getters
    EXPECT_EQ(fileOffset, fileDataPdu.getOffset()) << "File offset mismatch";
    EXPECT_EQ(testDataSize, fileDataPdu.getDataSize()) << "Data size mismatch";
    ASSERT_NE(nullptr, fileDataPdu.getData()) << "Data pointer is null";

    // Validate data content
    for (U16 i = 0; i < testDataSize; ++i) {
        EXPECT_EQ(static_cast<U8>(i & 0xFF), fileDataPdu.getData()[i])
            << "Data mismatch at byte " << i;
    }
}

}  // namespace Ccsds

}  // namespace Svc
