// ======================================================================
// \title  CfdpManagerRxDisposalTests.cpp
// \brief  Tests that a reception that does not complete is reported failed and leaves no file
// ======================================================================

#include <Os/FileSystem.hpp>
#include <Svc/Ccsds/CfdpManager/test/ut/CfdpManagerTester.hpp>

#include <cinttypes>
#include <cstring>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

namespace {

constexpr U16 RX_DISPOSAL_DATA_SIZE = 16;

bool fileExists(const char* path) {
    return Os::FileSystem::exists(path);
}

void fillData(U8* data, U16 size) {
    for (U16 i = 0; i < size; ++i) {
        data[i] = static_cast<U8>(i + 1);
    }
}

//! Staging file a receive on channel 0 from sourceEid writes before it commits to dst
Fw::String stagingPathOf(const char* dst, EntityId sourceEid, TransactionSeq transactionSeq) {
    Fw::String staging;
    (void)staging.format("%s.cfdp-0-%" PRIu32 "-%" PRIu32 ".part", dst, static_cast<U32>(sourceEid),
                         static_cast<U32>(transactionSeq));
    return staging;
}

U32 checksumOf(const U8* data, U16 size) {
    CFDP::Checksum crc;
    crc.update(data, 0, size);
    return crc.getValue();
}

}  // namespace

void CfdpManagerTester::runRxUntilRecycled(U8 channelId, TransactionSeq transactionSeq, U32 maxCycles) {
    for (U32 cycle = 0; cycle < maxCycles; ++cycle) {
        if (this->findTransaction(channelId, transactionSeq) == nullptr) {
            return;
        }
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }
    ASSERT_EQ(nullptr, this->findTransaction(channelId, transactionSeq))
        << "transaction " << transactionSeq << " was never recycled";
}

U32 CfdpManagerTester::rxRecycleCycleBudget(U8 channelId) {
    // Inactivity timer and post-inactivity retries, plus the full class 2 ACK/NAK retry schedule
    const U32 ackBudget = static_cast<U32>(this->component.getAckTimerParam(channelId)) *
                          (static_cast<U32>(this->component.getAckLimitParam(channelId)) +
                           static_cast<U32>(this->component.getNackLimitParam(channelId)) + 2);
    return this->component.getInactivityTimerParam(channelId) + this->component.getPostInactivitySendRetriesParam() +
           ackBudget + 10;
}

void CfdpManagerTester::expectRxFailedAndRemoved(TxnStatus expectedStatus, const char* path) {
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(1);
    if (this->eventHistory_RxFileTransferFailed->size() == 1) {
        EXPECT_EQ(static_cast<U8>(expectedStatus), this->eventHistory_RxFileTransferFailed->at(0).conditionCode);
    }
    ASSERT_EVENTS_DanglingFileHandleClosed_SIZE(0);
    ASSERT_EVENTS_FileRemoveFailed_SIZE(0);
    EXPECT_FALSE(fileExists(path)) << path << " should have been removed";
}

void CfdpManagerTester::testClass1RxChecksumMismatchRemovesFile() {
    const U8 channelId = 0;
    const TransactionSeq transactionSeq = 520;
    const char* srcFile = "/ground/test_rx_disposal_crc_source.bin";
    const char* dstFile = "test/ut/output/test_rx_disposal_crc_received.bin";
    U8 data[RX_DISPOSAL_DATA_SIZE];
    fillData(data, sizeof(data));

    cleanupTestFile(dstFile);
    this->clearHistory();

    TransactionSetup setup;
    setupRxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_1, sizeof(data), transactionSeq,
                       TxnState::TXN_STATE_R1, setup);
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, 0, sizeof(data), data,
                    Cfdp::Class::CLASS_1);
    component.doDispatch();
    // Data is staged; the destination is not written until the transfer is verified
    ASSERT_TRUE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    ASSERT_FALSE(fileExists(dstFile));

    sendEofPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
               Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, checksumOf(data, sizeof(data)) + 1, sizeof(data),
               Cfdp::Class::CLASS_1);
    component.doDispatch();

    ASSERT_EVENTS_RxCrcMismatch_SIZE(1);
    expectRxFailedAndRemoved(TxnStatus::TXN_STATUS_FILE_CHECKSUM_FAILURE, dstFile);
    EXPECT_FALSE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    cleanupTestFile(dstFile);
}

void CfdpManagerTester::testClass1RxSizeMismatchRemovesFile() {
    const U8 channelId = 0;
    const TransactionSeq transactionSeq = 521;
    const char* srcFile = "/ground/test_rx_disposal_size_source.bin";
    const char* dstFile = "test/ut/output/test_rx_disposal_size_received.bin";
    U8 data[RX_DISPOSAL_DATA_SIZE];
    fillData(data, sizeof(data));

    cleanupTestFile(dstFile);
    this->clearHistory();

    TransactionSetup setup;
    setupRxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_1, sizeof(data), transactionSeq,
                       TxnState::TXN_STATE_R1, setup);
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, 0, sizeof(data), data,
                    Cfdp::Class::CLASS_1);
    component.doDispatch();
    // Data is staged; the destination is not written until the transfer is verified
    ASSERT_TRUE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    ASSERT_FALSE(fileExists(dstFile));

    // Correct checksum, but the EOF declares a different size than the metadata
    sendEofPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
               Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, checksumOf(data, sizeof(data)), sizeof(data) + 4,
               Cfdp::Class::CLASS_1);
    component.doDispatch();

    ASSERT_EVENTS_RxFileSizeMismatch_SIZE(1);
    expectRxFailedAndRemoved(TxnStatus::TXN_STATUS_FILE_SIZE_ERROR, dstFile);
    EXPECT_FALSE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    cleanupTestFile(dstFile);
}

void CfdpManagerTester::testClass2RxChecksumMismatchRemovesFile() {
    const U8 channelId = 0;
    const TransactionSeq transactionSeq = 522;
    const char* srcFile = "/ground/test_rx_disposal_crc2_source.bin";
    const char* dstFile = "test/ut/output/test_rx_disposal_crc2_received.bin";
    U8 data[RX_DISPOSAL_DATA_SIZE];
    fillData(data, sizeof(data));

    cleanupTestFile(dstFile);
    this->clearHistory();

    TransactionSetup setup;
    setupRxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, Cfdp::Class::CLASS_2, sizeof(data), transactionSeq,
                       TxnState::TXN_STATE_R2, setup);
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, 0, sizeof(data), data,
                    Cfdp::Class::CLASS_2);
    component.doDispatch();
    sendEofPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq,
               Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR, checksumOf(data, sizeof(data)) + 1, sizeof(data),
               Cfdp::Class::CLASS_2);
    component.doDispatch();
    // Data is staged; the destination is not written until the transfer is verified
    ASSERT_TRUE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    ASSERT_FALSE(fileExists(dstFile));

    // The sender never acknowledges the FIN, so the transaction ends on its retry limits
    runRxUntilRecycled(channelId, transactionSeq, rxRecycleCycleBudget(channelId));

    ASSERT_EVENTS_RxCrcMismatch_SIZE(1);
    expectRxFailedAndRemoved(TxnStatus::TXN_STATUS_FILE_CHECKSUM_FAILURE, dstFile);
    EXPECT_FALSE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    cleanupTestFile(dstFile);
}

void CfdpManagerTester::runRxInactivityCase(Cfdp::Class::T cfdpClass, TransactionSeq transactionSeq) {
    const U8 channelId = 0;
    const char* srcFile = "/ground/test_rx_disposal_timeout_source.bin";
    const char* dstFile = "test/ut/output/test_rx_disposal_timeout_received.bin";
    U8 data[RX_DISPOSAL_DATA_SIZE];
    fillData(data, sizeof(data));

    cleanupTestFile(dstFile);
    this->clearHistory();

    // The sender declares more data than it sends, then goes quiet
    TransactionSetup setup;
    setupRxTransaction(srcFile, dstFile, channelId, TEST_GROUND_EID, cfdpClass, 10 * sizeof(data), transactionSeq,
                       (cfdpClass == Cfdp::Class::CLASS_1) ? TxnState::TXN_STATE_R1 : TxnState::TXN_STATE_R2, setup);
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, 0, sizeof(data), data,
                    cfdpClass);
    component.doDispatch();
    // Data is staged; the destination is not written until the transfer is verified
    ASSERT_TRUE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    ASSERT_FALSE(fileExists(dstFile));

    runRxUntilRecycled(channelId, transactionSeq, rxRecycleCycleBudget(channelId));

    if (cfdpClass == Cfdp::Class::CLASS_1) {
        // Class 1 has no retries: the inactivity timer ends it and recycles the slot directly
        ASSERT_EVENTS_RxInactivityTimeout_SIZE(1);
        expectRxFailedAndRemoved(TxnStatus::TXN_STATUS_INACTIVITY_DETECTED, dstFile);
        EXPECT_FALSE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    } else {
        // Class 2 keeps NAKing the missing data and gives up on the NAK limit first
        ASSERT_EVENTS_RxNakLimitReached_SIZE(1);
        expectRxFailedAndRemoved(TxnStatus::TXN_STATUS_NAK_LIMIT_REACHED, dstFile);
        EXPECT_FALSE(fileExists(stagingPathOf(dstFile, TEST_GROUND_EID, transactionSeq).toChar()));
    }
    cleanupTestFile(dstFile);
}

void CfdpManagerTester::testClass1RxInactivityRemovesFile() {
    runRxInactivityCase(Cfdp::Class::CLASS_1, 523);
}

void CfdpManagerTester::testClass2RxStalledRemovesFile() {
    runRxInactivityCase(Cfdp::Class::CLASS_2, 524);
}

void CfdpManagerTester::testClass2RxWithoutMetadataRemovesTempFile() {
    const U8 channelId = 0;
    const TransactionSeq transactionSeq = 525;
    U8 data[RX_DISPOSAL_DATA_SIZE];
    fillData(data, sizeof(data));

    this->clearHistory();

    // File data with no metadata: the reception is staged in a temporary file
    sendFileDataPdu(channelId, TEST_GROUND_EID, component.getLocalEidParam(), transactionSeq, 0, sizeof(data), data,
                    Cfdp::Class::CLASS_2);
    component.doDispatch();
    ASSERT_EVENTS_RxTempFileCreated_SIZE(1);
    const Fw::String tmpFile = this->eventHistory_RxTempFileCreated->at(0).filename;
    ASSERT_TRUE(fileExists(tmpFile.toChar())) << tmpFile.toChar();

    runRxUntilRecycled(channelId, transactionSeq, rxRecycleCycleBudget(channelId));

    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(1);
    ASSERT_EVENTS_DanglingFileHandleClosed_SIZE(0);
    ASSERT_EVENTS_FileRemoveFailed_SIZE(0);
    EXPECT_FALSE(fileExists(tmpFile.toChar())) << tmpFile.toChar() << " should have been removed";
    cleanupTestFile(tmpFile.toChar());
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
