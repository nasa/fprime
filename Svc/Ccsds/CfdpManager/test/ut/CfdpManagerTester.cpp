// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include "Fw/Types/SerialBuffer.hpp>
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
    Svc::Ccsds::CfdpPduHeader& header
) {
    // Create a mutable copy for deserialization since getDeserializer() is non-const
    Fw::Buffer mutableBuffer = pduBuffer;
    Fw::ExternalSerializeBufferWithMemberCopy deserialiser = mutableBuffer.getDeserializer();
    Fw::SerializeStatus status = header.deserializeFrom(deserialiser);
    if (status != Fw::FW_SERIALIZE_OK) {
        std::cout << "deserializePduHeader failed with status: " << status << std::endl;
    }
    return (status == Fw::FW_SERIALIZE_OK);
}

bool CfdpManagerTester::validateHeaderFlags(
    U8 flags,
    U8 expectedVersion,
    U8 expectedPduType,
    U8 expectedDirection,
    U8 expectedTxMode
) {
    // Extract bit fields from flags byte
    // CFDP Blue Book 5.1: flags byte layout
    // bit 0: large_file_flag
    // bit 1: crc_flag
    // bit 2: txm_mode (0=ack, 1=unack)
    // bit 3: direction (0=toward receiver, 1=toward sender)
    // bit 4: pdu_type (0=directive, 1=file data)
    // bits 5-7: version (should be 001b = 1)

    U8 version = (flags >> 5) & 0x07;
    U8 pduType = (flags >> 4) & 0x01;
    U8 direction = (flags >> 3) & 0x01;
    U8 txMode = (flags >> 2) & 0x01;

    std::cout << "version: " << version << ", pduType: " << pduType << std::endl;
    std::cout << "direction: " << direction << ", txMode: " << txMode << std::endl;

    bool match = true;
    match &= (version == expectedVersion);
    match &= (pduType == expectedPduType);
    match &= (direction == expectedDirection);
    match &= (txMode == expectedTxMode);

    return match;
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

    // Step 4: Deserialize and validate PDU header
    Svc::Ccsds::CfdpPduHeader header;
    bool headerOk = deserializePduHeader(pduBuffer, header);
    ASSERT_TRUE(headerOk) << "Failed to deserialize PDU header";

    // Validate header flags using getter
    bool flagsOk = validateHeaderFlags(
        header.get_flags(),
        1,  // version
        0,  // pdu_type (directive)
        0,  // direction (toward receiver)
        1   // txm_mode (unacknowledged for class 1)
    );
    ASSERT_TRUE(flagsOk) << "PDU header flags validation failed";

    // Validate header entity IDs and transaction sequence using getters
    EXPECT_EQ(header.get_sourceEid(), component.getLocalEidParam());
    EXPECT_EQ(header.get_destinationEid(), testPeerId);
    EXPECT_EQ(header.get_transactionSeq(), testSequenceId);

    // // Step 5: Deserialize file directive header
    // FwSizeType offset = header.SERIALIZED_SIZE;

    // Svc::Ccsds::CfdpFileDirectiveHeader directiveHdr;
    // Fw::SerialBuffer directiveBuffer(
    //     const_cast<U8*>(pduBytes + offset),
    //     directiveHdr.SERIALIZED_SIZE
    // );

    // Fw::SerializeStatus serStatus = directiveHdr.deserializeFrom(directiveBuffer);
    // ASSERT_EQ(serStatus, Fw::FW_SERIALIZE_OK)
    //     << "Failed to deserialize directive header";

    // ASSERT_EQ(directiveHdr.get_directiveCode(),
    //           Svc::Ccsds::CfdpFileDirective::METADATA)
    //     << "Expected METADATA directive code (7)";

    // // Step 6: Deserialize and validate Metadata PDU body
    // offset += directiveHdr.SERIALIZED_SIZE;

    // Svc::Ccsds::CfdpMetadataPdu mdPdu;
    // Fw::SerialBuffer mdBuffer(
    //     const_cast<U8*>(pduBytes + offset),
    //     mdPdu.SERIALIZED_SIZE
    // );

    // serStatus = mdPdu.deserializeFrom(mdBuffer);
    // ASSERT_EQ(serStatus, Fw::FW_SERIALIZE_OK)
    //     << "Failed to deserialize metadata PDU body";

    // // Validate metadata fields using getters
    // EXPECT_EQ(mdPdu.get_fileSize(), fileSize) << "File size mismatch";

    // // Validate segmentation control byte using getter
    // U8 segCtrl = mdPdu.get_segmentationControl();
    // U8 checksumType = segCtrl & 0x0F;
    // U8 closureRequested = (segCtrl >> 7) & 0x01;
    // EXPECT_EQ(checksumType, 0) << "Expected modular checksum";
    // EXPECT_EQ(closureRequested, 0) << "Class 1 should not request closure";

    // // Validate source filename LV pair using getters
    // EXPECT_EQ(mdPdu.get_sourceFilenameLength(), strlen(srcFile))
    //     << "Source filename length mismatch";
    // EXPECT_EQ(memcmp(mdPdu.get_sourceFilename(), srcFile, strlen(srcFile)), 0)
    //     << "Source filename mismatch";

    // // Validate destination filename LV pair using getters
    // EXPECT_EQ(mdPdu.get_destFilenameLength(), strlen(dstFile))
    //     << "Destination filename length mismatch";
    // EXPECT_EQ(memcmp(mdPdu.get_destFilename(), dstFile, strlen(dstFile)), 0)
    //     << "Destination filename mismatch";
}

}  // namespace Ccsds

}  // namespace Svc
