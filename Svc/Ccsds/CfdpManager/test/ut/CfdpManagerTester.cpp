// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>
#include <Os/File.hpp>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Static member definitions
// ----------------------------------------------------------------------

constexpr FwSizeType CfdpManagerTester::MAX_PDU_COPIES;

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpManagerTester ::CfdpManagerTester()
    : CfdpManagerGTestBase("CfdpManagerTester", CfdpManagerTester::MAX_HISTORY_SIZE),
      component("CfdpManager"),
      m_pduCopyCount(0) {
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

void CfdpManagerTester::from_dataOut_handler(
    FwIndexType portNum,
    Fw::Buffer& fwBuffer
) {
    // Make a copy of the PDU data to avoid buffer reuse issues
    EXPECT_LT(m_pduCopyCount, MAX_PDU_COPIES) << "Too many PDUs sent";
    if (m_pduCopyCount < MAX_PDU_COPIES) {
        FwSizeType copySize = fwBuffer.getSize();
        if (copySize > CF_MAX_PDU_SIZE) {
            copySize = CF_MAX_PDU_SIZE;
        }
        memcpy(m_pduCopyStorage[m_pduCopyCount], fwBuffer.getData(), copySize);

        // Create a new buffer pointing to our copy
        Fw::Buffer copyBuffer(m_pduCopyStorage[m_pduCopyCount], copySize);
        m_pduCopyCount++;

        // Call base class handler with the copy
        CfdpManagerTesterBase::from_dataOut_handler(portNum, copyBuffer);
    }
}

// ----------------------------------------------------------------------
// Transaction Test Helper Implementations
// ----------------------------------------------------------------------

CF_Transaction_t* CfdpManagerTester::findTransaction(U8 chanNum, CfdpTransactionSeq seqNum) {
    // Access global cfdpEngine to search for transaction
    CF_Channel_t* chan = &cfdpEngine.channels[chanNum];

    // Search through all transaction queues (PEND, TXA, TXW, RX)
    for (U8 qIdx = 0; qIdx < CfdpQueueId::NUM; qIdx++) {
        CF_CListNode_t* head = chan->qs[qIdx];
        if (head == nullptr) {
            continue;
        }

        // Traverse circular linked list, stopping when we loop back to head
        CF_CListNode_t* node = head;
        do {
            CF_Transaction_t* txn = container_of_cpp(node, &CF_Transaction_t::cl_node);
            if (txn->history && txn->history->seq_num == seqNum) {
                return txn;
            }
            node = node->next;
        } while (node != nullptr && node != head);
    }
    return nullptr;
}

// ----------------------------------------------------------------------
// Transaction Test Implementations
// ----------------------------------------------------------------------

void CfdpManagerTester::testClass1TxNominal() {
    // Clear any port history from previous operations
    this->clearHistory();
    this->m_pduCopyCount = 0;

    // Test configuration
    const char* srcFile = "test/ut/data/test_file.bin"; 
    const char* dstFile = "test/ut/output/test_class1_tx_dst.dat";
    const U8 channelId = 0;
    const CfdpEntityId destEid = 10;
    const U8 priority = 0;

    // Step 1: Get test file size
    Os::File::Status fileStatus;
    Os::File testFile;
    FwSizeType fileSize = 0;
    fileStatus = testFile.open(srcFile, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Test file should exist";
    fileStatus = testFile.size(fileSize);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should get file size";
    testFile.close();

    // Step 2: Record initial engine state
    const U32 initialSeqNum = cfdpEngine.seq_num;

    // Step 3: Send SendFile command
    this->sendCmd_SendFile(0, 0, channelId, destEid, CfdpClass::CLASS_1,
                           CfdpKeep::KEEP, priority,
                           Fw::CmdStringArg(srcFile), Fw::CmdStringArg(dstFile));
    this->component.doDispatch();

    // Step 4: Verify command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManager::OPCODE_SENDFILE, 0, Fw::CmdResponse::OK);

    // Step 5: Verify engine state after command
    const U32 expectedSeqNum = initialSeqNum + 1;
    EXPECT_EQ(expectedSeqNum, cfdpEngine.seq_num) << "Sequence number should increment";

    // Step 6: Find the transaction
    CF_Transaction_t* txn = findTransaction(channelId, expectedSeqNum);
    ASSERT_NE(nullptr, txn) << "Transaction should exist";

    // Step 7: Verify initial transaction state (in PEND queue, not yet active)
    EXPECT_EQ(CF_TxnState_S1, txn->state) << "Should be in S1 state for Class 1 TX";
    EXPECT_EQ(0, txn->foffs) << "File offset should be 0 initially";
    EXPECT_EQ(CF_TxSubState_METADATA, txn->state_data.send.sub_state) << "Should start in METADATA sub-state";
    EXPECT_EQ(channelId, txn->chan_num) << "Channel number should match";
    EXPECT_EQ(priority, txn->priority) << "Priority should match";

    // Verify history fields
    EXPECT_EQ(expectedSeqNum, txn->history->seq_num) << "History seq_num should match";
    EXPECT_EQ(component.getLocalEidParam(), txn->history->src_eid) << "Source EID should match local EID";
    EXPECT_EQ(destEid, txn->history->peer_eid) << "Peer EID should match dest EID";
    EXPECT_STREQ(srcFile, txn->history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, txn->history->fnames.dst_filename.toChar()) << "Destination filename should match";

    // Note: fsize is not set until the file is opened in CF_CFDP_S_SubstateSendMetadata during first cycle

    // Step 8: Run first engine cycle - should send Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Step 9: Verify exactly 2 PDUs were sent in first cycle (Metadata + FileData)
    ASSERT_FROM_PORT_HISTORY_SIZE(2);

    // Step 10: Verify Metadata PDU (index 0)
    Fw::Buffer metadataPduBuffer = this->getSentPduBuffer(0);
    ASSERT_GT(metadataPduBuffer.getSize(), 0) << "Metadata PDU should be sent";

    // Verify file was opened and fsize was set
    EXPECT_EQ(fileSize, txn->fsize) << "File size should be set after file is opened";

    verifyMetadataPdu(metadataPduBuffer, component.getLocalEidParam(), destEid,
                      expectedSeqNum, static_cast<CfdpFileSize>(fileSize), srcFile, dstFile, Cfdp::CLASS_1);

    // Step 11: Verify FileData PDU (index 1)
    Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(1);
    ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU should be sent";
    verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), destEid,
                     expectedSeqNum, 0, static_cast<U16>(fileSize), srcFile, Cfdp::CLASS_1);

    // Verify file was completely read
    EXPECT_EQ(fileSize, txn->foffs) << "Should have read entire file";

    // Verify transaction progressed to EOF sub-state after sending all file data
    EXPECT_EQ(CF_TxSubState_EOF, txn->state_data.send.sub_state) << "Should progress to EOF sub-state";

    // Step 12: Run second engine cycle - should send EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Step 13: Verify exactly 1 more PDU was sent (EOF)
    ASSERT_FROM_PORT_HISTORY_SIZE(3);

    // Step 14: Verify EOF PDU (index 2)
    Fw::Buffer eofPduBuffer = this->getSentPduBuffer(2);
    ASSERT_GT(eofPduBuffer.getSize(), 0) << "EOF PDU should be sent";

    verifyEofPdu(eofPduBuffer, component.getLocalEidParam(), destEid,
                 expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(fileSize), srcFile);

    // Step 15: Run additional cycles to expire inactivity timer and recycle transaction
    // Use actual inactivity timer parameter value to ensure transaction is fully recycled
    U32 inactivityTimer = this->component.getInactivityTimerParam(channelId);
    U32 cyclesToRun = inactivityTimer + 1;
    for (U32 i = 0; i < cyclesToRun; ++i) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Verify transaction was recycled (no longer findable by sequence number)
    txn = findTransaction(channelId, expectedSeqNum);
    EXPECT_EQ(nullptr, txn) << "Transaction should be recycled after inactivity timeout";

    // Clear port history after cleanup to ensure next test starts fresh
    this->clearHistory();
    this->m_pduCopyCount = 0;
}

void CfdpManagerTester::testClass2TxNominal() {
    // Clear any port history from previous operations
    this->clearHistory();
    this->m_pduCopyCount = 0;

    // Test configuration
    const U8 channelId = 0;
    const CfdpEntityId destEid = 10;
    const U8 priority = 0;

    // Step 1: Get the actual outgoing file chunk size parameter
    // The implementation uses OutgoingFileChunkSize parameter, not CF_MAX_PDU_SIZE
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());

    // Calculate total file size for exactly 5 PDUs
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    // Step 2: Generate test file with calculated size
    const char* srcFile = "test/ut/data/test_class2_tx_5pdu.bin";
    const char* dstFile = "test/ut/output/test_class2_tx_dst.dat";

    Os::File::Status fileStatus;
    Os::File testFile;

    // Create and write test file
    fileStatus = testFile.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should create test file";

    // Fill file with test data (repeating pattern for easy verification)
    U8 writeBuffer[256];
    for (U16 i = 0; i < 256; i++) {
        writeBuffer[i] = static_cast<U8>(i);
    }

    FwSizeType bytesWritten = 0;
    while (bytesWritten < expectedFileSize) {
        FwSizeType chunkSize = (expectedFileSize - bytesWritten > 256) ? 256 : (expectedFileSize - bytesWritten);
        FwSizeType writeSize = chunkSize;
        fileStatus = testFile.write(writeBuffer, writeSize, Os::File::WAIT);
        ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should write to test file";
        ASSERT_EQ(chunkSize, writeSize) << "Should write requested bytes";
        bytesWritten += writeSize;
    }
    testFile.close();

    // Step 3: Verify test file was created with correct size
    fileStatus = testFile.open(srcFile, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Test file should exist";
    FwSizeType fileSize = 0;
    fileStatus = testFile.size(fileSize);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should get file size";
    EXPECT_EQ(expectedFileSize, fileSize) << "File should be exactly " << expectedFileSize << " bytes (5 PDUs)";
    testFile.close();

    // Step 4: Record initial engine state
    const U32 initialSeqNum = cfdpEngine.seq_num;

    // Step 5: Send SendFile command with CLASS_2
    this->sendCmd_SendFile(0, 0, channelId, destEid, CfdpClass::CLASS_2,
                           CfdpKeep::KEEP, priority,
                           Fw::CmdStringArg(srcFile), Fw::CmdStringArg(dstFile));
    this->component.doDispatch();

    // Step 6: Verify command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManager::OPCODE_SENDFILE, 0, Fw::CmdResponse::OK);

    // Step 7: Verify engine state after command
    const U32 expectedSeqNum = initialSeqNum + 1;
    EXPECT_EQ(expectedSeqNum, cfdpEngine.seq_num) << "Sequence number should increment";

    // Step 8: Find the transaction
    CF_Transaction_t* txn = findTransaction(channelId, expectedSeqNum);
    ASSERT_NE(nullptr, txn) << "Transaction should exist";

    // Step 9: Verify initial transaction state for Class 2
    EXPECT_EQ(CF_TxnState_S2, txn->state) << "Should be in S2 state for Class 2 TX";
    EXPECT_EQ(0, txn->foffs) << "File offset should be 0 initially";
    EXPECT_EQ(CF_TxSubState_METADATA, txn->state_data.send.sub_state) << "Should start in METADATA sub-state";
    EXPECT_EQ(channelId, txn->chan_num) << "Channel number should match";
    EXPECT_EQ(priority, txn->priority) << "Priority should match";

    // Verify history fields
    EXPECT_EQ(expectedSeqNum, txn->history->seq_num) << "History seq_num should match";
    EXPECT_EQ(component.getLocalEidParam(), txn->history->src_eid) << "Source EID should match local EID";
    EXPECT_EQ(destEid, txn->history->peer_eid) << "Peer EID should match dest EID";
    EXPECT_STREQ(srcFile, txn->history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, txn->history->fnames.dst_filename.toChar()) << "Destination filename should match";

    // Step 10: Run first engine cycle - should send Metadata + 5 FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Step 11: Verify 6 PDUs were sent (1 Metadata + 5 FileData)
    ASSERT_FROM_PORT_HISTORY_SIZE(6);

    // Step 12: Verify Metadata PDU (index 0)
    Fw::Buffer metadataPduBuffer = this->getSentPduBuffer(0);
    ASSERT_GT(metadataPduBuffer.getSize(), 0) << "Metadata PDU should be sent";
    verifyMetadataPdu(metadataPduBuffer, component.getLocalEidParam(), destEid,
                      expectedSeqNum, static_cast<CfdpFileSize>(expectedFileSize), srcFile, dstFile, Cfdp::CLASS_2);

    // Verify file was opened and fsize was set
    EXPECT_EQ(expectedFileSize, txn->fsize) << "File size should be set after file is opened";

    // Step 13: Verify all 5 FileData PDUs (indices 1-5)
    for (U8 pduIdx = 0; pduIdx < 5; pduIdx++) {
        Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(1 + pduIdx);
        ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU " << static_cast<int>(pduIdx) << " should be sent";

        U32 expectedOffset = pduIdx * dataPerPdu;
        U16 expectedDataSize = dataPerPdu;  // All 5 PDUs should be exactly full

        verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), destEid,
                          expectedSeqNum, expectedOffset, expectedDataSize, srcFile, Cfdp::CLASS_2);
    }

    // Verify file was completely read
    EXPECT_EQ(expectedFileSize, txn->foffs) << "Should have read entire file";

    // Verify transaction moved to CLOSEOUT_SYNC sub-state after completing file data
    // For Class 2, CF_CFDP_S2_SubstateSendEof() immediately sets sub_state to CLOSEOUT_SYNC
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, txn->state_data.send.sub_state) << "Should be in CLOSEOUT_SYNC after file data complete";
    EXPECT_TRUE(txn->flags.tx.send_eof) << "send_eof flag should be set";
    EXPECT_EQ(CF_TxnState_S2, txn->state) << "Should remain in S2 state";

    // Step 14: Run second engine cycle - should send EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Step 15: Verify 1 more PDU was sent (total 7 PDUs)
    ASSERT_FROM_PORT_HISTORY_SIZE(7);

    // Step 16: Verify EOF PDU (index 6)
    Fw::Buffer eofPduBuffer = this->getSentPduBuffer(6);
    ASSERT_GT(eofPduBuffer.getSize(), 0) << "EOF PDU should be sent";
    verifyEofPdu(eofPduBuffer, component.getLocalEidParam(), destEid,
                 expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(expectedFileSize), srcFile);

    // Verify transaction remains in S2/CLOSEOUT_SYNC waiting for EOF-ACK
    EXPECT_EQ(CF_TxnState_S2, txn->state) << "Should remain in S2 state until EOF-ACK received";
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, txn->state_data.send.sub_state) << "Should remain in CLOSEOUT_SYNC waiting for EOF-ACK";
    EXPECT_FALSE(txn->flags.tx.send_eof) << "send_eof flag should be cleared after EOF sent";

    // For a complete test, we would simulate receiving an EOF-ACK here
    // and verify the transaction completes. This is left as a future enhancement.
}

}  // namespace Ccsds

}  // namespace Svc
