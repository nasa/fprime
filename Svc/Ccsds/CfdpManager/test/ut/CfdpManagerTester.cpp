// ======================================================================
// \title  CfdpManagerTester.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Svc/Ccsds/CfdpManager/Clist.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Static member definitions
// ----------------------------------------------------------------------

constexpr FwSizeType CfdpManagerTester::MAX_PDU_COPIES;

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpManagerTester ::CfdpManagerTester()
    : CfdpManagerGTestBase("CfdpManagerTester", MAX_HISTORY_SIZE),
      component("CfdpManager"),
      m_pduCopyCount(0),
      m_failBufferAllocation(false) {
    this->connectPorts();
    this->initComponents();
    this->component.loadParameters();

    // Configure CFDP engine after parameters are loaded
    this->component.configure(this->m_allocator, 10);
}

CfdpManagerTester ::~CfdpManagerTester() {
    this->component.cleanup();
}

// ----------------------------------------------------------------------
// Handler implementations for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer CfdpManagerTester::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    // Simulate allocation failure if flag is set
    if (this->m_failBufferAllocation) {
        return Fw::Buffer();
    }

    EXPECT_LT(size, MaxPduSize) << "Buffer size request is too large";
    if (size >= MaxPduSize) {
        return Fw::Buffer();
    }
    return Fw::Buffer(this->m_internalDataBuffer, size);
}

void CfdpManagerTester::from_dataOut_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Make a copy of the PDU data to avoid buffer reuse issues
    EXPECT_LT(m_pduCopyCount, MAX_PDU_COPIES) << "Too many PDUs sent";
    if (m_pduCopyCount < MAX_PDU_COPIES) {
        FwSizeType copySize = fwBuffer.getSize();
        if (copySize > MaxPduSize) {
            copySize = MaxPduSize;
        }
        memcpy(m_pduCopyStorage[m_pduCopyCount], fwBuffer.getData(), copySize);

        // Create a new buffer pointing to our copy
        Fw::Buffer copyBuffer(m_pduCopyStorage[m_pduCopyCount], copySize);
        m_pduCopyCount++;

        // Call base class handler with the copy
        CfdpManagerTesterBase::from_dataOut_handler(portNum, copyBuffer);
    }
}

void CfdpManagerTester::from_fileDoneOut_handler(FwIndexType portNum, const Svc::SendFileResponse& response) {
    // Push to port history
    CfdpManagerGTestBase::from_fileDoneOut_handler(portNum, response);
}

// ----------------------------------------------------------------------
// Transaction Test Helper Implementations
// ----------------------------------------------------------------------

Transaction* CfdpManagerTester::findTransaction(U8 chanNum, TransactionSeq seqNum) {
    // Grab requested channel
    Channel* chan = component.m_engine->m_channels[chanNum];

    // Search through all transaction queues (PEND, TXA, TXW, RX, FREE)
    // Skip HIST and HIST_FREE as they contain History, not Transaction
    for (U8 qIdx = 0; qIdx < Cfdp::QueueId::NUM; qIdx++) {
        // Skip history queues (HIST=4, HIST_FREE=5)
        if (qIdx == Cfdp::QueueId::HIST || qIdx == Cfdp::QueueId::HIST_FREE) {
            continue;
        }

        CListNode* head = chan->m_qs[qIdx];
        if (head == nullptr) {
            continue;
        }

        // Traverse circular linked list, stopping when we loop back to head
        CListNode* node = head;
        do {
            Transaction* txn = container_of_cpp(node, &Transaction::m_cl_node);
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

void CfdpManagerTester::createAndVerifyTestFile(const char* filePath,
                                                FwSizeType expectedFileSize,
                                                FwSizeType& actualFileSize) {
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

void CfdpManagerTester::setupTxTransaction(const char* srcFile,
                                           const char* dstFile,
                                           U8 channelId,
                                           EntityId destEid,
                                           Cfdp::Class cfdpClass,
                                           U8 priority,
                                           TxnState expectedState,
                                           TransactionSetup& setup) {
    const U32 initialSeqNum = component.m_engine->m_seqNum;

    this->sendCmd_SendFile(0, 0, channelId, destEid, cfdpClass, Cfdp::Keep::KEEP, priority, Fw::CmdStringArg(srcFile),
                           Fw::CmdStringArg(dstFile));
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
    EXPECT_EQ(TxSubState::TX_SUB_STATE_METADATA, setup.txn->m_state_data.send.sub_state)
        << "Should start in METADATA sub-state";
    EXPECT_EQ(channelId, setup.txn->m_chan_num) << "Channel number should match";
    EXPECT_EQ(priority, setup.txn->m_priority) << "Priority should match";

    EXPECT_EQ(setup.expectedSeqNum, setup.txn->m_history->seq_num) << "History seq_num should match";
    EXPECT_EQ(component.getLocalEidParam(), setup.txn->m_history->src_eid) << "Source EID should match local EID";
    EXPECT_EQ(destEid, setup.txn->m_history->peer_eid) << "Peer EID should match dest EID";
    EXPECT_STREQ(srcFile, setup.txn->m_history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, setup.txn->m_history->fnames.dst_filename.toChar()) << "Destination filename should match";
}

void CfdpManagerTester::setupTxPortTransaction(const char* srcFile,
                                               const char* dstFile,
                                               U8 channelId,
                                               TxnState expectedState,
                                               TransactionSetup& setup) {
    // Capture current sequence number before initiating
    const U32 initialSeqNum = component.m_engine->m_seqNum;

    // Initiate via port. The guarded port only enqueues the request; the transaction is not
    // created until the active thread drains the queue, which we trigger explicitly here.
    Svc::SendFileResponse response = invokeSendFilePort(srcFile, dstFile);

    ASSERT_EQ(Svc::SendFileStatus::STATUS_OK, response.get_status()) << "Port-based file send should be accepted";

    this->component.drainFileInQueue();

    // Find the transaction that was created
    setup.expectedSeqNum = initialSeqNum + 1;
    EXPECT_EQ(setup.expectedSeqNum, component.m_engine->m_seqNum) << "Sequence number should increment";

    setup.txn = findTransaction(channelId, setup.expectedSeqNum);
    ASSERT_NE(nullptr, setup.txn) << "Transaction should exist after port invocation";

    // Verify initial transaction state
    ASSERT_EQ(expectedState, setup.txn->m_state) << "Should be in expected state";
    ASSERT_EQ(TransactionInitType::INIT_BY_PORT, setup.txn->m_initType) << "Should be marked as port-initiated";
    EXPECT_EQ(0, setup.txn->m_foffs) << "File offset should be 0 initially";
    EXPECT_EQ(TxSubState::TX_SUB_STATE_METADATA, setup.txn->m_state_data.send.sub_state)
        << "Should start in METADATA sub-state";
    EXPECT_EQ(channelId, setup.txn->m_chan_num) << "Channel number should match";

    // Verify transaction history
    EXPECT_EQ(setup.expectedSeqNum, setup.txn->m_history->seq_num) << "History seq_num should match";
    EXPECT_EQ(component.getLocalEidParam(), setup.txn->m_history->src_eid) << "Source EID should match local EID";
    EXPECT_STREQ(srcFile, setup.txn->m_history->fnames.src_filename.toChar()) << "Source filename should match";
    EXPECT_STREQ(dstFile, setup.txn->m_history->fnames.dst_filename.toChar()) << "Destination filename should match";
}

void CfdpManagerTester::setupRxTransaction(const char* srcFile,
                                           const char* dstFile,
                                           U8 channelId,
                                           EntityId sourceEid,
                                           Cfdp::Class::T cfdpClass,
                                           U32 fileSize,
                                           U32 transactionSeq,
                                           TxnState expectedState,
                                           TransactionSetup& setup) {
    // Send Metadata PDU to initiate RX transaction
    U8 closureRequested = (cfdpClass == Cfdp::Class::CLASS_1) ? 0 : 1;

    this->sendMetadataPdu(channelId, sourceEid, component.getLocalEidParam(), transactionSeq, fileSize, srcFile,
                          dstFile, cfdpClass, closureRequested);
    this->component.doDispatch();

    // Find the created transaction
    setup.expectedSeqNum = transactionSeq;
    setup.txn = findTransaction(channelId, transactionSeq);
    ASSERT_NE(nullptr, setup.txn) << "RX transaction should be created after Metadata PDU";

    // Verify transaction state
    EXPECT_EQ(expectedState, setup.txn->m_state) << "Should be in expected RX state";
    EXPECT_EQ(RxSubState::RX_SUB_STATE_FILEDATA, setup.txn->m_state_data.receive.sub_state)
        << "Should start in FILEDATA sub-state";
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

    Transaction* txn = findTransaction(channelId, expectedSeqNum);
    EXPECT_EQ(nullptr, txn) << "Transaction should be recycled after inactivity timeout";
}

void CfdpManagerTester::completeClass2Handshake(U8 channelId, EntityId destEid, U32 expectedSeqNum, Transaction* txn) {
    // Send EOF-ACK
    this->sendAckPdu(channelId, component.getLocalEidParam(), destEid, expectedSeqNum,
                     Cfdp::FileDirective::FILE_DIRECTIVE_END_OF_FILE, 0, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                     Cfdp::AckTxnStatus::ACK_TXN_STATUS_ACTIVE);
    this->component.doDispatch();

    EXPECT_TRUE(txn->m_flags.tx.eof_ack_recv) << "eof_ack_recv flag should be set after EOF-ACK received";
    EXPECT_FALSE(txn->m_flags.com.ack_timer_armed) << "ack_timer_armed should be cleared after EOF-ACK";
    EXPECT_EQ(TxnState::TXN_STATE_S2, txn->m_state) << "Should remain in S2 state waiting for FIN";
    EXPECT_EQ(TxSubState::TX_SUB_STATE_CLOSEOUT_SYNC, txn->m_state_data.send.sub_state)
        << "Should remain in CLOSEOUT_SYNC waiting for FIN";

    // Send FIN
    this->sendFinPdu(channelId, component.getLocalEidParam(), destEid, expectedSeqNum,
                     Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, Cfdp::FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE,
                     Cfdp::FinFileStatus::FIN_FILE_STATUS_RETAINED);
    this->component.doDispatch();

    EXPECT_TRUE(txn->m_flags.tx.fin_recv) << "fin_recv flag should be set after FIN received";
    EXPECT_EQ(TxnState::TXN_STATE_HOLD, txn->m_state) << "Should move to HOLD state after FIN received";
    EXPECT_TRUE(txn->m_flags.tx.send_fin_ack) << "send_fin_ack flag should be set";

    // Run cycle to send FIN-ACK
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
}

void CfdpManagerTester::verifyFinAckPdu(FwIndexType pduIndex,
                                        EntityId sourceEid,
                                        EntityId destEid,
                                        U32 expectedSeqNum) {
    Fw::Buffer finAckPduBuffer = this->getSentPduBuffer(pduIndex);
    ASSERT_GT(finAckPduBuffer.getSize(), 0) << "FIN-ACK PDU should be sent";

    verifyAckPdu(finAckPduBuffer, sourceEid, destEid, expectedSeqNum, Cfdp::FileDirective::FILE_DIRECTIVE_FIN, 1,
                 Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, Cfdp::AckTxnStatus::ACK_TXN_STATUS_TERMINATED);
}

void CfdpManagerTester::verifyMetadataPduAtIndex(FwIndexType pduIndex,
                                                 const TransactionSetup& setup,
                                                 FwSizeType fileSize,
                                                 const char* srcFile,
                                                 const char* dstFile,
                                                 Cfdp::Class::T cfdpClass) {
    Fw::Buffer metadataPduBuffer = this->getSentPduBuffer(pduIndex);
    ASSERT_GT(metadataPduBuffer.getSize(), 0) << "Metadata PDU should be sent";
    EXPECT_EQ(fileSize, setup.txn->m_fsize) << "File size should be set after file is opened";
    verifyMetadataPdu(metadataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                      static_cast<FileSize>(fileSize), srcFile, dstFile, cfdpClass);
}

void CfdpManagerTester::verifyMultipleFileDataPdus(FwIndexType startIndex,
                                                   U8 numPdus,
                                                   const TransactionSetup& setup,
                                                   U16 dataPerPdu,
                                                   const char* srcFile,
                                                   Cfdp::Class::T cfdpClass) {
    for (U8 pduIdx = 0; pduIdx < numPdus; pduIdx++) {
        Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(static_cast<FwIndexType>(startIndex + pduIdx));
        ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU " << static_cast<int>(pduIdx) << " should be sent";
        verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                          pduIdx * dataPerPdu, dataPerPdu, srcFile, cfdpClass);
    }
}

void CfdpManagerTester::cleanupTestFile(const char* filePath) {
    Os::FileSystem::Status fsStatus = Os::FileSystem::removeFile(filePath);
    // File may already be deleted by CFDP (keep=DELETE), which is acceptable
    EXPECT_TRUE(fsStatus == Os::FileSystem::OP_OK || fsStatus == Os::FileSystem::DOESNT_EXIST)
        << "Should remove test file or file already deleted";
}

void CfdpManagerTester::setFailBufferAllocation(bool fail) {
    this->m_failBufferAllocation = fail;
}

void CfdpManagerTester::verifyReceivedFile(const char* filePath, const U8* expectedData, FwSizeType expectedSize) {
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
// Refactored Test Helper Implementations
// ----------------------------------------------------------------------

Svc::SendFileResponse CfdpManagerTester::invokeSendFilePort(const char* srcFile, const char* dstFile) {
    Fw::String source(srcFile);
    Fw::String dest(dstFile);
    Svc::SendFileResponse response = this->invoke_to_fileIn(0,       // portNum
                                                            source,  // sourceFileName
                                                            dest,    // destFileName
                                                            0,       // offset (unused)
                                                            0        // length (0 = entire file)
    );
    return response;
}

void CfdpManagerTester::sendAndVerifyClass1Tx(const char* srcFile, const char* dstFile, FwSizeType expectedFileSize) {
    // Create and verify test file
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    // Setup transaction and verify initial state (command-based only)
    TransactionSetup setup;
    setupTxTransaction(srcFile, dstFile, TEST_CHANNEL_ID_0, TEST_GROUND_EID, Cfdp::Class::CLASS_1, TEST_PRIORITY,
                       TxnState::TXN_STATE_S1, setup);

    // Run first engine cycle - should send Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(2);

    // Verify Metadata PDU
    verifyMetadataPduAtIndex(0, setup, fileSize, srcFile, dstFile, Cfdp::Class::CLASS_1);

    // Verify FileData PDU
    Fw::Buffer fileDataPduBuffer = this->getSentPduBuffer(1);
    ASSERT_GT(fileDataPduBuffer.getSize(), 0) << "File data PDU should be sent";
    verifyFileDataPdu(fileDataPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum, 0,
                      static_cast<U16>(fileSize), srcFile, Cfdp::Class::CLASS_1);

    EXPECT_EQ(fileSize, setup.txn->m_foffs) << "Should have read entire file";
    EXPECT_EQ(TxSubState::TX_SUB_STATE_EOF, setup.txn->m_state_data.send.sub_state)
        << "Should progress to EOF sub-state";

    // Run second engine cycle - should send EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(3);

    // Verify EOF PDU
    Fw::Buffer eofPduBuffer = this->getSentPduBuffer(2);
    ASSERT_GT(eofPduBuffer.getSize(), 0) << "EOF PDU should be sent";
    verifyEofPdu(eofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                 Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, static_cast<FileSize>(fileSize), srcFile);

    // Verify telemetry was emitted (should be emitted at end of each run1Hz)
    // We called run1Hz twice, so expect at least 2 telemetry emissions
    ASSERT_GE(this->tlmHistory_ChannelTelemetry->size(), 1u);

    // Get the LATEST telemetry value (last emission has cumulative counts)
    U32 tlmIndex = static_cast<U32>(this->tlmHistory_ChannelTelemetry->size() - 1);
    Cfdp::ChannelTelemetryArray tlm = this->tlmHistory_ChannelTelemetry->at(tlmIndex).arg;

    // Verify TX counters incremented (we sent Metadata + FileData + EOF PDUs = 3 total)
    EXPECT_EQ(3u, tlm[TEST_CHANNEL_ID_0].get_sentPdu()) << "sentPdu should be 3 (Metadata + FileData + EOF)";
    EXPECT_GT(tlm[TEST_CHANNEL_ID_0].get_sentFileDataBytes(), 0u)
        << "sentFileDataBytes should increment when file data is sent";
    EXPECT_EQ(fileSize, tlm[TEST_CHANNEL_ID_0].get_sentFileDataBytes()) << "sentFileDataBytes should equal file size";

    // Verify no receive counters incremented (this is TX only)
    EXPECT_EQ(0u, tlm[TEST_CHANNEL_ID_0].get_recvPdu()) << "recvPdu should be 0 for TX-only transaction";
    EXPECT_EQ(0u, tlm[TEST_CHANNEL_ID_0].get_recvFileDataBytes())
        << "recvFileDataBytes should be 0 for TX-only transaction";

    // Verify no errors occurred
    EXPECT_EQ(0u, tlm[TEST_CHANNEL_ID_0].get_recvErrors()) << "No receive errors should occur";
    EXPECT_EQ(0u, tlm[TEST_CHANNEL_ID_0].get_faultAckLimit()) << "No ACK limit faults should occur";
    EXPECT_EQ(0u, tlm[TEST_CHANNEL_ID_0].get_faultNakLimit()) << "No NAK limit faults should occur";

    // Verify completion event was emitted
    ASSERT_EVENTS_TxFileTransferCompleted_SIZE(1);
    ASSERT_EVENTS_TxFileTransferCompleted(0,  // index
                                          Cfdp::Class::CLASS_1, setup.expectedSeqNum, component.getLocalEidParam(),
                                          srcFile, TEST_GROUND_EID, dstFile, static_cast<U32>(fileSize));

    // Wait for transaction recycle
    waitForTransactionRecycle(TEST_CHANNEL_ID_0, setup.expectedSeqNum);
}

void CfdpManagerTester::sendAndVerifyClass1Rx(const char* srcFile,
                                              const char* dstFile,
                                              const char* groundSrcFile,
                                              FwSizeType expectedFileSize,
                                              U8 channelId) {
    const U32 transactionSeq = 100;

    // Create test data file dynamically
    FwSizeType actualFileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, actualFileSize);

    // Uplink Metadata PDU and setup RX transaction
    TransactionSetup setup;
    setupRxTransaction(groundSrcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_1,
                       static_cast<U32>(actualFileSize), transactionSeq, TxnState::TXN_STATE_R1, setup);

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
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
                    0,                                 // offset
                    static_cast<U16>(actualFileSize),  // size
                    testData, Cfdp::Class::CLASS_1);
    component.doDispatch();

    // Verify FileData processed
    EXPECT_EQ(TxnState::TXN_STATE_R1, setup.txn->m_state) << "Should remain in R1 state after FileData";
    EXPECT_EQ(RxSubState::RX_SUB_STATE_FILEDATA, setup.txn->m_state_data.receive.sub_state)
        << "Should remain in FILEDATA sub-state";

    // Compute CRC for EOF PDU
    CFDP::Checksum crc;
    crc.update(testData, 0, static_cast<U32>(actualFileSize));
    U32 expectedCrc = crc.getValue();

    // Send EOF PDU
    sendEofPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
               Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, expectedCrc, static_cast<FileSize>(actualFileSize),
               Cfdp::Class::CLASS_1);
    component.doDispatch();

    // Verify transaction completed
    EXPECT_EQ(TxnState::TXN_STATE_HOLD, setup.txn->m_state) << "Should be in HOLD state after EOF processing";

    // Verify file written to disk
    verifyReceivedFile(dstFile, testData, actualFileSize);

    // Emit telemetry by calling run1Hz (RX tests don't automatically call this)
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Verify telemetry for RX transaction
    ASSERT_GE(this->tlmHistory_ChannelTelemetry->size(), 1u);
    U32 tlmIndex = static_cast<U32>(this->tlmHistory_ChannelTelemetry->size() - 1);
    Cfdp::ChannelTelemetryArray tlm = this->tlmHistory_ChannelTelemetry->at(tlmIndex).arg;

    // Verify RX counters (received Metadata + FileData + EOF = exactly 3 PDUs)
    EXPECT_EQ(3u, tlm[channelId].get_recvPdu()) << "recvPdu should be exactly 3 (Metadata + FileData + EOF)";
    EXPECT_EQ(actualFileSize, tlm[channelId].get_recvFileDataBytes()) << "recvFileDataBytes should match file size";

    // Class1 RX doesn't send responses, but sentPdu may have values from previous transactions
    // So we just log the values without strict assertions for Class1 RX

    // Verify no errors occurred
    EXPECT_EQ(0u, tlm[channelId].get_recvErrors()) << "No receive errors should occur";

    // Verify completion event was emitted
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    ASSERT_EVENTS_RxFileTransferCompleted(0,  // index
                                          Cfdp::Class::CLASS_1, transactionSeq, TEST_GROUND_EID, groundSrcFile,
                                          component.getLocalEidParam(), dstFile, static_cast<U32>(actualFileSize));

    // Clean up
    delete[] testData;
    waitForTransactionRecycle(channelId, transactionSeq);
    cleanupTestFile(dstFile);
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::sendAndVerifyClass2Rx(const char* srcFile,
                                              const char* dstFile,
                                              const char* groundSrcFile,
                                              FwSizeType expectedFileSize,
                                              bool simulateNak,
                                              bool expectExactCounts,
                                              U8 channelId) {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const U32 transactionSeq = simulateNak ? 300 : 200;

    // Create test data file
    FwSizeType actualFileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, actualFileSize);

    // Setup RX transaction
    TransactionSetup setup;
    setupRxTransaction(groundSrcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_2,
                       static_cast<U32>(actualFileSize), transactionSeq, TxnState::TXN_STATE_R2, setup);

    // Read test data
    U8* testData = new U8[actualFileSize];
    Os::File file;
    Os::File::Status fileStatus = file.open(srcFile, Os::File::OPEN_READ, Os::File::NO_OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);

    FwSizeType bytesRead = actualFileSize;
    fileStatus = file.read(testData, bytesRead, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    ASSERT_EQ(actualFileSize, bytesRead);

    // Send FileData PDUs
    if (simulateNak) {
        // Send only PDUs 0 and 3 (skip 1, 2, 4 to create gaps)
        U8 pduIndices[] = {0, 3};
        for (U8 i = 0; i < 2; i++) {
            U8 pduIdx = pduIndices[i];
            U32 offset = pduIdx * dataPerPdu;
            sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, offset,
                            dataPerPdu, testData + offset, Cfdp::Class::CLASS_2);
            component.doDispatch();
        }
    } else {
        // Send all PDUs
        U8 numPdus = static_cast<U8>(actualFileSize / dataPerPdu);
        for (U8 pduIdx = 0; pduIdx < numPdus; pduIdx++) {
            U32 offset = pduIdx * dataPerPdu;
            sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, offset,
                            dataPerPdu, testData + offset, Cfdp::Class::CLASS_2);
            component.doDispatch();
        }
    }

    // Verify FileData processed
    EXPECT_EQ(TxnState::TXN_STATE_R2, setup.txn->m_state);
    EXPECT_EQ(RxSubState::RX_SUB_STATE_FILEDATA, setup.txn->m_state_data.receive.sub_state);

    // Compute CRC and send EOF
    CFDP::Checksum crc;
    crc.update(testData, 0, static_cast<U32>(actualFileSize));
    U32 expectedCrc = crc.getValue();

    FwSizeType pduCountBeforeEof = this->fromPortHistory_dataOut->size();

    sendEofPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
               Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, expectedCrc, static_cast<FileSize>(actualFileSize),
               Cfdp::Class::CLASS_2);
    component.doDispatch();

    // Verify EOF processed
    EXPECT_EQ(TxnState::TXN_STATE_R2, setup.txn->m_state);
    EXPECT_TRUE(setup.txn->m_flags.rx.eof_recv);
    EXPECT_TRUE(setup.txn->m_flags.rx.send_eof_ack);

    if (simulateNak) {
        EXPECT_FALSE(setup.txn->m_flags.rx.send_fin);
        EXPECT_TRUE(setup.txn->m_flags.rx.send_nak);
    } else {
        EXPECT_TRUE(setup.txn->m_flags.rx.send_fin);
    }

    // Run cycle to send EOF-ACK
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Verify EOF-ACK sent
    FwSizeType pduCountAfterTick = this->fromPortHistory_dataOut->size();
    EXPECT_EQ(pduCountBeforeEof + 1, pduCountAfterTick);
    Fw::Buffer eofAckPduBuffer = this->getSentPduBuffer(static_cast<FwIndexType>(pduCountBeforeEof));
    ASSERT_GT(eofAckPduBuffer.getSize(), 0);
    verifyAckPdu(eofAckPduBuffer, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
                 Cfdp::FileDirective::FILE_DIRECTIVE_END_OF_FILE, 1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                 Cfdp::AckTxnStatus::ACK_TXN_STATUS_ACTIVE);

    // Handle NAK if simulated
    if (simulateNak) {
        // Wait for NAK
        U32 maxCycles = 20;
        bool foundNak = false;

        for (U32 cycle = 0; cycle < maxCycles && !foundNak; ++cycle) {
            this->invoke_to_run1Hz(0, 0);
            this->component.doDispatch();

            if (this->fromPortHistory_dataOut->size() > pduCountAfterTick) {
                FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
                Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
                Cfdp::NakPdu nakPdu;
                const U8* pduData;
                FwSizeType pduSize;
                if (this->getPduData(lastPdu, pduData, pduSize)) {
                    Fw::SerialBuffer sb(const_cast<U8*>(pduData), pduSize);
                    sb.setBuffLen(pduSize);
                    if (nakPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                        foundNak = true;
                    }
                }
            }
        }

        ASSERT_TRUE(foundNak);

        // Send missing PDUs 1, 2, and 4
        FwSizeType pduCountBeforeRetransmit = this->fromPortHistory_dataOut->size();
        U8 missingPduIndices[] = {1, 2, 4};
        for (U8 i = 0; i < 3; i++) {
            U8 pduIdx = missingPduIndices[i];
            U32 offset = pduIdx * dataPerPdu;
            sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, offset,
                            dataPerPdu, testData + offset, Cfdp::Class::CLASS_2);
            component.doDispatch();
        }

        EXPECT_TRUE(setup.txn->m_flags.rx.complete);

        // Wait for FIN after retransmission
        bool foundFin = false;

        for (U32 cycle = 0; cycle < maxCycles && !foundFin; ++cycle) {
            this->invoke_to_run1Hz(0, 0);
            this->component.doDispatch();

            if (this->fromPortHistory_dataOut->size() > pduCountBeforeRetransmit) {
                FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
                Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
                Cfdp::FinPdu finPdu;
                const U8* pduData;
                FwSizeType pduSize;
                if (this->getPduData(lastPdu, pduData, pduSize)) {
                    Fw::SerialBuffer sb(const_cast<U8*>(pduData), pduSize);
                    sb.setBuffLen(pduSize);
                    if (finPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                        foundFin = true;
                    }
                }
            }
        }

        ASSERT_TRUE(foundFin);
    } else {
        // Wait for FIN (no NAK)
        U32 maxCycles = 20;
        bool foundFin = false;

        for (U32 cycle = 0; cycle < maxCycles && !foundFin; ++cycle) {
            this->invoke_to_run1Hz(0, 0);
            this->component.doDispatch();

            if (this->fromPortHistory_dataOut->size() > 1) {
                FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
                Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
                Cfdp::FinPdu finPdu;
                const U8* pduData;
                FwSizeType pduSize;
                if (this->getPduData(lastPdu, pduData, pduSize)) {
                    Fw::SerialBuffer sb(const_cast<U8*>(pduData), pduSize);
                    sb.setBuffLen(pduSize);
                    if (finPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                        foundFin = true;
                    }
                }
            }
        }

        ASSERT_TRUE(foundFin);
    }

    // Verify transaction state before FIN-ACK
    EXPECT_EQ(TxnState::TXN_STATE_R2, setup.txn->m_state);
    EXPECT_EQ(RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC, setup.txn->m_state_data.receive.sub_state);

    // Send FIN-ACK
    this->sendAckPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
                     Cfdp::FileDirective::FILE_DIRECTIVE_FIN, 1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                     Cfdp::AckTxnStatus::ACK_TXN_STATUS_TERMINATED);
    this->component.doDispatch();

    // Verify transaction completed
    EXPECT_EQ(TxnState::TXN_STATE_HOLD, setup.txn->m_state);

    // Verify completion event was emitted
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    ASSERT_EVENTS_RxFileTransferCompleted(0,  // index
                                          Cfdp::Class::CLASS_2, transactionSeq, TEST_GROUND_EID, groundSrcFile,
                                          component.getLocalEidParam(), dstFile, static_cast<U32>(actualFileSize));

    // Wait for transaction recycle
    waitForTransactionRecycle(channelId, transactionSeq);

    // Verify file
    verifyReceivedFile(dstFile, testData, actualFileSize);

    // Verify telemetry for Class2 RX transaction
    ASSERT_GE(this->tlmHistory_ChannelTelemetry->size(), 1u);
    U32 tlmIndex = static_cast<U32>(this->tlmHistory_ChannelTelemetry->size() - 1);
    Cfdp::ChannelTelemetryArray tlm = this->tlmHistory_ChannelTelemetry->at(tlmIndex).arg;

    // Verify RX counters (cumulative across all transactions on this channel)
    U8 numFileDataPdus = static_cast<U8>(actualFileSize / dataPerPdu);
    U32 expectedRecvPdus = 1 + numFileDataPdus + 1 + 1;  // Metadata + FileData PDUs + EOF + FIN-ACK
    if (simulateNak) {
        expectedRecvPdus += 3;  // Add 3 retransmitted FileData PDUs
    }

    if (expectExactCounts && !simulateNak) {
        // Single-transaction test without NAKs: expect exact counts
        EXPECT_EQ(expectedRecvPdus, tlm[channelId].get_recvPdu())
            << "recvPdu should be exactly Metadata + FileData + EOF + FIN-ACK";
        EXPECT_EQ(actualFileSize, tlm[channelId].get_recvFileDataBytes())
            << "recvFileDataBytes should exactly match file size";
        EXPECT_EQ(2u, tlm[channelId].get_sentPdu()) << "sentPdu should be exactly 2 (EOF-ACK + FIN)";
    } else {
        // Multi-transaction test or NAK test: use lower bounds
        EXPECT_GT(tlm[channelId].get_recvPdu(), numFileDataPdus)
            << "recvPdu should include Metadata + FileData + EOF + FIN-ACK";
        EXPECT_GE(tlm[channelId].get_recvFileDataBytes(), actualFileSize)
            << "recvFileDataBytes should be at least file size (cumulative)";
        EXPECT_GE(tlm[channelId].get_sentPdu(), 2u) << "sentPdu should be at least 2 (EOF-ACK + FIN)";
    }
    if (simulateNak) {
        EXPECT_GT(tlm[channelId].get_sentNakSegmentRequests(), 0u)
            << "NAK segment requests should be sent when gaps detected";
    }

    // Verify no errors occurred
    EXPECT_EQ(0u, tlm[channelId].get_recvErrors()) << "No receive errors should occur";

    // Clean up
    delete[] testData;
    cleanupTestFile(dstFile);
    cleanupTestFile(srcFile);
}

void CfdpManagerTester::sendAndVerifyClass2Tx(TransactionInitType initType,
                                              const char* srcFile,
                                              const char* dstFile,
                                              FwSizeType expectedFileSize,
                                              bool simulateNak,
                                              bool expectExactCounts) {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const U8 channelId = (initType == TransactionInitType::INIT_BY_COMMAND) ? TEST_CHANNEL_ID_1 : TEST_CHANNEL_ID_0;

    // Create and verify test file
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    // Setup transaction
    TransactionSetup setup;

    if (initType == TransactionInitType::INIT_BY_COMMAND) {
        setupTxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_2, TEST_PRIORITY,
                           TxnState::TXN_STATE_S2, setup);
    } else {
        // Initiate via port
        setupTxPortTransaction(srcFile, dstFile, channelId, TxnState::TXN_STATE_S2, setup);
    }

    // Run engine cycle - Metadata + FileData PDUs
    U8 numFileDataPdus = static_cast<U8>(fileSize / dataPerPdu);
    if (fileSize % dataPerPdu != 0) {
        numFileDataPdus++;
    }

    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1 + numFileDataPdus);

    verifyMetadataPduAtIndex(0, setup, expectedFileSize, srcFile, dstFile, Cfdp::Class::CLASS_2);
    verifyMultipleFileDataPdus(1, numFileDataPdus, setup, dataPerPdu, srcFile, Cfdp::Class::CLASS_2);

    EXPECT_EQ(expectedFileSize, setup.txn->m_foffs);
    EXPECT_EQ(TxSubState::TX_SUB_STATE_CLOSEOUT_SYNC, setup.txn->m_state_data.send.sub_state);
    EXPECT_TRUE(setup.txn->m_flags.tx.send_eof);

    // Run cycle - EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    FwIndexType firstEofIndex = static_cast<FwIndexType>(1 + numFileDataPdus);
    ASSERT_FROM_PORT_HISTORY_SIZE(static_cast<U32>(firstEofIndex + 1));

    Fw::Buffer firstEofPduBuffer = this->getSentPduBuffer(firstEofIndex);
    ASSERT_GT(firstEofPduBuffer.getSize(), 0);
    verifyEofPdu(firstEofPduBuffer, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum,
                 Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, static_cast<FileSize>(expectedFileSize), srcFile);

    EXPECT_FALSE(setup.txn->m_flags.tx.send_eof);

    // Handle NAK if requested
    if (simulateNak) {
        // Clear history for retransmission
        this->clearHistory();
        this->m_pduCopyCount = 0;

        // Send NAK requesting retransmission of PDUs 2 and 5
        Cfdp::SegmentRequest segments[2];
        segments[0].offsetStart = dataPerPdu;
        segments[0].offsetEnd = 2 * dataPerPdu;
        segments[1].offsetStart = 4 * dataPerPdu;
        segments[1].offsetEnd = 5 * dataPerPdu;

        this->sendNakPdu(channelId, component.getLocalEidParam(), TEST_GROUND_EID, setup.expectedSeqNum, 0,
                         static_cast<FileSize>(expectedFileSize), 2, segments);
        this->component.doDispatch();

        // Run cycles until second EOF
        U32 maxCycles = 20;
        bool foundSecondEof = false;

        for (U32 cycle = 0; cycle < maxCycles && !foundSecondEof; ++cycle) {
            this->invoke_to_run1Hz(0, 0);
            this->component.doDispatch();

            if (this->fromPortHistory_dataOut->size() > 0) {
                FwIndexType lastIndex = static_cast<FwIndexType>(this->fromPortHistory_dataOut->size() - 1);
                Fw::Buffer lastPdu = this->getSentPduBuffer(lastIndex);
                Cfdp::EofPdu eofPdu;
                const U8* pduData;
                FwSizeType pduSize;
                if (this->getPduData(lastPdu, pduData, pduSize)) {
                    Fw::SerialBuffer sb(const_cast<U8*>(pduData), pduSize);
                    sb.setBuffLen(pduSize);
                    if (eofPdu.deserializeFrom(sb) == Fw::FW_SERIALIZE_OK) {
                        foundSecondEof = true;
                    }
                }
            }
        }

        ASSERT_TRUE(foundSecondEof) << "Second EOF should be sent after NAK retransmission";
    }

    // Complete Class 2 handshake
    completeClass2Handshake(channelId, TEST_GROUND_EID, setup.expectedSeqNum, setup.txn);

    // If port-initiated, verify fileDoneOut callback BEFORE clearing history
    if (initType == TransactionInitType::INIT_BY_PORT) {
        ASSERT_EQ(1u, this->fromPortHistory_fileDoneOut->size())
            << "fileDoneOut port should be invoked once for port-initiated transfer";
        Svc::SendFileResponse completionResp = this->fromPortHistory_fileDoneOut->at(0).resp;
        ASSERT_EQ(Svc::SendFileStatus::STATUS_OK, completionResp.get_status()) << "fileDoneOut should indicate success";
    }

    // Verify completion event was emitted
    ASSERT_EVENTS_TxFileTransferCompleted_SIZE(1);
    ASSERT_EVENTS_TxFileTransferCompleted(0,  // index
                                          Cfdp::Class::CLASS_2, setup.expectedSeqNum, component.getLocalEidParam(),
                                          srcFile, TEST_GROUND_EID, dstFile, static_cast<U32>(expectedFileSize));

    // Wait for transaction recycle
    waitForTransactionRecycle(channelId, setup.expectedSeqNum);

    // Verify telemetry for Class2 TX transaction
    ASSERT_GE(this->tlmHistory_ChannelTelemetry->size(), 1u);
    U32 tlmIndex = static_cast<U32>(this->tlmHistory_ChannelTelemetry->size() - 1);
    Cfdp::ChannelTelemetryArray tlm = this->tlmHistory_ChannelTelemetry->at(tlmIndex).arg;

    // Verify TX counters (Metadata + FileData PDUs + EOF(s) + FIN-ACK)
    U32 expectedSentPdus = 1 + numFileDataPdus + 1 + 1;  // Metadata + FileData + EOF + FIN-ACK
    if (simulateNak) {
        expectedSentPdus += 3;  // Add 2 retransmitted FileData PDUs + second EOF
    }

    if (expectExactCounts && !simulateNak) {
        // Single-transaction test without NAKs: expect exact counts
        EXPECT_EQ(expectedSentPdus, tlm[channelId].get_sentPdu())
            << "sentPdu should be exactly Metadata + FileData + EOF + FIN-ACK";
        EXPECT_EQ(fileSize, tlm[channelId].get_sentFileDataBytes())
            << "sentFileDataBytes should exactly match file size";
        EXPECT_EQ(2u, tlm[channelId].get_recvPdu()) << "recvPdu should be exactly 2 (EOF-ACK + FIN)";
    } else {
        // Multi-transaction test or NAK test: use lower bounds
        EXPECT_GE(tlm[channelId].get_sentPdu(), expectedSentPdus - 1)
            << "sentPdu should include Metadata + FileData + EOF + FIN-ACK";
        EXPECT_GE(tlm[channelId].get_sentFileDataBytes(), fileSize)
            << "sentFileDataBytes should be at least file size (may include retransmissions)";
        EXPECT_GE(tlm[channelId].get_recvPdu(), 2u) << "recvPdu should be at least 2 (EOF-ACK + FIN)";
    }
    if (simulateNak) {
        EXPECT_GT(tlm[channelId].get_recvNakSegmentRequests(), 0u)
            << "NAK segment requests should be received when peer requests retransmission";
    }

    // Verify no errors occurred
    EXPECT_EQ(0u, tlm[channelId].get_recvErrors()) << "No receive errors should occur";

    // Clean up
    cleanupTestFile(srcFile);
}

// ----------------------------------------------------------------------
// Command based Transaction Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testClass1TxNominal() {
    sendAndVerifyClass1Tx("test/ut/output/test_class1_tx.bin", "test/ut/output/test_class1_tx_dst.dat",
                          component.getOutgoingFileChunkSizeParam());
}

void CfdpManagerTester::testClass2TxNominal() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Tx(TransactionInitType::INIT_BY_COMMAND, "test/ut/output/test_class2_tx_5pdu.bin",
                          "test/ut/output/test_class2_tx_dst.dat", expectedFileSize,
                          false,  // No NAK simulation
                          true    // Expect exact counts (single transaction)
    );
}

void CfdpManagerTester::testClass2TxNack() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Tx(TransactionInitType::INIT_BY_COMMAND, "test/ut/output/test_c2_tx_nak.bin",
                          "test/ut/output/test_c2_nak_dst.dat", expectedFileSize,
                          true  // Simulate NAK
    );
}

void CfdpManagerTester::testClass2TxLateFinAck() {
    // Exercises Engine::receivePdu's stateless FIN-ACK path: a retransmitted FIN arrives for a
    // downlink (TX) transaction this entity sourced, but that transaction has already completed
    // and been recycled (no live transaction remains). Per CFDP the sender must still acknowledge
    // the FIN, so the engine re-ACKs it statelessly and emits TxLateFinAcked.
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;
    const char* srcFile = "test/ut/output/c2_latefin.bin";
    const char* dstFile = "test/ut/output/c2_latefin_dst.dat";
    const U8 channelId = TEST_CHANNEL_ID_1;

    // Create the source file and drive a full nominal Class 2 TX to completion.
    FwSizeType fileSize;
    createAndVerifyTestFile(srcFile, expectedFileSize, fileSize);

    TransactionSetup setup;
    setupTxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_2, TEST_PRIORITY,
                       TxnState::TXN_STATE_S2, setup);
    const TransactionSeq seqNum = setup.expectedSeqNum;

    U8 numFileDataPdus = static_cast<U8>(fileSize / dataPerPdu);
    if (fileSize % dataPerPdu != 0) {
        numFileDataPdus++;
    }

    // Metadata + FileData PDUs
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1 + numFileDataPdus);
    // EOF PDU
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // Close out the handshake (EOF-ACK + FIN -> FIN-ACK) and confirm completion.
    completeClass2Handshake(channelId, TEST_GROUND_EID, seqNum, setup.txn);
    ASSERT_EVENTS_TxFileTransferCompleted_SIZE(1);

    // Let the inactivity timer fire so the transaction is recycled (txn == nullptr precondition).
    waitForTransactionRecycle(channelId, seqNum);
    ASSERT_EQ(nullptr, findTransaction(channelId, seqNum)) << "Transaction should be recycled before late FIN";

    // Only observe what the late FIN produces.
    this->clearHistory();
    this->m_pduCopyCount = 0;

    // Retransmitted FIN for the completed transaction. Its sourceEid is the local (sender) EID and
    // destEid is the ground peer -- exactly the header a FIN toward this sender carries, which is
    // what routes it into the stateless FIN-ACK branch.
    this->sendFinPdu(channelId, component.getLocalEidParam(), TEST_GROUND_EID, seqNum,
                     Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, Cfdp::FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE,
                     Cfdp::FinFileStatus::FIN_FILE_STATUS_RETAINED);
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();

    // A stateless ACK(FIN) must go out. Unlike the in-transaction FIN-ACK (verifyFinAckPdu, which
    // expects ACK_TXN_STATUS_TERMINATED), the stateless path reports ACK_TXN_STATUS_UNRECOGNIZED.
    ASSERT_EQ(1u, this->fromPortHistory_dataOut->size()) << "Exactly one PDU (the stateless FIN-ACK) should be sent";
    Fw::Buffer finAckBuffer = this->getSentPduBuffer(0);
    ASSERT_GT(finAckBuffer.getSize(), 0) << "Stateless FIN-ACK PDU should be sent";
    verifyAckPdu(finAckBuffer, component.getLocalEidParam(), TEST_GROUND_EID, seqNum,
                 Cfdp::FileDirective::FILE_DIRECTIVE_FIN, 1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                 Cfdp::AckTxnStatus::ACK_TXN_STATUS_UNRECOGNIZED);

    // The diagnostic event should be emitted with the source EID and sequence number.
    ASSERT_EVENTS_TxLateFinAcked_SIZE(1);
    ASSERT_EVENTS_TxLateFinAcked(0, component.getLocalEidParam(), seqNum);

    // No stray transaction should have been created by the late FIN.
    ASSERT_EQ(nullptr, findTransaction(channelId, seqNum)) << "Late FIN must not create a new transaction";

    cleanupTestFile(srcFile);
}

void CfdpManagerTester::testClass1RxNominal() {
    const U16 fileDataSize = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());

    sendAndVerifyClass1Rx("test/ut/output/test_rx_source.bin", "test/ut/output/test_rx_received.bin",
                          "/ground/test_rx_source.bin", fileDataSize);
}

void CfdpManagerTester::testClass2RxNominal() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Rx("test/ut/output/test_class2_rx_source.bin", "test/ut/output/test_class2_rx_received.bin",
                          "/ground/test_class2_rx_source.bin", expectedFileSize,
                          false,  // No NAK simulation
                          true    // Expect exact counts (single transaction)
    );
}

void CfdpManagerTester::testClass2RxNack() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Rx("test/ut/output/test_class2_rx_nack_source.bin",
                          "test/ut/output/test_class2_rx_nack_received.bin", "/ground/test_class2_rx_nack_source.bin",
                          expectedFileSize,
                          true  // Simulate NAK
    );
}

// ----------------------------------------------------------------------
// Port-Based Transaction Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testClass2TxPortBased() {
    // Port-initiated transfers use Class 2 for reliability
    sendAndVerifyClass2Tx(TransactionInitType::INIT_BY_PORT, "test/ut/output/test_class2_tx_port.bin",
                          "test/ut/output/test_class2_tx_port_dst.dat", component.getOutgoingFileChunkSizeParam(),
                          false,  // No NAK simulation
                          true    // Expect exact counts (single transaction)
    );
}

void CfdpManagerTester::testClass2TxPortBasedNack() {
    // Port-initiated Class 2 transfer that exercises NAK-driven retransmission,
    // mirroring testClass2TxNack but initiated via the fileIn port.
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());
    const FwSizeType expectedFileSize = 5 * dataPerPdu;

    sendAndVerifyClass2Tx(TransactionInitType::INIT_BY_PORT, "test/ut/output/test_c2_tx_port_nak.bin",
                          "test/ut/output/test_c2_tx_port_nak_dst.dat", expectedFileSize,
                          true  // Simulate NAK
    );
}

// ----------------------------------------------------------------------
// Multi-Transactions Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testMultipleTransactionsInSeries() {
    const U16 dataPerPdu = static_cast<U16>(this->component.getOutgoingFileChunkSizeParam());

    // Transaction 1: Class 1 TX (command-based)
    sendAndVerifyClass1Tx("test/ut/output/series_c1_tx.bin", "test/ut/output/series_c1_tx_dst.dat", dataPerPdu);

    // Transaction 2: Class 1 RX
    sendAndVerifyClass1Rx("test/ut/output/series_c1_rx_src.bin", "test/ut/output/series_c1_rx_dst.bin",
                          "/ground/series_c1_rx_src.bin", dataPerPdu);

    // Transaction 3: Class 2 TX (port-based)
    sendAndVerifyClass2Tx(TransactionInitType::INIT_BY_PORT, "test/ut/output/series_c2_tx.bin",
                          "test/ut/output/series_c2_tx_dst.dat", 5 * dataPerPdu,
                          false  // No NAK simulation
    );

    // Transaction 4: Class 2 RX
    sendAndVerifyClass2Rx("test/ut/output/series_c2_rx_src.bin", "test/ut/output/series_c2_rx_dst.bin",
                          "/ground/series_c2_rx_src.bin", 5 * dataPerPdu,
                          false  // No NAK simulation
    );
}

// ----------------------------------------------------------------------
// Miscellaneous Tests
// ----------------------------------------------------------------------

void CfdpManagerTester ::testPing() {
    const U32 key = 1234;
    this->invoke_to_pingIn(0, key);
    this->component.doDispatch();
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, key);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
