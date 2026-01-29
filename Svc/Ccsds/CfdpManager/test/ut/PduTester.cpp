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
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpUtils.hpp>
#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdio>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// PDU Test Helper Implementations
// ----------------------------------------------------------------------

CfdpTransaction* CfdpManagerTester::setupTestTransaction(
    CF_TxnState_t state,
    U8 channelId,
    const char* srcFilename,
    const char* dstFilename,
    U32 fileSize,
    U32 sequenceId,
    U32 peerId
) {
    // For white box testing, directly use the first transaction for the specified channel
    CfdpChannel* chan = component.m_engine->m_channels[channelId];
    FW_ASSERT(chan != nullptr);

    CfdpTransaction* txn = chan->getTransaction(0);  // Use first transaction for channel
    CF_History_t* history = chan->getHistory(0);     // Use first history for channel

    // Initialize transaction state
    txn->m_state = state;
    txn->m_fsize = fileSize;
    txn->m_chan_num = channelId;
    txn->m_cfdpManager = &this->component;
    txn->m_history = history;

    // Set transaction class based on state
    // S2/R2 are Class 2, S1/R1 are Class 1
    if ((state == CF_TxnState_S2) || (state == CF_TxnState_R2)) {
        txn->m_txn_class = CfdpClass::CLASS_2;
    } else {
        txn->m_txn_class = CfdpClass::CLASS_1;
    }

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

// ----------------------------------------------------------------------
// PDU Verify Functions
// ----------------------------------------------------------------------

void CfdpManagerTester::verifyMetadataPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    CfdpFileSize expectedFileSize,
    const char* expectedSourceFilename,
    const char* expectedDestFilename,
    Svc::Ccsds::Cfdp::Class expectedClass
) {
    // Deserialize PDU
    Cfdp::Pdu::MetadataPdu metadataPdu;
    Fw::SerializeStatus status = metadataPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize Metadata PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = metadataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_METADATA, header.getType()) << "Expected T_METADATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(expectedClass, header.getTxmMode()) << "TX mode mismatch";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate metadata-specific fields
    EXPECT_EQ(expectedFileSize, metadataPdu.getFileSize()) << "File size mismatch";
    EXPECT_EQ(Cfdp::CHECKSUM_TYPE_MODULAR, metadataPdu.getChecksumType()) << "Expected modular checksum type";

    // Closure requested should be 0 for Class 1, 1 for Class 2
    U8 expectedClosureRequested = (expectedClass == Cfdp::CLASS_2) ? 1 : 0;
    EXPECT_EQ(expectedClosureRequested, metadataPdu.getClosureRequested())
        << "Closure requested mismatch for class " << static_cast<int>(expectedClass);

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

void CfdpManagerTester::verifyFileDataPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    U32 expectedOffset,
    U16 expectedDataSize,
    const char* filename,
    Svc::Ccsds::Cfdp::Class expectedClass
) {
    // Deserialize PDU
    Cfdp::Pdu::FileDataPdu fileDataPdu;
    Fw::SerializeStatus status = fileDataPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize File Data PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = fileDataPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_FILE_DATA, header.getType()) << "Expected T_FILE_DATA type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    EXPECT_EQ(expectedClass, header.getTxmMode()) << "TX mode mismatch";
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

void CfdpManagerTester::verifyEofPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::ConditionCode expectedConditionCode,
    CfdpFileSize expectedFileSize,
    const char* sourceFilename
) {
    // Deserialize PDU
    Cfdp::Pdu::EofPdu eofPdu;
    Fw::SerializeStatus status = eofPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize EOF PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = eofPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_EOF, header.getType()) << "Expected T_EOF type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_RECEIVER, header.getDirection()) << "Expected direction toward receiver";
    // Note: Can be either acknowledged or unacknowledged depending on class
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate EOF-specific fields
    EXPECT_EQ(expectedConditionCode, eofPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedFileSize, eofPdu.getFileSize()) << "File size mismatch";

    // For Class 1 (unacknowledged), checksum validation is optional
    // For Class 2 (acknowledged), validate checksum if non-zero
    U32 rxChecksum = eofPdu.getChecksum();
    if (rxChecksum != 0) {
        // Compute file CRC and validate against EOF PDU checksum
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
}

void CfdpManagerTester::verifyFinPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::ConditionCode expectedConditionCode,
    Cfdp::FinDeliveryCode expectedDeliveryCode,
    Cfdp::FinFileStatus expectedFileStatus
) {
    // Deserialize PDU
    Cfdp::Pdu::FinPdu finPdu;
    Fw::SerializeStatus status = finPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize FIN PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = finPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_FIN, header.getType()) << "Expected T_FIN type";
    EXPECT_EQ(Cfdp::DIRECTION_TOWARD_SENDER, header.getDirection()) << "Expected direction toward sender";
    EXPECT_EQ(Cfdp::CLASS_2, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate FIN-specific fields
    EXPECT_EQ(expectedConditionCode, finPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedDeliveryCode, finPdu.getDeliveryCode()) << "Delivery code mismatch";
    EXPECT_EQ(expectedFileStatus, finPdu.getFileStatus()) << "File status mismatch";
}

void CfdpManagerTester::verifyAckPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    Cfdp::FileDirective expectedDirectiveCode,
    U8 expectedDirectiveSubtypeCode,
    Cfdp::ConditionCode expectedConditionCode,
    Cfdp::AckTxnStatus expectedTransactionStatus
) {
    // Deserialize PDU
    Cfdp::Pdu::AckPdu ackPdu;
    Fw::SerializeStatus status = ackPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize ACK PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = ackPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_ACK, header.getType()) << "Expected T_ACK type";
    EXPECT_EQ(Cfdp::CLASS_2, header.getTxmMode()) << "Expected acknowledged mode for class 2";
    EXPECT_EQ(expectedSourceEid, header.getSourceEid()) << "Source EID mismatch";
    EXPECT_EQ(expectedDestEid, header.getDestEid()) << "Destination EID mismatch";
    EXPECT_EQ(expectedTransactionSeq, header.getTransactionSeq()) << "Transaction sequence mismatch";

    // Validate ACK-specific fields
    EXPECT_EQ(expectedDirectiveCode, ackPdu.getDirectiveCode()) << "Directive code mismatch";
    EXPECT_EQ(expectedDirectiveSubtypeCode, ackPdu.getDirectiveSubtypeCode()) << "Directive subtype code mismatch";
    EXPECT_EQ(expectedConditionCode, ackPdu.getConditionCode()) << "Condition code mismatch";
    EXPECT_EQ(expectedTransactionStatus, ackPdu.getTransactionStatus()) << "Transaction status mismatch";
}

void CfdpManagerTester::verifyNakPdu(
    const Fw::Buffer& pduBuffer,
    U32 expectedSourceEid,
    U32 expectedDestEid,
    U32 expectedTransactionSeq,
    CfdpFileSize expectedScopeStart,
    CfdpFileSize expectedScopeEnd,
    U8 expectedNumSegments,
    const Cfdp::Pdu::SegmentRequest* expectedSegments
) {
    // Deserialize PDU
    Cfdp::Pdu::NakPdu nakPdu;
    Fw::SerializeStatus status = nakPdu.fromBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to deserialize NAK PDU";

    // Validate header fields
    const Cfdp::Pdu::Header& header = nakPdu.asHeader();
    EXPECT_EQ(Cfdp::Pdu::T_NAK, header.getType()) << "Expected T_NAK type";
    EXPECT_EQ(Cfdp::CLASS_2, header.getTxmMode()) << "Expected acknowledged mode for class 2";
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
// PDU Uplink Helper Functions
// ----------------------------------------------------------------------

void CfdpManagerTester::sendMetadataPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    CfdpFileSize fileSize,
    const char* sourceFilename,
    const char* destFilename,
    Cfdp::Class txmMode,
    U8 closureRequested
) {
    // Create and initialize Metadata PDU
    Cfdp::Pdu::MetadataPdu metadataPdu;
    metadataPdu.initialize(
        Cfdp::DIRECTION_TOWARD_RECEIVER,
        txmMode,
        sourceEid,
        transactionSeq,
        destEid,
        fileSize,
        sourceFilename,
        destFilename,
        Cfdp::CHECKSUM_TYPE_MODULAR,
        closureRequested
    );

    // Allocate buffer for PDU
    U32 pduSize = metadataPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = metadataPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize Metadata PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

void CfdpManagerTester::sendFileDataPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    CfdpFileSize offset,
    U16 dataSize,
    const U8* data,
    Cfdp::Class txmMode
) {
    // Create and initialize File Data PDU
    Cfdp::Pdu::FileDataPdu fileDataPdu;
    fileDataPdu.initialize(
        Cfdp::DIRECTION_TOWARD_RECEIVER,
        txmMode,
        sourceEid,
        transactionSeq,
        destEid,
        offset,
        dataSize,
        data
    );

    // Allocate buffer for PDU
    U32 pduSize = fileDataPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = fileDataPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize File Data PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

void CfdpManagerTester::sendEofPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    Cfdp::ConditionCode conditionCode,
    U32 checksum,
    CfdpFileSize fileSize,
    Cfdp::Class txmMode
) {
    // Create and initialize EOF PDU
    Cfdp::Pdu::EofPdu eofPdu;
    eofPdu.initialize(
        Cfdp::DIRECTION_TOWARD_RECEIVER,
        txmMode,
        sourceEid,
        transactionSeq,
        destEid,
        conditionCode,
        checksum,
        fileSize
    );

    // Allocate buffer for PDU
    U32 pduSize = eofPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = eofPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize EOF PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

void CfdpManagerTester::sendFinPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    Cfdp::ConditionCode conditionCode,
    Cfdp::FinDeliveryCode deliveryCode,
    Cfdp::FinFileStatus fileStatus
) {
    // Create and initialize FIN PDU
    Cfdp::Pdu::FinPdu finPdu;
    finPdu.initialize(
        Cfdp::DIRECTION_TOWARD_SENDER,  // FIN is sent from receiver to sender
        Cfdp::CLASS_2,  // FIN is only used in Class 2
        sourceEid,
        transactionSeq,
        destEid,
        conditionCode,
        deliveryCode,
        fileStatus
    );

    // Allocate buffer for PDU
    U32 pduSize = finPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);
    pduBuffer.setSize(pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = finPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize FIN PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

void CfdpManagerTester::sendAckPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    Cfdp::FileDirective directiveCode,
    U8 directiveSubtypeCode,
    Cfdp::ConditionCode conditionCode,
    Cfdp::AckTxnStatus transactionStatus
) {
    // Create and initialize ACK PDU
    Cfdp::Pdu::AckPdu ackPdu;
    ackPdu.initialize(
        Cfdp::DIRECTION_TOWARD_SENDER,  // ACK is sent from receiver to sender
        Cfdp::CLASS_2,  // ACK is only used in Class 2
        sourceEid,
        transactionSeq,
        destEid,
        directiveCode,
        directiveSubtypeCode,
        conditionCode,
        transactionStatus
    );

    // Allocate buffer for PDU
    U32 pduSize = ackPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);
    pduBuffer.setSize(pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = ackPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize ACK PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

void CfdpManagerTester::sendNakPdu(
    U8 channelId,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    CfdpTransactionSeq transactionSeq,
    CfdpFileSize scopeStart,
    CfdpFileSize scopeEnd,
    U8 numSegments,
    const Cfdp::Pdu::SegmentRequest* segments
) {
    // Create and initialize NAK PDU
    Cfdp::Pdu::NakPdu nakPdu;
    nakPdu.initialize(
        Cfdp::DIRECTION_TOWARD_SENDER,  // NAK is sent from receiver to sender
        Cfdp::CLASS_2,  // NAK is only used in Class 2
        sourceEid,
        transactionSeq,
        destEid,
        scopeStart,
        scopeEnd
    );

    // Verify segment count does not exceed maximum
    ASSERT_LE(numSegments, CF_NAK_MAX_SEGMENTS) << "Number of segments exceeds CF_NAK_MAX_SEGMENTS";

    // Add segment requests if provided
    for (U8 i = 0; i < numSegments; i++) {
        bool success = nakPdu.addSegment(segments[i].offsetStart, segments[i].offsetEnd);
        ASSERT_TRUE(success) << "Failed to add segment " << static_cast<int>(i) << " to NAK PDU";
    }

    // Allocate buffer for PDU
    U32 pduSize = nakPdu.bufferSize();
    Fw::Buffer pduBuffer(m_internalDataBuffer, pduSize);

    // Serialize PDU to buffer
    Fw::SerializeStatus status = nakPdu.toBuffer(pduBuffer);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, status) << "Failed to serialize NAK PDU";

    // Send PDU to CfdpManager via dataIn port
    invoke_to_dataIn(channelId, pduBuffer);
}

// ----------------------------------------------------------------------
// PDU Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testMetaDataPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CfdpEngine->sendMd()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Configure transaction for Metadata PDU emission
    const char* srcFile = "/tmp/test_source.bin";
    const char* dstFile = "/tmp/test_dest.bin";
    const CfdpFileSize fileSize = 1024;
    const U8 channelId = 0;
    const U32 testSequenceId = 98;
    const U32 testPeerId = 100;

    CfdpTransaction* txn = setupTestTransaction(
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

    // Invoke sender to emit Metadata PDU using refactored API
    CfdpStatus::T status = component.m_engine->sendMd(txn);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendMd failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify Metadata PDU
    verifyMetadataPdu(pduBuffer, component.getLocalEidParam(), testPeerId,
                      testSequenceId, fileSize, srcFile, dstFile, Cfdp::CLASS_1);
}

void CfdpManagerTester::testFileDataPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Read test file and construct File Data PDU
    // 3. Invoke CfdpEngine->sendFd()
    // 4. Capture PDU from dataOut and validate

    // Test file configuration
    const char* testFilePath = "Pdu/test/ut/data/test_file.bin";
    const U32 fileOffset = 50;   // Read from offset 50
    const U16 readSize = 64;     // Read 64 bytes

    // Configure transaction for File Data PDU emission
    const char* srcFile = testFilePath;
    const char* dstFile = "/tmp/dest_file.bin";
    const U32 fileSize = 256;  // Approximate file size
    const U8 channelId = 0;
    const U32 testSequenceId = 42;
    const U32 testPeerId = 200;

    CfdpTransaction* txn = setupTestTransaction(
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

    // Read test data from file
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

    // Construct PDU buffer with File Data header using refactored API
    CF_Logical_PduBuffer_t* ph = component.m_engine->constructPduHeader(
        txn,
        CF_CFDP_FileDirective_INVALID_MIN,  // File data PDU has invalid directive
        component.getLocalEidParam(),
        testPeerId,
        0,  // towards receiver
        testSequenceId,
        false
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

    // Invoke sendFd using refactored API
    CfdpStatus::T status = component.m_engine->sendFd(txn, ph);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendFd failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify File Data PDU
    verifyFileDataPdu(pduBuffer, component.getLocalEidParam(), testPeerId,
                      testSequenceId, fileOffset, readSize, testFilePath, Cfdp::CLASS_1);
}

void CfdpManagerTester::testEofPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CfdpEngine->sendEof()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Configure transaction for EOF PDU emission
    const char* srcFile = "Pdu/test/ut/data/test_file.bin";
    const char* dstFile = "/tmp/dest_eof.bin";
    const CfdpFileSize fileSize = 242;  // Actual size of test_file.bin
    const U8 channelId = 0;
    const U32 testSequenceId = 55;
    const U32 testPeerId = 150;

    CfdpTransaction* txn = setupTestTransaction(
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
    txn->m_state_data.send.cached_pos = fileSize;  // Simulate file transfer complete

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

    // Compute and set CRC in transaction (matches what sendEof expects)
    txn->m_crc.update(fileData, 0, fileSize);
    delete[] fileData;

    // Clear port history before test
    this->clearHistory();

    // Invoke sender to emit EOF PDU using refactored API
    CfdpStatus::T status = component.m_engine->sendEof(txn);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendEof failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify EOF PDU
    verifyEofPdu(pduBuffer, component.getLocalEidParam(), testPeerId,
                 testSequenceId, testConditionCode, fileSize, srcFile);
}

void CfdpManagerTester::testFinPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CfdpEngine->sendFin()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Configure transaction for FIN PDU emission
    const char* srcFile = "/tmp/test_fin.bin";
    const char* dstFile = "/tmp/dest_fin.bin";
    const CfdpFileSize fileSize = 8192;
    const U8 channelId = 0;
    const U32 testSequenceId = 77;
    const U32 testPeerId = 200;

    CfdpTransaction* txn = setupTestTransaction(
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

    // Invoke receiver to emit FIN PDU using refactored API
    CfdpStatus::T status = component.m_engine->sendFin(txn, testDeliveryCode, testFileStatus, testConditionCode);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendFin failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify FIN PDU
    // FIN PDU is sent from receiver (testPeerId) to sender (component.getLocalEidParam())
    // So source=testPeerId, dest=component.getLocalEidParam()
    verifyFinPdu(pduBuffer, testPeerId, component.getLocalEidParam(),
                 testSequenceId,
                 static_cast<Cfdp::ConditionCode>(testConditionCode),
                 static_cast<Cfdp::FinDeliveryCode>(testDeliveryCode),
                 static_cast<Cfdp::FinFileStatus>(testFileStatus));
}

void CfdpManagerTester::testAckPdu() {
    // Test pattern:
    // 1. Setup transaction
    // 2. Invoke CfdpEngine->sendAck()
    // 3. Capture PDU from dataOut
    // 4. Deserialize and validate

    // Configure transaction for ACK PDU emission
    const char* srcFile = "/tmp/test_ack.bin";
    const char* dstFile = "/tmp/dest_ack.bin";
    const CfdpFileSize fileSize = 2048;
    const U8 channelId = 0;
    const U32 testSequenceId = 88;
    const U32 testPeerId = 175;

    CfdpTransaction* txn = setupTestTransaction(
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

    // Invoke sendAck using refactored API
    CfdpStatus::T status = component.m_engine->sendAck(txn, testTransactionStatus, testDirectiveCode,
                                                       testConditionCode, testPeerId, testSequenceId);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendAck failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify ACK PDU
    // ACK PDU is sent from receiver (component.getLocalEidParam()) to sender (testPeerId)
    // acknowledging the EOF directive
    const U8 expectedSubtypeCode = 1;
    verifyAckPdu(pduBuffer, component.getLocalEidParam(), testPeerId,
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
    // 3. Invoke CfdpEngine->sendNak()
    // 4. Capture PDU from dataOut and validate

    // Configure transaction for NAK PDU emission
    const char* srcFile = "/tmp/test_nak.bin";
    const char* dstFile = "/tmp/dest_nak.bin";
    const CfdpFileSize fileSize = 4096;
    const U8 channelId = 0;
    const U32 testSequenceId = 99;
    const U32 testPeerId = 200;

    CfdpTransaction* txn = setupTestTransaction(
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

    // Construct PDU buffer with NAK header using refactored API
    CF_Logical_PduBuffer_t* ph = component.m_engine->constructPduHeader(
        txn,
        CF_CFDP_FileDirective_NAK,
        component.getLocalEidParam(),  // NAK sent from receiver (local)
        testPeerId,                     // to sender (peer)
        1,  // towards sender
        testSequenceId,
        false
    );
    ASSERT_NE(ph, nullptr) << "Failed to construct PDU header";

    // Setup NAK-specific fields
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

    // Invoke sendNak using refactored API
    CfdpStatus::T status = component.m_engine->sendNak(txn, ph);
    ASSERT_EQ(status, CfdpStatus::SUCCESS) << "sendNak failed";

    // Verify PDU was sent through dataOut port
    ASSERT_FROM_PORT_HISTORY_SIZE(1);

    // Get encoded PDU buffer
    const Fw::Buffer& pduBuffer = getSentPduBuffer(0);
    ASSERT_GT(pduBuffer.getSize(), 0) << "PDU size is zero";

    // Verify NAK PDU
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

    // Verify all fields including segments
    verifyNakPdu(pduBuffer, component.getLocalEidParam(), testPeerId,
                 testSequenceId, testScopeStart, testScopeEnd,
                 3, expectedSegments);
}

}  // namespace Ccsds
}  // namespace Svc
