// ======================================================================
// \title  CfdpManagerRxStagingTests.cpp
// \brief  Receive staging, commit and cleanup tests (issue #6230)
//
// A received file is written to a staging file beside its destination and renamed onto the
// destination only once the transfer has completed and its checksum has passed. These tests
// check that an incomplete or failed receive leaves nothing at the destination, never alters an
// existing destination file, and removes only the files the transaction itself created. Every
// file they use is under test/ut/output.
// ======================================================================

#include <cinttypes>
#include <cstring>

#include "CFDP/Checksum/Checksum.hpp"
#include "CfdpManagerTester.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

namespace {

constexpr U8 RX_CHANNEL = 0;
const char* const RX_OUTPUT_DIR = "test/ut/output";
const char* const RX_DST = "test/ut/output/rx_staging_dst.bin";
const char* const RX_GROUND_SRC = "/ground/rx_staging_src.bin";
const char* const RX_SENTINEL = "test/ut/output/rx_staging_dst.bin/sentinel";

//! Size declared by the Metadata PDU of a transfer that is cut off after one File Data PDU
constexpr FileSize RX_DECLARED_SIZE = 100;
constexpr U16 RX_CHUNK_SIZE = 10;
const U8 RX_NEW_DATA[RX_CHUNK_SIZE] = {'N', 'E', 'W', '-', 'D', 'A', 'T', 'A', '-', '1'};
const U8 RX_ORIGINAL[] = {'O', 'R', 'I', 'G', 'I', 'N', 'A', 'L', '-', 'G', 'O', 'O', 'D', '-', 'F', 'I', 'L', 'E'};
const U8 RX_FOREIGN[] = {'F', 'O', 'R', 'E', 'I', 'G', 'N'};
const U8 RX_REPEATED_CHUNK[RX_CHUNK_SIZE] = {'R', 'E', 'P', 'E', 'A', 'T', 'E', 'D', '-', '0'};

//! Upper bound on run1Hz cycles for any transaction in these tests to be recycled
constexpr U32 RX_MAX_CYCLES = 300;

U32 crcOf(const U8* data, FwSizeType size) {
    CFDP::Checksum crc;
    crc.update(data, 0, static_cast<U32>(size));
    return crc.getValue();
}

}  // namespace

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void CfdpManagerTester::rxStagingBegin(TransactionSeq seq, bool existing, RxStagingPaths& paths, U32 inactivityTimer) {
    Cfdp::ChannelArrayParams channelConfig;
    for (FwSizeType chan = 0; chan < Cfdp::NumChannels; chan++) {
        Cfdp::ChannelParams& params = channelConfig[chan];
        // The default timers and limits, so transfers end the way they do on a stock channel
        params.set_ack_limit(10);
        params.set_nack_limit(10);
        params.set_ack_timer(2);
        params.set_inactivity_timer(inactivityTimer);
        params.set_dequeue_enabled(Fw::Enabled::ENABLED);
        params.set_move_dir(Fw::String(""));
        params.set_max_outgoing_pdus_per_cycle(64);
        params.set_tmp_dir(Fw::String(RX_OUTPUT_DIR));
        params.set_fail_dir(Fw::String(""));
        params.set_rx_dir(Fw::String(""));
    }
    this->paramSet_ChannelConfig(channelConfig, Fw::ParamValid::VALID);
    this->paramSend_ChannelConfig(0, 0);

    ASSERT_EQ(
        Fw::FormatStatus::SUCCESS,
        paths.staging.format("%s.cfdp-%" PRIu32 "-%" PRIu32 "-%" PRIu32 ".part", RX_DST, static_cast<U32>(RX_CHANNEL),
                             static_cast<U32>(TEST_GROUND_EID), static_cast<U32>(seq)));
    ASSERT_EQ(Fw::FormatStatus::SUCCESS, paths.tmp.format("%s/%" PRIu32 ":%" PRIu32 ".tmp", RX_OUTPUT_DIR,
                                                          static_cast<U32>(TEST_GROUND_EID), static_cast<U32>(seq)));
    this->rxStagingCleanup(paths);
    if (existing) {
        this->rxStagingWriteFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    }
    this->clearHistory();
}

void CfdpManagerTester::rxStagingCleanup(const RxStagingPaths& paths) {
    // Test cleanup; any of these may legitimately be absent
    (void)Os::FileSystem::removeFile(RX_SENTINEL);
    (void)Os::FileSystem::removeDirectory(RX_DST);  // only present in the commit-failure test
    (void)Os::FileSystem::removeFile(RX_DST);
    (void)Os::FileSystem::removeFile(paths.staging.toChar());
    (void)Os::FileSystem::removeFile(paths.tmp.toChar());
}

void CfdpManagerTester::rxStagingWriteFile(const char* path, const U8* data, FwSizeType size) {
    Os::File file;
    ASSERT_EQ(Os::File::OP_OK, file.open(path, Os::File::OPEN_CREATE, Os::File::OVERWRITE));
    FwSizeType written = size;
    const Os::File::Status status = file.write(data, written, Os::File::WAIT);
    file.close();
    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_EQ(size, written);
}

void CfdpManagerTester::rxStagingExpectFile(const char* path, const U8* data, FwSizeType size) {
    FwSizeType actualSize = 0;
    ASSERT_EQ(Os::FileSystem::OP_OK, Os::FileSystem::getFileSize(path, actualSize)) << path << " should exist";
    ASSERT_EQ(size, actualSize) << path << " has the wrong size";
    if (size > 0) {
        this->verifyReceivedFile(path, data, size);
    }
}

void CfdpManagerTester::rxStagingExpectNoFile(const char* path) {
    EXPECT_EQ(Os::FileSystem::NOT_EXIST, Os::FileSystem::getPathType(path)) << path << " should not exist";
}

void CfdpManagerTester::rxStagingExpectKept(const RxStagingPaths& paths) {
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingExpectNoFile(paths.staging.toChar());
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
}

void CfdpManagerTester::rxStagingExpectCommitted(const RxStagingPaths& paths, const U8* data, FwSizeType size) {
    this->rxStagingExpectFile(RX_DST, data, size);
    this->rxStagingExpectNoFile(paths.staging.toChar());
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(0);
}

void CfdpManagerTester::rxStagingExpectFailed(Cfdp::ConditionCode conditionCode) {
    ASSERT_EQ(1u, this->eventHistory_RxFileTransferFailed->size()) << "expected one RxFileTransferFailed";
    EXPECT_EQ(static_cast<U8>(conditionCode), this->eventHistory_RxFileTransferFailed->at(0).conditionCode);
}

void CfdpManagerTester::rxStagingTelemetry(Cfdp::ChannelTelemetry& tlm) {
    this->clearTlm();
    this->invoke_to_run1Hz(0, 0);
    this->component.doDispatch();
    ASSERT_GE(this->tlmHistory_ChannelTelemetry->size(), 1u);
    tlm = this->tlmHistory_ChannelTelemetry->at(this->tlmHistory_ChannelTelemetry->size() - 1).arg[RX_CHANNEL];
}

void CfdpManagerTester::rxStagingSendMetadata(TransactionSeq seq, Cfdp::Class::T cfdpClass, FileSize fileSize) {
    this->rxStagingSendMetadataTo(seq, cfdpClass, fileSize, RX_DST);
}

void CfdpManagerTester::rxStagingSendMetadataTo(TransactionSeq seq,
                                                Cfdp::Class::T cfdpClass,
                                                FileSize fileSize,
                                                const char* dst) {
    this->sendMetadataPdu(RX_CHANNEL, TEST_GROUND_EID, this->component.getLocalEidParam(), seq, fileSize, RX_GROUND_SRC,
                          dst, cfdpClass, (cfdpClass == Cfdp::Class::CLASS_1) ? 0 : 1);
    this->component.doDispatch();
}

void CfdpManagerTester::rxStagingSendSegment(TransactionSeq seq,
                                             Cfdp::Class::T cfdpClass,
                                             FileSize offset,
                                             const U8* data,
                                             U16 size) {
    this->sendFileDataPdu(RX_CHANNEL, TEST_GROUND_EID, this->component.getLocalEidParam(), seq, offset, size, data,
                          cfdpClass);
    this->component.doDispatch();
}

void CfdpManagerTester::rxStagingSendData(TransactionSeq seq, Cfdp::Class::T cfdpClass) {
    this->rxStagingSendSegment(seq, cfdpClass, 0, RX_NEW_DATA, RX_CHUNK_SIZE);
}

void CfdpManagerTester::rxStagingSendEof(TransactionSeq seq,
                                         Cfdp::Class::T cfdpClass,
                                         Cfdp::ConditionCode conditionCode,
                                         U32 checksum,
                                         FileSize fileSize) {
    this->sendEofPdu(RX_CHANNEL, TEST_GROUND_EID, this->component.getLocalEidParam(), seq, conditionCode, checksum,
                     fileSize, cfdpClass);
    this->component.doDispatch();
}

void CfdpManagerTester::rxStagingSendGoodEof(TransactionSeq seq, Cfdp::Class::T cfdpClass) {
    this->rxStagingSendEof(seq, cfdpClass, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(RX_NEW_DATA, sizeof(RX_NEW_DATA)), RX_CHUNK_SIZE);
}

void CfdpManagerTester::rxStagingClass1Segments(TransactionSeq seq,
                                                const U8* payload,
                                                FileSize fileSize,
                                                const FileSize* offsets,
                                                const U16* sizes,
                                                U32 count) {
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, fileSize);
    for (U32 i = 0; i < count; ++i) {
        this->rxStagingSendSegment(seq, Cfdp::Class::CLASS_1, offsets[i], &payload[offsets[i]], sizes[i]);
    }
    // The EOF declares the full size and carries the checksum of the full payload
    this->rxStagingSendEof(seq, Cfdp::Class::CLASS_1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(payload, fileSize), fileSize);
}

bool CfdpManagerTester::rxStagingRunUntilFreed(TransactionSeq seq) {
    bool freed = (this->findTransaction(RX_CHANNEL, seq) == nullptr);
    for (U32 cycle = 0; (cycle < RX_MAX_CYCLES) && !freed; ++cycle) {
        // Events are kept for the assertions; sent PDUs and telemetry are dropped so a long run
        // does not exhaust the tester's fixed-size histories
        this->clearFromPortHistory();
        this->clearTlm();
        this->m_pduCopyCount = 0;
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
        freed = (this->findTransaction(RX_CHANNEL, seq) == nullptr);
    }
    return freed;
}

void CfdpManagerTester::rxStagingRunClass2ToFinAck(TransactionSeq seq,
                                                   FinFileStatus* finFileStatus,
                                                   FinDeliveryCode* deliveryCode) {
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    for (U32 cycle = 0;
         (cycle < RX_MAX_CYCLES) && (txn->m_state_data.receive.sub_state != RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC);
         ++cycle) {
        this->clearFromPortHistory();
        this->clearTlm();
        this->m_pduCopyCount = 0;
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }
    ASSERT_EQ(RxSubState::RX_SUB_STATE_CLOSEOUT_SYNC, txn->m_state_data.receive.sub_state) << "FIN was never sent";
    if (finFileStatus != nullptr) {
        *finFileStatus = txn->m_state_data.receive.r2.fs;
    }
    if (deliveryCode != nullptr) {
        *deliveryCode = txn->m_state_data.receive.r2.dc;
    }

    this->sendAckPdu(RX_CHANNEL, TEST_GROUND_EID, this->component.getLocalEidParam(), seq,
                     Cfdp::FileDirective::FILE_DIRECTIVE_FIN, 1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                     Cfdp::AckTxnStatus::ACK_TXN_STATUS_TERMINATED);
    this->component.doDispatch();
    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
}

void CfdpManagerTester::rxStagingTimeout(Cfdp::Class::T cfdpClass, TransactionSeq seq) {
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // Metadata declares 100 bytes, one 10-byte File Data PDU arrives, then the sender goes quiet
    this->rxStagingSendMetadata(seq, cfdpClass, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, cfdpClass);
    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq)) << "transaction should be recycled after it times out";

    this->rxStagingExpectKept(paths);
    ASSERT_EVENTS_FileRemoveFailed_SIZE(0);
    if (cfdpClass == Cfdp::Class::CLASS_1) {
        // A Class 1 receive ends on its inactivity timer and is reported as failed
        ASSERT_EVENTS_RxInactivityTimeout_SIZE(1);
        ASSERT_EVENTS_DanglingFileHandleClosed_SIZE(0);
        this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_INACTIVITY_DETECTED);
    } else {
        // A Class 2 receive ends when its NAK limit, then its ACK limit, are reached
        ASSERT_EVENTS_RxNakLimitReached_SIZE(1);
        this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_NAK_LIMIT_REACHED);
    }

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::rxStagingCrcMismatch(Cfdp::Class::T cfdpClass, TransactionSeq seq) {
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingSendMetadata(seq, cfdpClass, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, cfdpClass);
    this->rxStagingSendEof(seq, cfdpClass, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(RX_NEW_DATA, sizeof(RX_NEW_DATA)) ^ 0x1u, RX_CHUNK_SIZE);
    if (cfdpClass == Cfdp::Class::CLASS_2) {
        this->rxStagingRunClass2ToFinAck(seq);
    }

    this->rxStagingExpectKept(paths);
    ASSERT_EVENTS_RxCrcMismatch_SIZE(1);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_CHECKSUM_FAILURE);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::rxStagingClass1Incomplete(TransactionSeq seq,
                                                  const U8* payload,
                                                  FileSize fileSize,
                                                  const FileSize* offsets,
                                                  const U16* sizes,
                                                  U32 count) {
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingClass1Segments(seq, payload, fileSize, offsets, sizes, count);

    this->rxStagingExpectKept(paths);
    ASSERT_EVENTS_RxCrcMismatch_SIZE(0);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_SIZE_ERROR);
    Cfdp::ChannelTelemetry tlm;
    this->rxStagingTelemetry(tlm);
    EXPECT_EQ(1u, tlm.get_faultFileSizeMismatch());

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::rxStagingCommitFailure(Cfdp::Class::T cfdpClass, TransactionSeq seq) {
    RxStagingPaths paths;
    this->rxStagingBegin(seq, false, paths);
    // The destination is a non-empty directory, so the final rename fails
    ASSERT_EQ(Os::FileSystem::OP_OK, Os::FileSystem::createDirectory(RX_DST));
    this->rxStagingWriteFile(RX_SENTINEL, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    this->rxStagingSendMetadata(seq, cfdpClass, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, cfdpClass);
    this->rxStagingSendGoodEof(seq, cfdpClass);
    if (cfdpClass == Cfdp::Class::CLASS_2) {
        // The FIN reports the data complete but the file discarded by the filestore
        FinFileStatus finFileStatus = FinFileStatus::FIN_FILE_STATUS_RETAINED;
        FinDeliveryCode deliveryCode = FinDeliveryCode::FIN_DELIVERY_CODE_INCOMPLETE;
        this->rxStagingRunClass2ToFinAck(seq, &finFileStatus, &deliveryCode);
        EXPECT_EQ(FinFileStatus::FIN_FILE_STATUS_DISCARDED_FILESTORE, finFileStatus);
        EXPECT_EQ(FinDeliveryCode::FIN_DELIVERY_CODE_COMPLETE, deliveryCode);
    }

    ASSERT_EVENTS_RxFileRenameFailed_SIZE(1);
    EXPECT_STREQ(paths.staging.toChar(), this->eventHistory_RxFileRenameFailed->at(0).tempFile.toChar());
    EXPECT_STREQ(RX_DST, this->eventHistory_RxFileRenameFailed->at(0).finalFile.toChar());
    EXPECT_EQ(Os::FileSystem::DIRECTORY, Os::FileSystem::getPathType(RX_DST));
    this->rxStagingExpectFile(RX_SENTINEL, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);

    this->rxStagingCleanup(paths);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testRxStagingClass1TimeoutKeepsExistingFile() {
    this->rxStagingTimeout(Cfdp::Class::CLASS_1, 7002);
}

void CfdpManagerTester::testRxStagingClass2NakLimitKeepsExistingFile() {
    this->rxStagingTimeout(Cfdp::Class::CLASS_2, 7004);
}

void CfdpManagerTester::testRxStagingClass1CommitReplacesExistingFile() {
    const TransactionSeq seq = 7005;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    // Until the transfer is verified, the data is only in the staging file
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_1);

    this->rxStagingExpectCommitted(paths, RX_NEW_DATA, sizeof(RX_NEW_DATA));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1CrcMismatchKeepsExistingFile() {
    this->rxStagingCrcMismatch(Cfdp::Class::CLASS_1, 7006);
}

void CfdpManagerTester::testRxStagingClass2CrcMismatchKeepsExistingFile() {
    this->rxStagingCrcMismatch(Cfdp::Class::CLASS_2, 7007);
}

void CfdpManagerTester::testRxStagingClass1EofCancelKeepsExistingFile() {
    const TransactionSeq seq = 7008;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    this->rxStagingSendEof(seq, Cfdp::Class::CLASS_1, Cfdp::ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED, 0,
                           RX_CHUNK_SIZE);

    this->rxStagingExpectKept(paths);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_CANCEL_REQUEST_RECEIVED);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataCommits() {
    const TransactionSeq seq = 7009;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // File Data before Metadata goes to the temp file; the destination is untouched until commit
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_2);
    this->rxStagingRunClass2ToFinAck(seq);

    this->rxStagingExpectCommitted(paths, RX_NEW_DATA, sizeof(RX_NEW_DATA));
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(0);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataOversizeDataNotCommitted() {
    const TransactionSeq seq = 7035;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // Before Metadata the declared size is unknown, so data past it is written to the temp file. The
    // checksum covers only the declared bytes, so it would pass and commit a file longer than declared.
    this->rxStagingSendSegment(seq, Cfdp::Class::CLASS_2, 0, RX_NEW_DATA, RX_CHUNK_SIZE);
    this->rxStagingSendSegment(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE, RX_REPEATED_CHUNK, RX_CHUNK_SIZE);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_2);

    FinFileStatus finStatus = FinFileStatus::FIN_FILE_STATUS_RETAINED;
    this->rxStagingRunClass2ToFinAck(seq, &finStatus);
    EXPECT_NE(FinFileStatus::FIN_FILE_STATUS_RETAINED, finStatus);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_SIZE_ERROR);
    this->rxStagingExpectKept(paths);
    Cfdp::ChannelTelemetry tlm;
    this->rxStagingTelemetry(tlm);
    EXPECT_EQ(1u, tlm.get_faultFileSizeMismatch());

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataUnknownTempSizeFails() {
    const TransactionSeq seq = 7036;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // If the temp file's size cannot be read when Metadata arrives, whether data lies past the declared
    // size is unknown, so the transfer fails before anything is staged (white box: the handle is closed)
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    txn->m_fd.close();
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(0);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(0);

    FinFileStatus finStatus = FinFileStatus::FIN_FILE_STATUS_RETAINED;
    this->rxStagingRunClass2ToFinAck(seq, &finStatus);
    EXPECT_NE(FinFileStatus::FIN_FILE_STATUS_RETAINED, finStatus);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_SIZE_ERROR);
    this->rxStagingExpectKept(paths);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2RepeatedMetadataIgnored() {
    const TransactionSeq seq = 7037;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // A Metadata PDU repeated after the first was accepted changes nothing: no second file is claimed
    // or moved, and the transfer commits its data once
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(0);
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(0);

    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_2);
    this->rxStagingRunClass2ToFinAck(seq);
    this->rxStagingExpectCommitted(paths, RX_NEW_DATA, sizeof(RX_NEW_DATA));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataMoveFailureKeepsExistingFile() {
    const TransactionSeq seq = 7010;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // The temp file disappears before Metadata arrives, so moving it into the claimed staging file
    // fails; the claimed staging file is removed and the destination is untouched
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    ASSERT_EQ(Os::FileSystem::OP_OK, Os::FileSystem::removeFile(paths.tmp.toChar()));
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(1);
    EXPECT_STREQ(paths.tmp.toChar(), this->eventHistory_RxFileRenameFailed->at(0).tempFile.toChar());
    EXPECT_STREQ(paths.staging.toChar(), this->eventHistory_RxFileRenameFailed->at(0).finalFile.toChar());
    this->rxStagingExpectNoFile(paths.staging.toChar());

    // A retransmitted Metadata PDU must not retry the move on a failed transaction
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileRenameFailed_SIZE(1);

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingExpectKept(paths);
    // Removing the transaction's own vanished temp file is best effort and reports no event
    ASSERT_EVENTS_FileRemoveFailed_SIZE(0);
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    Cfdp::ChannelTelemetry tlm;
    this->rxStagingTelemetry(tlm);
    EXPECT_EQ(1u, tlm.get_faultFileRename());

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataSizeMismatchThenRetransmit() {
    const TransactionSeq seq = 7012;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // File Data and EOF arrive before Metadata, then a Metadata PDU whose size disagrees with the EOF
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_DECLARED_SIZE);
    ASSERT_EVENTS_RxEofMdSizeMismatch_SIZE(1);

    // A retransmitted, consistent Metadata PDU and more File Data must not reach the destination
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    EXPECT_EQ(TxnStatus::TXN_STATUS_FILE_SIZE_ERROR, txn->m_history->txn_stat);
    EXPECT_FALSE(txn->m_flags.rx.md_recv);

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingExpectKept(paths);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingExistingStagingFileNotReplaced() {
    const TransactionSeq seq = 7013;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);
    // A file already at the staging path is not this transaction's
    this->rxStagingWriteFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_1);

    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_1, TEST_GROUND_EID, seq, paths.staging.toChar(),
                                     Os::File::FILE_EXISTS);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);
    Cfdp::ChannelTelemetry tlm;
    this->rxStagingTelemetry(tlm);
    EXPECT_EQ(1u, tlm.get_faultFileOpen());

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2ExistingStagingFileNotReplaced() {
    const TransactionSeq seq = 7038;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);
    // A file already at the staging path is not this transaction's, and Metadata arrives first
    this->rxStagingWriteFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    // FILE_EXISTS is what the POSIX Os::File backend reports for an exclusive create of an existing file
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_2, TEST_GROUND_EID, seq, paths.staging.toChar(),
                                     Os::File::FILE_EXISTS);

    FinFileStatus finStatus = FinFileStatus::FIN_FILE_STATUS_RETAINED;
    this->rxStagingRunClass2ToFinAck(seq, &finStatus);
    EXPECT_NE(FinFileStatus::FIN_FILE_STATUS_RETAINED, finStatus);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1EofSizeMismatchFails() {
    const TransactionSeq seq = 7039;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // EOF declares a different size than Metadata: the transfer fails with FILE_SIZE_ERROR
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    this->rxStagingSendEof(seq, Cfdp::Class::CLASS_1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(RX_NEW_DATA, sizeof(RX_NEW_DATA)), 2 * RX_CHUNK_SIZE);

    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_SIZE_ERROR);
    this->rxStagingExpectKept(paths);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2LateMetadataExistingStagingFileNotReplaced() {
    const TransactionSeq seq = 7014;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);
    this->rxStagingWriteFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));

    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_2, TEST_GROUND_EID, seq, paths.staging.toChar(),
                                     Os::File::FILE_EXISTS);

    // Retransmitted Metadata on the failed transaction is ignored
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingExpectFile(paths.staging.toChar(), RX_FOREIGN, sizeof(RX_FOREIGN));
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingCommitFailureKeepsDestination() {
    this->rxStagingCommitFailure(Cfdp::Class::CLASS_1, 7015);
}

void CfdpManagerTester::testRxStagingClass2CommitFailureKeepsDestination() {
    this->rxStagingCommitFailure(Cfdp::Class::CLASS_2, 7022);
}

void CfdpManagerTester::testRxStagingPathFormat() {
    const TransactionSeq seq = 7016;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, false, paths);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);

    Fw::String staging;
    ASSERT_EQ(Cfdp::Status::SUCCESS, txn->rFormatStagingPath(Fw::String(RX_DST), staging));
    EXPECT_STREQ(paths.staging.toChar(), staging.toChar());

    // A destination too long for the suffix is refused rather than truncated
    char longDst[FW_FIXED_LENGTH_STRING_SIZE];
    ::memset(longDst, 'a', sizeof(longDst) - 1);
    longDst[sizeof(longDst) - 1] = '\0';
    EXPECT_EQ(Cfdp::Status::ERROR, txn->rFormatStagingPath(Fw::String(longDst), staging));
    EXPECT_EQ(Os::File::OTHER_ERROR, txn->rStagingPathFor(Fw::String(longDst), staging));

    // Destinations named like a receive's own staging or temp file are refused; look-alikes are not
    // (ignoring case, for case-insensitive file systems, and with letters in the IDs, for missions that
    // format IDs in hex)
    const char* const reserved[] = {"d/x.cfdp-0-100-7.part",
                                    "d/.cfdp-1-2-3.part",
                                    "x.cfdp-12-345-6789.part",
                                    "d/100:7.tmp",
                                    "7:100.tmp",
                                    "d/x.CFDP-0-100-7.PART",
                                    "d/x.Cfdp-a1-B2-ff.part",
                                    "d/1A:7f.TMP",
                                    "d/x.cfdp-a-1-2.part",
                                    "d/x:1.tmp"};
    for (const char* name : reserved) {
        EXPECT_EQ(Os::File::INVALID_ARGUMENT, txn->rStagingPathFor(Fw::String(name), staging)) << name;
    }
    const char* const allowed[] = {"d/log.2024-01-05.part",   "d/x.cfdp-0-1.part", "d/x.cfdp-0-1-2.parts",
                                   "d/x.cfdp-0-1-2.part.bin", "d/a.tmp",           "d/x-y:1.tmp",
                                   "d/1:2.tmp.bin",           "d/100:7.tmp/f",     "d/1-2-3.part"};
    for (const char* name : allowed) {
        EXPECT_EQ(Os::File::OP_OK, txn->rStagingPathFor(Fw::String(name), staging)) << name;
    }

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingReservedNameParsing() {
    const TransactionSeq seq = 7034;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, false, paths);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    Fw::String staging;

    // Short ordinary names, including ones that end the scan at the start of the name, are allowed. These
    // pin the parser's behaviour; that its scan never moves before the start of the name is shown by
    // evaluating it in a constant expression, which a runtime test cannot observe.
    const char* const ordinary[] = {"",         "/",       "a",       "a.part", "ab.part", "1.part",
                                    "-.part",   "a.tmp",   "1.tmp",   ".tmp",   ".part",   "1-2.part",
                                    "d/a.part", "/a.part", "d/1.tmp", "a.PART", "1:.tmp",  ":1.tmp"};
    for (const char* name : ordinary) {
        EXPECT_EQ(Os::File::OP_OK, txn->rStagingPathFor(Fw::String(name), staging)) << "\"" << name << "\"";
    }

    // Reserved staging and temp names are refused in any directory
    const char* const reserved[] = {".cfdp-1-2-3.part",
                                    "x.cfdp-0-100-7.part",
                                    "d/x.cfdp-0-100-7.part",
                                    "/abs/x.cfdp-1-2-3.part",
                                    "a.b.cfdp-1-2-3.part",
                                    "d//x.cfdp-1-2-3.part",
                                    "1:2.tmp",
                                    "d/100:7.tmp",
                                    "/abs/12:34.tmp"};
    for (const char* name : reserved) {
        EXPECT_EQ(Os::File::INVALID_ARGUMENT, txn->rStagingPathFor(Fw::String(name), staging)) << name;
    }

    // Matching ignores ASCII case only
    const char* const mixedCase[] = {"X.CFDP-1-2-3.PART", "x.CfDp-Ab-cD-EF.PaRt", "AB:cd.TmP", "d/1:2.TMP"};
    for (const char* name : mixedCase) {
        EXPECT_EQ(Os::File::INVALID_ARGUMENT, txn->rStagingPathFor(Fw::String(name), staging)) << name;
    }

    // Malformed look-alikes are allowed: wrong field count, empty or non-ASCII fields, wrong separators,
    // missing marker, extra characters, or a name that is a directory
    const char* const malformed[] = {"x.cfdp-1-2.part",
                                     "x.cfdp-1--3.part",
                                     "x.cfdp-1-2-3-.part",
                                     "x.cfdp--1-2-3.part",
                                     "cfdp-1-2-3.part",
                                     "x.cfdp_1-2-3.part",
                                     "x.cfdp-1-2-3 .part",
                                     "x.cfdp-1-2-3.part/",
                                     "x.cfdp-1-2-3.part.",
                                     "x.cfdp-\xC3\xA9-1-2.part",
                                     "x.cfd-1-2-3.part",
                                     "1::2.tmp",
                                     "1:2:3.tmp",
                                     "a b:1.tmp",
                                     "1:2.tmp ",
                                     "\xC3\xA9:1.tmp",
                                     "1-2.tmp",
                                     "d/1:2.tmp/",
                                     "x.cfdp-1-2-3~.part",
                                     "1~:2.tmp",
                                     "x.cfdp-1-2-{3.part"};
    for (const char* name : malformed) {
        EXPECT_EQ(Os::File::OP_OK, txn->rStagingPathFor(Fw::String(name), staging)) << name;
    }

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1MissingTrailingZerosNotCommitted() {
    // 10 data bytes followed by 90 zero bytes; zeros add nothing to the CFDP checksum, so the
    // checksum of the 10 bytes received matches the checksum of the whole file
    U8 payload[RX_DECLARED_SIZE] = {};
    ::memcpy(payload, RX_NEW_DATA, sizeof(RX_NEW_DATA));
    const FileSize offsets[] = {0};
    const U16 sizes[] = {RX_CHUNK_SIZE};
    this->rxStagingClass1Incomplete(7017, payload, RX_DECLARED_SIZE, offsets, sizes, 1);
}

void CfdpManagerTester::testRxStagingClass1MissingMiddleSegmentNotCommitted() {
    // The missing middle segment is zeros, so the checksum alone cannot detect it, and the last
    // segment makes the staging file reach the declared size
    U8 payload[3 * RX_CHUNK_SIZE] = {};
    ::memcpy(&payload[0], RX_NEW_DATA, RX_CHUNK_SIZE);
    ::memcpy(&payload[2 * RX_CHUNK_SIZE], RX_NEW_DATA, RX_CHUNK_SIZE);
    const FileSize offsets[] = {0, 2 * RX_CHUNK_SIZE};
    const U16 sizes[] = {RX_CHUNK_SIZE, RX_CHUNK_SIZE};
    this->rxStagingClass1Incomplete(7018, payload, sizeof(payload), offsets, sizes, 2);
}

void CfdpManagerTester::testRxStagingClass1OutOfOrderSegmentsCommit() {
    const TransactionSeq seq = 7019;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    U8 payload[3 * RX_CHUNK_SIZE] = {};
    for (FwSizeType i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(i + 1);
    }
    const FileSize offsets[] = {2 * RX_CHUNK_SIZE, 0, RX_CHUNK_SIZE};
    const U16 sizes[] = {RX_CHUNK_SIZE, RX_CHUNK_SIZE, RX_CHUNK_SIZE};
    this->rxStagingClass1Segments(seq, payload, sizeof(payload), offsets, sizes, 3);

    this->rxStagingExpectCommitted(paths, payload, sizeof(payload));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1ZeroLengthCommits() {
    const TransactionSeq seq = 7021;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // A zero-length file has no File Data PDUs and nothing missing
    this->rxStagingClass1Segments(seq, RX_NEW_DATA, 0, nullptr, nullptr, 0);

    this->rxStagingExpectCommitted(paths, RX_NEW_DATA, 0);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingDestinationNamingActiveStagingFileRefused() {
    const TransactionSeq seqB = 7023;
    const TransactionSeq seqA = 7024;
    RxStagingPaths paths;
    this->rxStagingBegin(seqB, true, paths);

    // B is part way through a Class 1 receive, so its staging file holds B's data
    this->rxStagingSendMetadata(seqB, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seqB, Cfdp::Class::CLASS_1);

    // A names B's staging file as its destination: refused before anything is created
    this->rxStagingSendMetadataTo(seqA, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE, paths.staging.toChar());
    this->rxStagingSendSegment(seqA, Cfdp::Class::CLASS_1, 0, RX_REPEATED_CHUNK, RX_CHUNK_SIZE);
    this->rxStagingSendEof(seqA, Cfdp::Class::CLASS_1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(RX_REPEATED_CHUNK, sizeof(RX_REPEATED_CHUNK)), RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_1, TEST_GROUND_EID, seqA, paths.staging.toChar(),
                                     Os::File::INVALID_ARGUMENT);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    // B completes with its own data
    this->rxStagingSendGoodEof(seqB, Cfdp::Class::CLASS_1);
    this->rxStagingExpectFile(RX_DST, RX_NEW_DATA, sizeof(RX_NEW_DATA));
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    EXPECT_EQ(seqB, this->eventHistory_RxFileTransferCompleted->at(0).seqNum);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingDestinationNamingActiveTempFileRefused() {
    const TransactionSeq seqB = 7025;
    const TransactionSeq seqA = 7026;
    RxStagingPaths paths;
    this->rxStagingBegin(seqB, true, paths);

    // B's File Data arrives before its Metadata, so B holds a temp file
    this->rxStagingSendData(seqB, Cfdp::Class::CLASS_2);
    this->rxStagingExpectFile(paths.tmp.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));

    // A names B's temp file as its destination: refused before anything is created
    this->rxStagingSendMetadataTo(seqA, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE, paths.tmp.toChar());
    this->rxStagingSendSegment(seqA, Cfdp::Class::CLASS_1, 0, RX_REPEATED_CHUNK, RX_CHUNK_SIZE);
    this->rxStagingSendEof(seqA, Cfdp::Class::CLASS_1, Cfdp::ConditionCode::CONDITION_CODE_NO_ERROR,
                           crcOf(RX_REPEATED_CHUNK, sizeof(RX_REPEATED_CHUNK)), RX_CHUNK_SIZE);
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_1, TEST_GROUND_EID, seqA, paths.tmp.toChar(),
                                     Os::File::INVALID_ARGUMENT);
    this->rxStagingExpectFile(paths.tmp.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));

    // B learns its destination and completes with its own data
    this->rxStagingSendMetadata(seqB, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingSendGoodEof(seqB, Cfdp::Class::CLASS_2);
    this->rxStagingRunClass2ToFinAck(seqB);
    this->rxStagingExpectFile(RX_DST, RX_NEW_DATA, sizeof(RX_NEW_DATA));
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    EXPECT_EQ(seqB, this->eventHistory_RxFileTransferCompleted->at(0).seqNum);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingDestinationNamingActiveStagingFileRefusedLateMetadata() {
    const TransactionSeq seqB = 7032;
    const TransactionSeq seqA = 7033;
    RxStagingPaths paths;
    this->rxStagingBegin(seqB, true, paths);
    Fw::String tmpA;
    ASSERT_EQ(Fw::FormatStatus::SUCCESS, tmpA.format("%s/%" PRIu32 ":%" PRIu32 ".tmp", RX_OUTPUT_DIR,
                                                     static_cast<U32>(TEST_GROUND_EID), static_cast<U32>(seqA)));
    (void)Os::FileSystem::removeFile(tmpA.toChar());

    // B is part way through a Class 1 receive, so its staging file holds B's data
    this->rxStagingSendMetadata(seqB, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seqB, Cfdp::Class::CLASS_1);

    // A's File Data arrives before its Metadata, so A holds a temp file; A's late Metadata then names
    // B's staging file and is refused
    this->rxStagingSendSegment(seqA, Cfdp::Class::CLASS_2, 0, RX_REPEATED_CHUNK, RX_CHUNK_SIZE);
    this->rxStagingExpectFile(tmpA.toChar(), RX_REPEATED_CHUNK, sizeof(RX_REPEATED_CHUNK));
    this->rxStagingSendMetadataTo(seqA, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE, paths.staging.toChar());
    ASSERT_EVENTS_RxFileCreateFailed_SIZE(1);
    ASSERT_EVENTS_RxFileCreateFailed(0, Cfdp::Class::CLASS_2, TEST_GROUND_EID, seqA, paths.staging.toChar(),
                                     Os::File::INVALID_ARGUMENT);

    // A ends removing only its own temp file; B's staging file and the destination are untouched
    FinFileStatus finStatus = FinFileStatus::FIN_FILE_STATUS_RETAINED;
    this->rxStagingRunClass2ToFinAck(seqA, &finStatus);
    EXPECT_NE(FinFileStatus::FIN_FILE_STATUS_RETAINED, finStatus);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);
    this->rxStagingExpectNoFile(tmpA.toChar());
    this->rxStagingExpectFile(paths.staging.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    // B completes with its own data
    this->rxStagingSendGoodEof(seqB, Cfdp::Class::CLASS_1);
    this->rxStagingExpectFile(RX_DST, RX_NEW_DATA, sizeof(RX_NEW_DATA));
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(1);
    EXPECT_EQ(seqB, this->eventHistory_RxFileTransferCompleted->at(0).seqNum);

    (void)Os::FileSystem::removeFile(tmpA.toChar());
    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingNotKeptReceiveRemovesOnlyOwnedFile() {
    const TransactionSeq seq = 7040;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // A receive that is not kept (keep is DELETE until its checksum passes) is disposed of through
    // Engine::handleNotKeepFile. That removes the staging file the receive created, never the
    // existing destination named by dst_filename.
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    ASSERT_EQ(Cfdp::Keep::DELETE, txn->m_keep);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));

    this->component.m_engine->handleNotKeepFile(txn);
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingExpectNoFile(paths.staging.toChar());
    ASSERT_EVENTS_FileRemoveFailed_SIZE(0);

    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));
    this->rxStagingExpectKept(paths);
    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1FlushFailureNotCommitted() {
    const TransactionSeq seq = 7041;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // Data that cannot be flushed to stable storage is not committed (white box: the transaction's
    // write handle is replaced by a read-only one, so the flush before the rename fails)
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    txn->m_fd.close();
    ASSERT_EQ(Os::File::OP_OK, txn->m_fd.open(paths.staging.toChar(), Os::File::OPEN_READ));
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_1);

    ASSERT_EVENTS_RxFileRenameFailed_SIZE(1);
    EXPECT_STREQ(paths.staging.toChar(), this->eventHistory_RxFileRenameFailed->at(0).tempFile.toChar());
    EXPECT_STREQ(RX_DST, this->eventHistory_RxFileRenameFailed->at(0).finalFile.toChar());
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILESTORE_REJECTION);
    this->rxStagingExpectKept(paths);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2PausedChecksumPassCommits() {
    const TransactionSeq seq = 7042;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);
    const U32 bytesPerCycle = this->component.getRxCrcCalcBytesPerCycleParam();

    // With RxCrcCalcBytesPerCycle at 0 the checksum pass reads nothing, so it starts over every cycle on
    // the handle it already flushed and reopened read-only. That must not fail the transfer.
    this->paramSet_RxCrcCalcBytesPerCycle(0, Fw::ParamValid::VALID);
    this->paramSend_RxCrcCalcBytesPerCycle(0, 0);
    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_CHUNK_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    this->rxStagingSendGoodEof(seq, Cfdp::Class::CLASS_2);
    for (U32 cycle = 0; cycle < 4; ++cycle) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }
    // The pass has started and is paused: the staging file is open for reading and nothing has been read
    Transaction* txn = this->findTransaction(RX_CHANNEL, seq);
    ASSERT_NE(nullptr, txn);
    ASSERT_TRUE(txn->m_fd.isOpen());
    ASSERT_EQ(0u, txn->m_state_data.receive.r2.rx_crc_calc_bytes);
    U8 probe = 0;
    FwSizeType probeSize = sizeof(probe);
    EXPECT_NE(Os::File::OP_OK, txn->m_fd.write(&probe, probeSize, Os::File::WAIT)) << "handle should be read-only";
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(0);
    Cfdp::ChannelTelemetry tlm;
    this->rxStagingTelemetry(tlm);
    EXPECT_EQ(0u, tlm.get_faultFileWrite());

    // Once the pass reads again, the transfer is verified and committed
    this->paramSet_RxCrcCalcBytesPerCycle(bytesPerCycle, Fw::ParamValid::VALID);
    this->paramSend_RxCrcCalcBytesPerCycle(0, 0);
    FinFileStatus finStatus = FinFileStatus::FIN_FILE_STATUS_DISCARDED;
    this->rxStagingRunClass2ToFinAck(seq, &finStatus);
    EXPECT_EQ(FinFileStatus::FIN_FILE_STATUS_RETAINED, finStatus);
    this->rxStagingExpectCommitted(paths, RX_NEW_DATA, sizeof(RX_NEW_DATA));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1AbandonKeepsExistingFile() {
    const TransactionSeq seq = 7027;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    ASSERT_EQ(Cfdp::Status::SUCCESS, this->component.m_engine->abandonTransaction(RX_CHANNEL, seq, TEST_GROUND_EID));

    // The files are handled like any other unfinished receive; the reported outcome is not asserted
    // here (see the SDD: AbandonTransaction records no error status)
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));
    this->rxStagingExpectNoFile(paths.staging.toChar());

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingTeardownRemovesInProgressFile() {
    const TransactionSeq seq = 7028;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_1, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    this->rxStagingExpectFile(paths.staging.toChar(), RX_NEW_DATA, sizeof(RX_NEW_DATA));

    // Tearing the component down removes the unfinished receive's file without emitting events
    this->clearEvents();
    this->component.cleanup();
    ASSERT_EVENTS_SIZE(0);
    this->rxStagingExpectNoFile(paths.staging.toChar());
    this->rxStagingExpectFile(RX_DST, RX_ORIGINAL, sizeof(RX_ORIGINAL));

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1PartialOverlapFailsChecksum() {
    const TransactionSeq seq = 7029;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, true, paths);

    // The second segment partly overlaps the first, so its overlapping bytes are digested twice
    U8 payload[2 * RX_CHUNK_SIZE] = {};
    for (FwSizeType i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(0x40 + i);
    }
    const FileSize offsets[] = {0, RX_CHUNK_SIZE / 2, RX_CHUNK_SIZE + (RX_CHUNK_SIZE / 2)};
    const U16 sizes[] = {RX_CHUNK_SIZE, RX_CHUNK_SIZE, RX_CHUNK_SIZE / 2};
    this->rxStagingClass1Segments(seq, payload, sizeof(payload), offsets, sizes, 3);

    this->rxStagingExpectKept(paths);
    ASSERT_EVENTS_RxCrcMismatch_SIZE(1);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_FILE_CHECKSUM_FAILURE);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass2InactivityKeepsExistingFile() {
    const TransactionSeq seq = 7030;
    RxStagingPaths paths;
    // An inactivity timer shorter than the NAK limit, so inactivity ends the receive
    this->rxStagingBegin(seq, true, paths, 3);

    this->rxStagingSendMetadata(seq, Cfdp::Class::CLASS_2, RX_DECLARED_SIZE);
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_2);
    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));

    this->rxStagingExpectKept(paths);
    ASSERT_EVENTS_RxInactivityTimeout_SIZE(1);
    this->rxStagingExpectFailed(Cfdp::ConditionCode::CONDITION_CODE_INACTIVITY_DETECTED);

    this->rxStagingCleanup(paths);
}

void CfdpManagerTester::testRxStagingClass1DroppedTransactionNotReported() {
    const TransactionSeq seq = 7031;
    RxStagingPaths paths;
    this->rxStagingBegin(seq, false, paths);

    // Class 1 File Data without Metadata cannot start a receive; it is dropped until it times out
    this->rxStagingSendData(seq, Cfdp::Class::CLASS_1);
    ASSERT_TRUE(this->rxStagingRunUntilFreed(seq));

    this->rxStagingExpectNoFile(RX_DST);
    this->rxStagingExpectNoFile(paths.staging.toChar());
    this->rxStagingExpectNoFile(paths.tmp.toChar());
    ASSERT_EVENTS_RxFileTransferCompleted_SIZE(0);
    ASSERT_EVENTS_RxFileTransferFailed_SIZE(0);

    this->rxStagingCleanup(paths);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
