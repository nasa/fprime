// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>

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

CfdpTransaction* CfdpManagerTester::findTransaction(U8 chanNum, CfdpTransactionSeq seqNum) {
    // Grab requested channel
    CfdpChannel* chan = component.m_engine->m_channels[chanNum];

    // Search through all transaction queues (PEND, TXA, TXW, RX, FREE)
    // Skip HIST and HIST_FREE as they contain CF_History_t, not CfdpTransaction
    for (U8 qIdx = 0; qIdx < Cfdp::QueueId::NUM; qIdx++) {
        // Skip history queues (HIST=4, HIST_FREE=5)
        if (qIdx == Cfdp::QueueId::HIST || qIdx == Cfdp::QueueId::HIST_FREE) {
            continue;
        }

        CF_CListNode_t* head = chan->m_qs[qIdx];
        if (head == nullptr) {
            continue;
        }

        // Traverse circular linked list, stopping when we loop back to head
        CF_CListNode_t* node = head;
        do {
            CfdpTransaction* txn = container_of_cpp(node, &CfdpTransaction::m_cl_node);
            if (txn->m_history && txn->m_history->seq_num == seqNum) {
                return txn;
            }
            node = node->next;
        } while (node != nullptr && node != head);
    }
    return nullptr;
}

// ----------------------------------------------------------------------
// Test Helper Function Implementations
// ----------------------------------------------------------------------

void CfdpManagerTester::createAndVerifyTestFile(const char* filePath, FwSizeType expectedFileSize, FwSizeType& actualFileSize) {
    Os::File::Status fileStatus;
    Os::File testFile;

    // Create file with repeating 0-255 pattern
    fileStatus = testFile.open(filePath, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should create test file";

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

    // Verify file and get size
    fileStatus = testFile.open(filePath, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Test file should exist";
    fileStatus = testFile.size(actualFileSize);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should get file size";
    testFile.close();

    EXPECT_EQ(expectedFileSize, actualFileSize) << "File size should match expected size";
}

void CfdpManagerTester::setupTxTransaction(
    const char* srcFile,
    const char* dstFile,
    U8 channelId,
    CfdpEntityId destEid,
    Cfdp::Class cfdpClass,
    U8 priority,
    CF_TxnState_t expectedState,
    TransactionSetup& setup)
{
    const U32 initialSeqNum = component.m_engine->m_seqNum;

    this->sendCmd_SendFile(0, 0, channelId, destEid, cfdpClass,
                          Cfdp::Keep::KEEP, priority,
                          Fw::CmdStringArg(srcFile), Fw::CmdStringArg(dstFile));
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManager::OPCODE_SENDFILE, 0, Fw::CmdResponse::OK);

    setup.expectedSeqNum = initialSeqNum + 1;
    EXPECT_EQ(setup.expectedSeqNum, component.m_engine->m_seqNum) << "Sequence number should increment";

    setup.txn = findTransaction(channelId, setup.expectedSeqNum);
    ASSERT_NE(nullptr, setup.txn) << "Transaction should exist";

    // Now verify initial state
    EXPECT_EQ(expectedState, setup.txn->m_state) << "Should be in expected state";
    EXPECT_EQ(0, setup.txn->m_foffs) << "File offset should be 0 initially";
    EXPECT_EQ(CF_TxSubState_METADATA, setup.txn->m_state_data.send.sub_state) << "Should start in METADATA sub-state";
    EXPECT_EQ(channelId, setup.txn->m_chan_num) << "Channel number should match";
    EXPECT_EQ(priority, setup.txn->m_priority) << "Priority should match";

    EXPECT_EQ(setup.expectedSeqNum, setup.txn->m_history->seq_num) << "History seq_num should match";
    EXPECT_EQ(component.getLocalEidParam(), setup.txn->m_history->src_eid) << "Source EID should match local EID";
    EXPECT_EQ(destEid, setup.txn->m_history->peer_eid) << "Peer EID should match dest EID";
    EXPECT_STREQ(srcFile, setup.txn->m_history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, setup.txn->m_history->fnames.dst_filename.toChar()) << "Destination filename should match";
}

void CfdpManagerTester::setupRxTransaction(
    const char* srcFile,
    const char* dstFile,
    U8 channelId,
    CfdpEntityId sourceEid,
    Cfdp::Class::T cfdpClass,
    U32 fileSize,
    U32 transactionSeq,
    CF_TxnState_t expectedState,
    TransactionSetup& setup)
{
    // Send Metadata PDU to initiate RX transaction
    U8 closureRequested = (cfdpClass == Cfdp::Class::CLASS_1) ? 0 : 1;

    this->sendMetadataPdu(
        channelId,
        sourceEid,                         
        component.getLocalEidParam(),     
        transactionSeq,
        fileSize,
        srcFile,
        dstFile,
        cfdpClass,
        closureRequested
    );
    this->component.doDispatch();

    // Find the created transaction
    setup.expectedSeqNum = transactionSeq;
    setup.txn = findTransaction(channelId, transactionSeq);
    ASSERT_NE(nullptr, setup.txn) << "RX transaction should be created after Metadata PDU";

    // Verify transaction state
    EXPECT_EQ(expectedState, setup.txn->m_state) << "Should be in expected RX state";
    EXPECT_EQ(CF_RxSubState_FILEDATA, setup.txn->m_state_data.receive.sub_state) << "Should start in FILEDATA sub-state";
    EXPECT_EQ(channelId, setup.txn->m_chan_num) << "Channel number should match";
    EXPECT_TRUE(setup.txn->m_flags.rx.md_recv) << "md_recv flag should be set after Metadata PDU";

    // Verify transaction history
    EXPECT_EQ(transactionSeq, setup.txn->m_history->seq_num) << "History seq_num should match";
    EXPECT_EQ(sourceEid, setup.txn->m_history->src_eid) << "Source EID should match ground EID (sender)";
    EXPECT_EQ(sourceEid, setup.txn->m_history->peer_eid) << "Peer EID should match ground EID (the remote peer)";
    EXPECT_STREQ(srcFile, setup.txn->m_history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, setup.txn->m_history->fnames.dst_filename.toChar()) << "Destination filename should match";
}

void CfdpManagerTester::waitForTransactionRecycle(U8 channelId, U32 expectedSeqNum) {
    this->clearHistory();
    this->m_pduCopyCount = 0;

    U32 inactivityTimer = this->component.getInactivityTimerParam(channelId);
    U32 cyclesToRun = inactivityTimer + 1;
    for (U32 i = 0; i < cyclesToRun; ++i) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    CfdpTransaction* txn = findTransaction(channelId, expectedSeqNum);
    EXPECT_EQ(nullptr, txn) << "Transaction should be recycled after inactivity timeout";
}

void CfdpManagerTester::completeClass2Handshake(
    U8 channelId,
    CfdpEntityId destEid,
    U32 expectedSeqNum,
    CfdpTransaction* txn)
{
    // Send EOF-ACK
    this->sendAckPdu(
        channelId,
        component.getLocalEidParam(),
        destEid,
        expectedSeqNum,
        Cfdp::FILE_DIRECTIVE_END_OF_FILE,
        0,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::ACK_TXN_STATUS_ACTIVE
    );
    this->component.doDispatch();

    EXPECT_TRUE(txn->m_flags.tx.eof_ack_recv) << "eof_ack_recv flag should be set after EOF-ACK received";
    EXPECT_FALSE(txn->m_flags.com.ack_timer_armed) << "ack_timer_armed should be cleared after EOF-ACK";
    EXPECT_EQ(CF_TxnState_S2, txn->m_state) << "Should remain in S2 state waiting for FIN";
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, txn->m_state_data.send.sub_state) << "Should remain in CLOSEOUT_SYNC waiting for FIN";

    // Send FIN
    this->sendFinPdu(
        channelId,
        component.getLocalEidParam(),
        destEid,
        expectedSeqNum,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::FIN_DELIVERY_CODE_COMPLETE,
        Cfdp::FIN_FILE_STATUS_RETAINED
    );
    this->component.doDispatch();

    EXPECT_TRUE(txn->m_flags.tx.fin_recv) << "fin_recv flag should be set after FIN received";
    EXPECT_EQ(CF_TxnState_HOLD, txn->m_state) << "Should move to HOLD state after FIN received";
    EXPECT_TRUE(txn->m_flags.tx.send_fin_ack) << "send_fin_ack flag should be set";

    // Run cycle to send FIN-ACK
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
}

void CfdpManagerTester::verifyFinAckPdu(
    FwIndexType pduIndex,
    CfdpEntityId sourceEid,
    CfdpEntityId destEid,
    U32 expectedSeqNum)
{
    Fw::Buffer finAckPduBuffer = this->getSentPduBuffer(pduIndex);
    ASSERT_GT(finAckPduBuffer.getSize(), 0) << "FIN-ACK PDU should be sent";

    verifyAckPdu(finAckPduBuffer,
                 sourceEid,
                 destEid,
                 expectedSeqNum,
                 Cfdp::FILE_DIRECTIVE_FIN,
                 1,
                 Cfdp::CONDITION_CODE_NO_ERROR,
                 Cfdp::ACK_TXN_STATUS_TERMINATED
    );
}

void CfdpManagerTester::verifyMetadataPduAtIndex(
    FwIndexType pduIndex,
    const TransactionSetup& setup,
    FwSizeType fileSize,
    const char* srcFile,
    const char* dstFile,
    Cfdp::Class::T cfdpClass)
{
    Fw::Buffer metadataPduBuffer = this->getSentPduBuffer(pduIndex);
    ASSERT_GT(metadataPduBuffer.getSize(), 0) << "Metadata PDU should be sent";
    EXPECT_EQ(fileSize, setup.txn->m_fsize) << "File size should be set after file is opened";
    verifyMetadataPdu(metadataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                      setup.expectedSeqNum, static_cast<CfdpFileSize>(fileSize), srcFile, dstFile, cfdpClass);
}

void CfdpManagerTester::verifyMultipleFileDataPdus(
    FwIndexType startIndex,
    U8 numPdus,
    const TransactionSetup& setup,
    U16 dataPerPdu,
    const char* srcFile,
    Cfdp::Class::T cfdpClass)
{
    for (U8 pduIdx = 0; pduIdx < numPdus; pduIdx++) {
        Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(startIndex + pduIdx);
        ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU " << static_cast<int>(pduIdx) << " should be sent";
        verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                         setup.expectedSeqNum, pduIdx * dataPerPdu, dataPerPdu, srcFile, cfdpClass);
    }
}

void CfdpManagerTester::cleanupTestFile(const char* filePath) {
    Os::FileSystem::Status fsStatus = Os::FileSystem::removeFile(filePath);
    EXPECT_EQ(Os::FileSystem::OP_OK, fsStatus) << "Should remove test file";
}

void CfdpManagerTester::verifyReceivedFile(
    const char* filePath,
    const U8* expectedData,
    FwSizeType expectedSize)
{
    // Read destination file
    U8* receivedData = new U8[expectedSize];
    Os::File file;
    Os::File::Status fileStatus = file.open(filePath, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Received file should exist";

    FwSizeType bytesRead = expectedSize;
    fileStatus = file.read(receivedData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Should read received file successfully";
    ASSERT_EQ(expectedSize, bytesRead) << "Received file size should match expected size";

    // Compare content byte-by-byte
    for (FwSizeType i = 0; i < expectedSize; ++i) {
        EXPECT_EQ(expectedData[i], receivedData[i]) << "File content mismatch at byte " << i;
    }

    // Clean up buffer
    delete[] receivedData;
}

// ----------------------------------------------------------------------
// Transaction Test Implementations
// ----------------------------------------------------------------------

void CfdpManagerTester::testClass1TxNominal() {
    // Test configuration
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = dataPerPdu;  // Single PDU
    const char* srcFile = "test/ut/output/test_class1_tx.bin";
    const char* dstFile = "test/ut/output/test_class1_tx_dst.dat";

    // Create and verify test file
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    // Setup transaction and verify initial state
    TransactionSetup setup;
    setupTxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID,
                             Cfdp::Class::CLASS_1, TEST_PRIORITY, CF_TxnState_S1, setup);

    // Run first engine cycle - should send Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(2);

    // Verify Metadata PDU
    verifyMetadataPduAtIndex(0, setup, fileSize, srcFile, dstFile, Cfdp::Class::CLASS_1);

    // Verify FileData PDU
    Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(1);
    ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU should be sent";
    verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                     setup.expectedSeqNum, 0, static_cast<U16>(fileSize), srcFile, Cfdp::Class::CLASS_1);

    EXPECT_EQ(fileSize, setup.txn->m_foffs) << "Should have read entire file";
    EXPECT_EQ(CF_TxSubState_EOF, setup.txn->m_state_data.send.sub_state) << "Should progress to EOF sub-state";

    // Run second engine cycle - should send EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(3);

    // Verify EOF PDU
    Fw::Buffer eofPduBuffer = this->getSentPduBuffer(2);
    ASSERT_GT(eofPduBuffer.getSize(), 0) << "EOF PDU should be sent";
    verifyEofPdu(eofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                 setup.expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(fileSize), srcFile);

    // Wait for transaction recycle
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, setup.expectedSeqNum);
}

void CfdpManagerTester::testClass2TxNominal() {
    // Test configuration
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;
    const char* srcFile = "test/ut/output/test_class2_tx_5pdu.bin";
    const char* dstFile = "test/ut/output/test_class2_tx_dst.dat";

    // Create and verify test file
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    // Setup transaction and verify initial state
    TransactionSetup setup;
    setupTxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_1, TEST_GROUND_EID,
                             Cfdp::Class::CLASS_2, TEST_PRIORITY, CF_TxnState_S2, setup);

    // Run engine cycle and verify Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(6);

    verifyMetadataPduAtIndex(0, setup, expectedFileSize, srcFile, dstFile, Cfdp::Class::CLASS_2);
    verifyMultipleFileDataPdus(1, 5, setup, dataPerPdu, srcFile, Cfdp::Class::CLASS_2);

    EXPECT_EQ(expectedFileSize, setup.txn->m_foffs) << "Should have read entire file";
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.send.sub_state) << "Should be in CLOSEOUT_SYNC after file data complete";
    EXPECT_TRUE(setup.txn->m_flags.tx.send_eof) << "send_eof flag should be set";
    EXPECT_EQ(CF_TxnState_S2, setup.txn->m_state) << "Should remain in S2 state";

    // Run cycle and verify EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(7);

    Fw::Buffer eofPduBuffer = this->getSentPduBuffer(6);
    ASSERT_GT(eofPduBuffer.getSize(), 0) << "EOF PDU should be sent";
    verifyEofPdu(eofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                 setup.expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(expectedFileSize), srcFile);

    EXPECT_EQ(CF_TxnState_S2, setup.txn->m_state) << "Should remain in S2 state until EOF-ACK received";
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.send.sub_state) << "Should remain in CLOSEOUT_SYNC waiting for EOF-ACK";
    EXPECT_FALSE(setup.txn->m_flags.tx.send_eof) << "send_eof flag should be cleared after EOF sent";
    EXPECT_FALSE(setup.txn->m_flags.tx.eof_ack_recv) << "eof_ack_recv should be false before ACK received";

    // Complete Class 2 handshake
    completeClass2Handshake(TEST_CHANNEL_ID_1, TEST_GROUND_EID, setup.expectedSeqNum, setup.txn);
    ASSERT_EQ(8, this->fromPortHistory_dataOut->size()) << "Should have exactly 8 PDUs sent";
    verifyFinAckPdu(7, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum);

    // Wait for transaction recycle
    waitForTransactionRecycle(TEST_CHANNEL_ID_1, setup.expectedSeqNum);

    // Clean up test file
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::testClass2TxNack() {
    // Test configuration
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;
    const char* srcFile = "test/ut/output/test_c2_tx_nak.bin";
    const char* dstFile = "test/ut/output/test_c2_nak_dst.dat";

    // Create and verify test file
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    // Setup transaction and verify initial state
    TransactionSetup setup;
    setupTxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID,
                             Cfdp::Class::CLASS_2, TEST_PRIORITY, CF_TxnState_S2, setup);

    // Run engine cycle and verify Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(6);

    verifyMetadataPduAtIndex(0, setup, expectedFileSize, srcFile, dstFile, Cfdp::Class::CLASS_2);
    verifyMultipleFileDataPdus(1, 5, setup, dataPerPdu, srcFile, Cfdp::Class::CLASS_2);

    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.send.sub_state) << "Should be in CLOSEOUT_SYNC after file data complete";
    EXPECT_TRUE(setup.txn->m_flags.tx.send_eof) << "send_eof flag should be set";

    // Run cycle and verify first EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(7);

    Fw::Buffer firstEofPduBuffer = this->getSentPduBuffer(6);
    ASSERT_GT(firstEofPduBuffer.getSize(), 0) << "First EOF PDU should be sent";
    verifyEofPdu(firstEofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                 setup.expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(expectedFileSize), srcFile);

    // Clear history to make room for retransmitted PDUs
    this->clearHistory();
    this->m_pduCopyCount = 0;

    // Send NAK requesting retransmission of PDUs 2 and 5
    Cfdp::SegmentRequest segments[2];
    segments[0].offsetStart = dataPerPdu;
    segments[0].offsetEnd = 2 * dataPerPdu;
    segments[1].offsetStart = 4 * dataPerPdu;
    segments[1].offsetEnd = 5 * dataPerPdu;

    this->sendNakPdu(
        TEST_CHANNEL_ID_0,
        component.getLocalEidParam(),
        TEST_GROUND_EID,
        setup.expectedSeqNum,
        0,
        static_cast<CfdpFileSize>(expectedFileSize),
        2,
        segments
    );
    this->component.doDispatch();

    EXPECT_EQ(CF_TxnState_S2, setup.txn->m_state) << "Should remain in S2 state after NAK";
    EXPECT_EQ(CF_TxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.send.sub_state) << "Should remain in CLOSEOUT_SYNC after NAK";

    // Run cycles until second EOF and verify
    U32 maxCycles = 10;
    bool foundSecondEof = false;
    FwIndexType secondEofIndex = 0;

    for (U32 cycle = 0; cycle < maxCycles && !foundSecondEof; ++cycle) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();

        if (this->fromPortHistory_dataOut->size() > 0) {
            FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
            Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
            Cfdp::EofPdu eofPdu;
            Fw::SerialBuffer sb(const_cast<U8*>(lastPdu.getData()), lastPdu.getSize());
            sb.setBuffLen(lastPdu.getSize());
            if (eofPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                foundSecondEof = true;
                secondEofIndex = lastIndex;
            }
        }
    }

    ASSERT_TRUE(foundSecondEof) << "Second EOF PDU should be sent after chunk retransmission";
    Fw::Buffer secondEofPduBuffer = this->getSentPduBuffer(secondEofIndex);
    ASSERT_GT(secondEofPduBuffer.getSize(), 0) << "Second EOF PDU should be sent";
    verifyEofPdu(secondEofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID,
                 setup.expectedSeqNum, Cfdp::CONDITION_CODE_NO_ERROR, static_cast<CfdpFileSize>(expectedFileSize), srcFile);

    // Complete Class 2 handshake after NAK
    completeClass2Handshake(TEST_CHANNEL_ID_0, TEST_GROUND_EID, setup.expectedSeqNum, setup.txn);

    // Note: Can't verify exact PDU count since retransmissions vary, but verify FIN-ACK at last index
    FwIndexType finAckIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
    verifyFinAckPdu(finAckIndex, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum);

    // Wait for transaction recycle
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, setup.expectedSeqNum);

    // Clean up test file
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::testClass1RxNominal() {
    // Test configuration - use CF_MAX_FILE_DATA_SIZE for single PDU
    const U16 fileDataSize = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = fileDataSize;
    const char* srcFile = "test/ut/output/test_rx_source.bin";
    const char* dstFile = "test/ut/output/test_rx_received.bin";
    const char* groundSideSrcFile = "/ground/test_rx_source.bin";
    const U32 transactionSeq = 100;

    // Create test data file dynamically
    FwSizeType actualFileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, actualFileSize);

    // Uplink Metadata PDU and setup RX transaction
    TransactionSetup setup;
    setupRxTransaction(groundSideSrcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID,
                       Cfdp::Class::CLASS_1, static_cast<U32>(actualFileSize), transactionSeq, CF_TxnState_R1, setup);

    // Uplink FileData PDU
    // Read test data from source file
    U8* testData = new U8[actualFileSize];
    Os::File file;
    Os::File::Status fileStatus = file.open(srcFile, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open source file for reading";

    FwSizeType bytesRead = actualFileSize;
    fileStatus = file.read(testData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read source file";
    ASSERT_EQ(actualFileSize, bytesRead) << "Should read entire file";

    // Send FileData PDU
    sendFileDataPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,                    
        component.getLocalEidParam(),     
        transactionSeq,
        0,                                 // offset
        static_cast<U16>(actualFileSize),  // size
        testData,
        Cfdp::Class::CLASS_1
    );
    component.doDispatch();

    // Verify FileData processed
    EXPECT_EQ(CF_TxnState_R1, setup.txn->m_state) << "Should remain in R1 state after FileData";
    EXPECT_EQ(CF_RxSubState_FILEDATA, setup.txn->m_state_data.receive.sub_state) << "Should remain in FILEDATA sub-state";

    // Compute CRC for EOF PDU
    CFDP::Checksum crc;
    crc.update(testData, 0, static_cast<U32>(actualFileSize));
    U32 expectedCrc = crc.getValue();

    // Uplink EOF PDU
    sendEofPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,                    
        component.getLocalEidParam(),     
        transactionSeq,
        Cfdp::CONDITION_CODE_NO_ERROR,
        expectedCrc,
        static_cast<CfdpFileSize>(actualFileSize),
        Cfdp::Class::CLASS_1
    );
    component.doDispatch();

    // Verify transaction completed (moved to HOLD state)
    EXPECT_EQ(CF_TxnState_HOLD, setup.txn->m_state) << "Should be in HOLD state after EOF processing";

    // Verify file written to disk
    verifyReceivedFile(dstFile, testData, actualFileSize);

    // Clean up dynamically allocated buffer
    delete[] testData;

    // Wait for transaction recycle
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, transactionSeq);

    // Cleanup test files
    cleanupTestFile(dstFile);
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::testClass2RxNominal() {
    // Test configuration - use 5 PDUs
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;
    const char* srcFile = "test/ut/output/test_class2_rx_source.bin";
    const char* dstFile = "test/ut/output/test_class2_rx_received.bin";
    const char* groundSideSrcFile = "/ground/test_class2_rx_source.bin";
    const U32 transactionSeq = 200;

    // Create test data file dynamically
    FwSizeType actualFileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, actualFileSize);

    // Uplink Metadata PDU and setup RX transaction
    TransactionSetup setup;
    setupRxTransaction(groundSideSrcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID,
                       Cfdp::Class::CLASS_2, static_cast<U32>(actualFileSize), transactionSeq, CF_TxnState_R2, setup);

    // Read test data from source file
    U8* testData = new U8[actualFileSize];
    Os::File file;
    Os::File::Status fileStatus = file.open(srcFile, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open source file for reading";

    FwSizeType bytesRead = actualFileSize;
    fileStatus = file.read(testData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read source file";
    ASSERT_EQ(actualFileSize, bytesRead) << "Should read entire file";

    // Uplink 5 FileData PDUs
    for (U8 pduIdx = 0; pduIdx < 5; pduIdx++) {
        U32 offset = pduIdx * dataPerPdu;
        sendFileDataPdu(
            TEST_CHANNEL_ID_0,
            TEST_GROUND_EID,
            component.getLocalEidParam(),
            transactionSeq,
            offset,
            dataPerPdu,
            testData + offset,
            Cfdp::Class::CLASS_2
        );
        component.doDispatch();
    }

    // Verify FileData processed

    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state after FileData";
    EXPECT_EQ(CF_RxSubState_FILEDATA, setup.txn->m_state_data.receive.sub_state) << "Should remain in FILEDATA sub-state";

    // Compute CRC for EOF PDU
    CFDP::Checksum crc;
    crc.update(testData, 0, static_cast<U32>(actualFileSize));
    U32 expectedCrc = crc.getValue();

    // Remember how many PDUs have been sent so far
    FwSizeType pduCountBeforeEof = this->fromPortHistory_dataOut->size();

    // Uplink EOF PDU
    sendEofPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::CONDITION_CODE_NO_ERROR,
        expectedCrc,
        static_cast<CfdpFileSize>(actualFileSize),
        Cfdp::Class::CLASS_2
    );
    component.doDispatch();

    // Verify EOF processed
    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state after EOF";
    EXPECT_TRUE(setup.txn->m_flags.rx.eof_recv) << "eof_recv flag should be set after EOF received";
    EXPECT_TRUE(setup.txn->m_flags.rx.send_eof_ack) << "send_eof_ack flag should be set after EOF received";
    EXPECT_TRUE(setup.txn->m_flags.rx.send_fin) << "send_fin flag should be set after EOF received (file is complete)";

    // Run cycle to send EOF-ACK
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Verify EOF-ACK PDU sent by FSW
    FwSizeType pduCountAfterTick = this->fromPortHistory_dataOut->size();
    EXPECT_EQ(pduCountBeforeEof + 1, pduCountAfterTick) << "Should send exactly 1 PDU (EOF-ACK)";
    Fw::Buffer eofAckPduBuffer = this->getSentPduBuffer(static_cast<FwIndexType>(pduCountBeforeEof));
    ASSERT_GT(eofAckPduBuffer.getSize(), 0) << "EOF-ACK PDU should be sent by FSW";
    verifyAckPdu(eofAckPduBuffer,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::FILE_DIRECTIVE_END_OF_FILE,
        1,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::ACK_TXN_STATUS_ACTIVE
    );

    // Run cycles until FIN PDU is sent (CRC calculation may take multiple ticks)
    U32 maxCycles = 20;
    bool foundFin = false;
    FwIndexType finIndex = 0;

    for (U32 cycle = 0; cycle < maxCycles && !foundFin; ++cycle) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();

        if (this->fromPortHistory_dataOut->size() > 1) {
            FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
            Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
            Cfdp::FinPdu finPdu;
            Fw::SerialBuffer sb(const_cast<U8*>(lastPdu.getData()), lastPdu.getSize());
            sb.setBuffLen(lastPdu.getSize());
            if (finPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                foundFin = true;
                finIndex = lastIndex;
            }
        }
    }

    // Verify FIN PDU was sent
    ASSERT_TRUE(foundFin) << "FIN PDU should be sent after CRC calculation completes";

    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state until FIN-ACK received";
    EXPECT_EQ(CF_RxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.receive.sub_state) << "Should be in CLOSEOUT_SYNC waiting for FIN-ACK";

    Fw::Buffer finPduBuffer = this->getSentPduBuffer(finIndex);
    verifyFinPdu(finPduBuffer,
        TEST_GROUND_EID,                 
        component.getLocalEidParam(),  
        transactionSeq,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::FIN_DELIVERY_CODE_COMPLETE,
        Cfdp::FIN_FILE_STATUS_RETAINED
    );

    // Send FIN-ACK from ground to FSW
    this->sendAckPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,                      
        component.getLocalEidParam(),       
        transactionSeq,
        Cfdp::FILE_DIRECTIVE_FIN,
        1,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::ACK_TXN_STATUS_TERMINATED
    );
    this->component.doDispatch();

    // Verify transaction completed (moved to HOLD state)
    EXPECT_EQ(CF_TxnState_HOLD, setup.txn->m_state) << "Should be in HOLD state after FIN-ACK received";

    // Wait for transaction recycle (this closes the file descriptor)
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, transactionSeq);

    // Verify file written to disk (after transaction is recycled and file is closed)
    verifyReceivedFile(dstFile, testData, actualFileSize);

    // Clean up dynamically allocated buffer
    delete[] testData;

    // Cleanup test files
    cleanupTestFile(dstFile);
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::testClass2RxNack() {
    // Test configuration - use 5 PDUs, but send only 0 and 3 initially (skip 1, 2, 4)
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;
    const char* srcFile = "test/ut/output/test_class2_rx_nack_source.bin";
    const char* dstFile = "test/ut/output/test_class2_rx_nack_received.bin";
    const char* groundSideSrcFile = "/ground/test_class2_rx_nack_source.bin";
    const U32 transactionSeq = 300;

    // Create test data file dynamically
    FwSizeType actualFileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, actualFileSize);

    // Uplink Metadata PDU and setup RX transaction
    TransactionSetup setup;
    setupRxTransaction(groundSideSrcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID,
                       Cfdp::Class::CLASS_2, static_cast<U32>(actualFileSize), transactionSeq, CF_TxnState_R2, setup);

    // Read test data from source file
    U8* testData = new U8[actualFileSize];
    Os::File file;
    Os::File::Status fileStatus = file.open(srcFile, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to open source file for reading";

    FwSizeType bytesRead = actualFileSize;
    fileStatus = file.read(testData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus) << "Failed to read source file";
    ASSERT_EQ(actualFileSize, bytesRead) << "Should read entire file";

    // Uplink FileData PDUs 0 and 3 only (skip 1, 2, 4 to create gaps)
    U8 pduIndices[] = {0, 3};
    for (U8 i = 0; i < 2; i++) {
        U8 pduIdx = pduIndices[i];
        U32 offset = pduIdx * dataPerPdu;
        sendFileDataPdu(
            TEST_CHANNEL_ID_0,
            TEST_GROUND_EID,
            component.getLocalEidParam(),
            transactionSeq,
            offset,
            dataPerPdu,
            testData + offset,
            Cfdp::Class::CLASS_2
        );
        component.doDispatch();
    }

    // Verify FileData processed
    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state after FileData";
    EXPECT_EQ(CF_RxSubState_FILEDATA, setup.txn->m_state_data.receive.sub_state) << "Should remain in FILEDATA sub-state";

    // Compute CRC for EOF PDU
    CFDP::Checksum crc;
    crc.update(testData, 0, static_cast<U32>(actualFileSize));
    U32 expectedCrc = crc.getValue();

    // Remember how many PDUs have been sent so far
    FwSizeType pduCountBeforeEof = this->fromPortHistory_dataOut->size();

    // Uplink EOF PDU
    sendEofPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::CONDITION_CODE_NO_ERROR,
        expectedCrc,
        static_cast<CfdpFileSize>(actualFileSize),
        Cfdp::Class::CLASS_2
    );
    component.doDispatch();

    // Verify EOF processed
    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state after EOF";
    EXPECT_TRUE(setup.txn->m_flags.rx.eof_recv) << "eof_recv flag should be set after EOF received";
    EXPECT_TRUE(setup.txn->m_flags.rx.send_eof_ack) << "send_eof_ack flag should be set after EOF received";
    EXPECT_FALSE(setup.txn->m_flags.rx.send_fin) << "send_fin flag should NOT be set (file has gaps)";
    EXPECT_TRUE(setup.txn->m_flags.rx.send_nak) << "send_nak flag should be set (missing segments)";

    // Run cycle to send EOF-ACK and NAK
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Verify EOF-ACK PDU sent by FSW
    FwSizeType pduCountAfterTick = this->fromPortHistory_dataOut->size();
    EXPECT_EQ(pduCountBeforeEof + 1, pduCountAfterTick) << "Should send exactly 1 PDU (EOF-ACK)";
    Fw::Buffer eofAckPduBuffer = this->getSentPduBuffer(static_cast<FwIndexType>(pduCountBeforeEof));
    ASSERT_GT(eofAckPduBuffer.getSize(), 0) << "EOF-ACK PDU should be sent by FSW";
    verifyAckPdu(eofAckPduBuffer,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::FILE_DIRECTIVE_END_OF_FILE,
        1,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::ACK_TXN_STATUS_ACTIVE
    );

    // Run cycles until NAK PDU is sent
    U32 maxCycles = 20;
    bool foundNak = false;
    FwIndexType nakIndex = 0;

    for (U32 cycle = 0; cycle < maxCycles && !foundNak; ++cycle) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();

        if (this->fromPortHistory_dataOut->size() > pduCountAfterTick) {
            FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
            Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
            Cfdp::NakPdu nakPdu;
            Fw::SerialBuffer sb(const_cast<U8*>(lastPdu.getData()), lastPdu.getSize());
            sb.setBuffLen(lastPdu.getSize());
            if (nakPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                foundNak = true;
                nakIndex = lastIndex;
            }
        }
    }

    // Verify NAK PDU was sent
    ASSERT_TRUE(foundNak) << "NAK PDU should be sent requesting missing segments";

    Fw::Buffer nakPduBuffer = this->getSentPduBuffer(nakIndex);

    // Verify NAK PDU requests missing segments 1, 2, and 4
    Cfdp::SegmentRequest expectedSegments[3];
    expectedSegments[0].offsetStart = 1 * dataPerPdu;
    expectedSegments[0].offsetEnd = 3 * dataPerPdu;  // Covers PDUs 1 and 2
    expectedSegments[1].offsetStart = 4 * dataPerPdu;
    expectedSegments[1].offsetEnd = 5 * dataPerPdu;  // Covers PDU 4

    verifyNakPdu(nakPduBuffer,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        0,
        0,  // scope_end is not set by the CFDP engine
        2,
        expectedSegments
    );

    // Clear history to make room for retransmission verification
    FwSizeType pduCountBeforeRetransmit = this->fromPortHistory_dataOut->size();

    // Uplink missing FileData PDUs 1, 2, and 4
    U8 missingPduIndices[] = {1, 2, 4};
    for (U8 i = 0; i < 3; i++) {
        U8 pduIdx = missingPduIndices[i];
        U32 offset = pduIdx * dataPerPdu;
        sendFileDataPdu(
            TEST_CHANNEL_ID_0,
            TEST_GROUND_EID,
            component.getLocalEidParam(),
            transactionSeq,
            offset,
            dataPerPdu,
            testData + offset,
            Cfdp::Class::CLASS_2
        );
        component.doDispatch();
    }

    // Verify transaction now sees file as complete
    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state after gap fill";
    EXPECT_TRUE(setup.txn->m_flags.rx.complete) << "complete flag should be set after gaps filled";

    // Run cycles until FIN PDU is sent (CRC calculation may take multiple ticks)
    bool foundFin = false;
    FwIndexType finIndex = 0;

    for (U32 cycle = 0; cycle < maxCycles && !foundFin; ++cycle) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();

        if (this->fromPortHistory_dataOut->size() > pduCountBeforeRetransmit) {
            FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
            Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
            Cfdp::FinPdu finPdu;
            Fw::SerialBuffer sb(const_cast<U8*>(lastPdu.getData()), lastPdu.getSize());
            sb.setBuffLen(lastPdu.getSize());
            if (finPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                foundFin = true;
                finIndex = lastIndex;
            }
        }
    }

    // Verify FIN PDU was sent
    ASSERT_TRUE(foundFin) << "FIN PDU should be sent after gaps filled and CRC calculated";

    EXPECT_EQ(CF_TxnState_R2, setup.txn->m_state) << "Should remain in R2 state until FIN-ACK received";
    EXPECT_EQ(CF_RxSubState_CLOSEOUT_SYNC, setup.txn->m_state_data.receive.sub_state) << "Should be in CLOSEOUT_SYNC waiting for FIN-ACK";

    Fw::Buffer finPduBuffer = this->getSentPduBuffer(finIndex);
    verifyFinPdu(finPduBuffer,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::FIN_DELIVERY_CODE_COMPLETE,
        Cfdp::FIN_FILE_STATUS_RETAINED
    );

    // Send FIN-ACK from ground to FSW
    this->sendAckPdu(
        TEST_CHANNEL_ID_0,
        TEST_GROUND_EID,
        component.getLocalEidParam(),
        transactionSeq,
        Cfdp::FILE_DIRECTIVE_FIN,
        1,
        Cfdp::CONDITION_CODE_NO_ERROR,
        Cfdp::ACK_TXN_STATUS_TERMINATED
    );
    this->component.doDispatch();

    // Verify transaction completed (moved to HOLD state)
    EXPECT_EQ(CF_TxnState_HOLD, setup.txn->m_state) << "Should be in HOLD state after FIN-ACK received";

    // Wait for transaction recycle (this closes the file descriptor)
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, transactionSeq);

    // Verify file written to disk (after transaction is recycled and file is closed)
    verifyReceivedFile(dstFile, testData, actualFileSize);

    // Clean up dynamically allocated buffer
    delete[] testData;

    // Cleanup test files
    cleanupTestFile(dstFile);
    cleanupTestFile(srcFile);
}

}  // namespace Ccsds
}  // namespace Svc
