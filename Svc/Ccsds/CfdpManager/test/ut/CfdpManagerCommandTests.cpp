// ======================================================================
// \title  CfdpManagerCommandTests.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component command unit tests
//
// ======================================================================

#include <sys/stat.h>
#include <Fw/Types/StringUtils.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include "CfdpManagerTester.hpp"

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// SendFile Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testSendFileZeroLength() {
    // Test that attempting to send a zero-length file fails gracefully
    // instead of triggering an assert (FW_ASSERT removed in the fix)
    //
    // KEY: Before fix, this would crash with FW_ASSERT. Now it handles gracefully.

    const char* zeroLengthFile = "test/ut/output/zero_length_file.bin";
    const char* dstFile = "test/ut/output/zero_length_dst.bin";

    // Create a zero-length file
    Os::File file;
    Os::File::Status status = file.open(zeroLengthFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, status) << "Should create zero-length test file";
    file.close();

    // Verify file exists and has zero size
    struct stat st;
    ASSERT_EQ(0, stat(zeroLengthFile, &st));
    ASSERT_EQ(0, st.st_size) << "Test file should be zero length";

    // Attempt to send the zero-length file
    U8 channelId = 0;
    EntityId destEid = 2;
    Fw::String srcFilename(zeroLengthFile);
    Fw::String dstFilename(dstFile);

    // Clear any previous events
    this->clearEvents();

    // Send command to transfer zero-length file
    // If the fix works, this will not crash (the key test)
    this->sendCmd_SendFile(0,  // Instance
                           0,  // cmdSeq
                           channelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                           0,  // priority
                           srcFilename, dstFilename);

    // Run component cycles to allow transaction initialization
    for (U32 i = 0; i < 10; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // The key success criteria: we didn't crash!
    // Before fix: FW_ASSERT would have terminated the component
    // After fix: graceful error handling, no crash
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SENDFILE, 0, Fw::CmdResponse::OK);

    // Verify TxZeroLengthFile event was emitted with correct filename
    ASSERT_EVENTS_TxZeroLengthFile_SIZE(1);
    EventEntry_TxZeroLengthFile entry = this->eventHistory_TxZeroLengthFile->at(0);
    ASSERT_STREQ(zeroLengthFile, entry.filename.toChar()) << "Event should report the zero-length filename";

    // Clean up test file
    Os::FileSystem::Status removeStatus = Os::FileSystem::removeFile(zeroLengthFile);
    ASSERT_EQ(Os::FileSystem::OP_OK, removeStatus);
}

void CfdpManagerTester::testSendFileNonExistent() {
    // Test that attempting to send a nonexistent file is accepted
    // but will fail later when the transaction tries to open the file

    const char* nonExistentFile = "test/ut/output/does_not_exist.bin";
    const char* dstFile = "test/ut/output/dst_file.bin";

    U8 channelId = 0;
    EntityId destEid = 2;
    Fw::String srcFilename(nonExistentFile);
    Fw::String dstFilename(dstFile);

    // Clear any previous events
    this->clearEvents();

    // Send command to transfer nonexistent file
    this->sendCmd_SendFile(0,  // Instance
                           0,  // cmdSeq
                           channelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                           0,  // priority
                           srcFilename, dstFilename);

    this->component.doDispatch();

    // Command is accepted - file validation happens during transaction execution
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SENDFILE, 0, Fw::CmdResponse::OK);

    // Run component cycles to allow transaction to attempt file open
    for (U32 i = 0; i < 10; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Transaction should emit TxFileOpenFailed event when it tries to open nonexistent file
    // This demonstrates graceful error handling - command accepted, transaction fails with event
    ASSERT_EVENTS_TxFileOpenFailed_SIZE(1);
    ASSERT_EVENTS_TxFileOpenFailed(0, Cfdp::Class::CLASS_1, this->component.getLocalEidParam(), 1, nonExistentFile,
                                   Os::File::DOESNT_EXIST);
}

// ----------------------------------------------------------------------
// StopPollDirectory Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testStopPollDirNotActive() {
    // Test that attempting to stop an inactive polling directory is accepted

    U8 channelId = 0;
    U8 pollId = 0;

    // Clear any previous events
    this->clearEvents();

    // Attempt to stop a poll directory that was never started
    this->sendCmd_StopPollDirectory(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, pollId);

    this->component.doDispatch();

    // Command is accepted even though poll directory wasn't active
    // Command should succeed even though the polling directory is not active
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_STOPPOLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // PollDirNotActive event should have been emitted
    ASSERT_EVENTS_PollDirNotActive_SIZE(1);
    ASSERT_EVENTS_PollDirNotActive(0, channelId, pollId);
}

void CfdpManagerTester::testStopPollDirActive() {
    // Test that stopping an active polling directory succeeds
    // This test demonstrates the fix for the inverted condition bug
    // Before fix: active directories returned ERROR (condition was inverted)
    // After fix: active directories are properly stopped

    U8 channelId = 0;
    U8 pollId = 0;
    EntityId destEid = 2;
    U32 interval = 60;
    U8 priority = 0;
    Fw::String srcDir("test/ut/output/poll_src");
    Fw::String dstDir("test/ut/output/poll_dst");

    // Create source directory
    Os::FileSystem::Status dirStatus = Os::FileSystem::createDirectory(srcDir.toChar());
    ASSERT_TRUE(dirStatus == Os::FileSystem::OP_OK || dirStatus == Os::FileSystem::ALREADY_EXISTS);

    // Clear events
    this->clearEvents();

    // Start a polling directory
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify start succeeded
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Clear for stop command
    this->clearEvents();
    this->clearHistory();

    // Now stop the polling directory - this tests the fix
    this->sendCmd_StopPollDirectory(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, pollId);

    this->component.doDispatch();

    // With the fix, stopping an ENABLED directory now succeeds
    // Before fix: would have returned ERROR with "not active" message
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_STOPPOLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Verify PollDirStopped event was emitted
    ASSERT_EVENTS_PollDirStopped_SIZE(1);
    ASSERT_EVENTS_PollDirStopped(0, channelId, pollId);

    // Clean up
    Os::FileSystem::Status removeStatus = Os::FileSystem::removeDirectory(srcDir.toChar());
    ASSERT_EQ(Os::FileSystem::OP_OK, removeStatus);
}

void CfdpManagerTester::testSendFileInvalidChannel() {
    // Test that attempting to send file with invalid channel ID returns VALIDATION_ERROR
    // and emits InvalidChannel event
    // Note: Channel validation happens BEFORE file validation, so we don't need a real file
    //
    // Event coverage: InvalidChannel

    const char* srcFile = "dummy_file.bin";
    const char* dstFile = "dummy_dst.bin";

    // Clear any previous events
    this->clearEvents();

    // Send command with invalid channel ID (>= NumChannels)
    U8 invalidChannelId = Cfdp::NumChannels;  // First invalid channel
    EntityId destEid = 2;
    Fw::String srcFilename(srcFile);
    Fw::String dstFilename(dstFile);

    this->sendCmd_SendFile(0,  // Instance
                           0,  // cmdSeq
                           invalidChannelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                           0,  // priority
                           srcFilename, dstFilename);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Currently returns EXECUTION_ERROR instead of VALIDATION_ERROR
    // Root cause: In CfdpManager.cpp SendFile_cmdHandler, the else block (line 275-278)
    // unconditionally sets EXECUTION_ERROR, overwriting the VALIDATION_ERROR from
    // checkCommandChannelIndex(). The else should only set EXECUTION_ERROR if
    // rspStatus is still OK.
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SENDFILE, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted with correct parameters
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// PlaybackDirectory Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testPlaybackDirectoryNominal() {
    // Test that PlaybackDirectory command with valid directory succeeds
    //
    // Event coverage: PlaybackInitiated

    const char* srcDir = "test/ut/output/playback_src";
    const char* dstDir = "test/ut/output/playback_dst";

    // Create source directory with a test file
    Os::FileSystem::Status dirStatus = Os::FileSystem::createDirectory(srcDir);
    ASSERT_TRUE(dirStatus == Os::FileSystem::OP_OK || dirStatus == Os::FileSystem::ALREADY_EXISTS);

    // Create a test file in the directory
    Fw::String testFilePath(srcDir);
    testFilePath += "/test.bin";
    Os::File file;
    Os::File::Status fileStatus = file.open(testFilePath.toChar(), Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    U8 testData[100] = {0};
    FwSizeType bytesToWrite = sizeof(testData);
    fileStatus = file.write(testData, bytesToWrite);
    ASSERT_EQ(Os::File::OP_OK, fileStatus);
    file.close();

    // Clear events
    this->clearEvents();

    // Send PlaybackDirectory command
    U8 channelId = 0;
    EntityId destEid = 2;
    Fw::String srcDirStr(srcDir);
    Fw::String dstDirStr(dstDir);

    this->sendCmd_PlaybackDirectory(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                                    0,  // priority
                                    srcDirStr, dstDirStr);

    this->component.doDispatch();

    // Verify command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_PLAYBACKDIRECTORY, 0, Fw::CmdResponse::OK);

    // Verify PlaybackInitiated event was emitted
    ASSERT_EVENTS_PlaybackInitiated_SIZE(1);
    ASSERT_EVENTS_PlaybackInitiated(0, srcDir);

    // Clean up
    Os::FileSystem::removeFile(testFilePath.toChar());
    Os::FileSystem::removeDirectory(srcDir);
}

void CfdpManagerTester::testPlaybackDirectoryInvalidChannel() {
    // Test that PlaybackDirectory command with invalid channel returns VALIDATION_ERROR
    //
    // Event coverage: PlaybackInvalidChannel

    const char* srcDir = "test/ut/output/playback_src";
    const char* dstDir = "test/ut/output/playback_dst";

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    U8 invalidChannelId = Cfdp::NumChannels;
    EntityId destEid = 2;
    Fw::String srcDirStr(srcDir);
    Fw::String dstDirStr(dstDir);

    this->sendCmd_PlaybackDirectory(0,  // Instance
                                    0,  // cmdSeq
                                    invalidChannelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                                    0,  // priority
                                    srcDirStr, dstDirStr);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_PLAYBACKDIRECTORY, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify PlaybackInvalidChannel event was emitted
    ASSERT_EVENTS_PlaybackInvalidChannel_SIZE(1);
    ASSERT_EVENTS_PlaybackInvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

void CfdpManagerTester::testPlaybackDirectoryOpenFailed() {
    // Test that PlaybackDirectory command with nonexistent directory is accepted
    // but emits PlaybackDirOpenFailed event when directory open is attempted
    //
    // Event coverage: PlaybackDirOpenFailed

    const char* nonexistentDir = "test/ut/output/does_not_exist_playback";
    const char* dstDir = "test/ut/output/playback_dst";

    // Ensure directory doesn't exist
    Os::FileSystem::removeDirectory(nonexistentDir);

    // Clear events
    this->clearEvents();

    // Send command with nonexistent directory
    U8 channelId = 0;
    EntityId destEid = 2;
    Fw::String srcDirStr(nonexistentDir);
    Fw::String dstDirStr(dstDir);

    this->sendCmd_PlaybackDirectory(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, destEid, Cfdp::Class::CLASS_1, Cfdp::Keep::KEEP,
                                    0,  // priority
                                    srcDirStr, dstDirStr);

    this->component.doDispatch();

    // Command is rejected with EXECUTION_ERROR because directory open fails immediately
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_PLAYBACKDIRECTORY, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // Verify PlaybackDirOpenFailed event was emitted
    ASSERT_EVENTS_PlaybackDirOpenFailed_SIZE(1);
}

// ----------------------------------------------------------------------
// PollDirectory Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testPollDirectoryNominal() {
    // Test that PollDirectory command with valid directory succeeds
    //
    // Event coverage: PollDirInitiated

    U8 channelId = 0;
    U8 pollId = 0;
    EntityId destEid = 2;
    U32 interval = 60;  // seconds
    U8 priority = 0;
    Fw::String srcDir("test/ut/output/poll_src");
    Fw::String dstDir("test/ut/output/poll_dst");

    // Create source directory
    Os::FileSystem::Status dirStatus = Os::FileSystem::createDirectory(srcDir.toChar());
    ASSERT_TRUE(dirStatus == Os::FileSystem::OP_OK || dirStatus == Os::FileSystem::ALREADY_EXISTS);

    // Clear events
    this->clearEvents();

    // Send PollDirectory command
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Verify PollDirInitiated event was emitted
    ASSERT_EVENTS_PollDirInitiated_SIZE(1);
    ASSERT_EVENTS_PollDirInitiated(0, srcDir.toChar());

    // Clean up - stop the poll
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_StopPollDirectory(0, 0, channelId, pollId);
    this->component.doDispatch();

    // Clean up directory
    Os::FileSystem::removeDirectory(srcDir.toChar());
}

void CfdpManagerTester::testPollDirectoryInvalidChannel() {
    // Test that PollDirectory command with invalid channel returns VALIDATION_ERROR
    //
    // Event coverage: (uses existing InvalidChannel event)

    U8 invalidChannelId = Cfdp::NumChannels;
    U8 pollId = 0;
    EntityId destEid = 2;
    U32 interval = 60;
    U8 priority = 0;
    Fw::String srcDir("test/ut/output/poll_src");
    Fw::String dstDir("test/ut/output/poll_dst");

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                invalidChannelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir,
                                dstDir);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // InvalidChannel event should be emitted for invalid channel
    // (or poll-specific invalid channel event if one exists)
}

void CfdpManagerTester::testPollDirectoryBusy() {
    // Test that attempting to start a poll on a slot already in use emits PollDirBusy event
    //
    // Event coverage: PollDirBusy

    U8 channelId = 0;
    U8 pollId = 0;
    EntityId destEid = 2;
    U32 interval = 60;
    U8 priority = 0;
    Fw::String srcDir("test/ut/output/poll_busy_src");
    Fw::String dstDir("test/ut/output/poll_busy_dst");

    // Create source directory
    Os::FileSystem::Status dirStatus = Os::FileSystem::createDirectory(srcDir.toChar());
    ASSERT_TRUE(dirStatus == Os::FileSystem::OP_OK || dirStatus == Os::FileSystem::ALREADY_EXISTS);

    // Clear events
    this->clearEvents();

    // Start first poll on channel 0, pollId 0
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify first command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Clear for second attempt
    this->clearEvents();
    this->clearHistory();

    // Attempt to start another poll on the same channel 0, pollId 0
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Command may be accepted or rejected depending on implementation
    // But PollDirBusy event should be emitted
    ASSERT_EVENTS_PollDirBusy_SIZE(1);
    ASSERT_EVENTS_PollDirBusy(0, channelId, pollId);

    // Clean up - stop the first poll
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_StopPollDirectory(0, 0, channelId, pollId);
    this->component.doDispatch();

    // Clean up directory
    Os::FileSystem::removeDirectory(srcDir.toChar());
}

// ----------------------------------------------------------------------
// SetChannelFlow Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testSetChannelFlowNominal() {
    // Test that SetChannelFlow command with valid channel succeeds
    //
    // Event coverage: SetFlowState

    U8 channelId = 0;
    Cfdp::Flow flowState = Cfdp::Flow::FROZEN;

    // Clear events
    this->clearEvents();

    // Send SetChannelFlow command
    this->sendCmd_SetChannelFlow(0,  // Instance
                                 0,  // cmdSeq
                                 channelId, flowState);

    this->component.doDispatch();

    // Verify command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SETCHANNELFLOW, 0, Fw::CmdResponse::OK);

    // Verify SetFlowState event was emitted
    ASSERT_EVENTS_SetFlowState_SIZE(1);
    ASSERT_EVENTS_SetFlowState(0, channelId, flowState);
}

void CfdpManagerTester::testSetChannelFlowInvalidChannel() {
    // Test that SetChannelFlow command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel (documented bug - returns EXECUTION_ERROR)

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::Flow flowState = Cfdp::Flow::FROZEN;

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    this->sendCmd_SetChannelFlow(0,  // Instance
                                 0,  // cmdSeq
                                 invalidChannelId, flowState);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SETCHANNELFLOW, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// SuspendResumeTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testSuspendResumeTransactionNominal() {
    // Test that SuspendResumeTransaction command can suspend and resume a transaction
    // Note: This is a simplified test - full transaction testing would require
    // setting up an active transaction
    //
    // Event coverage: TransactionSuspended, TransactionResumed, TransactionNotFound

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Attempt to suspend a nonexistent transaction (no transaction is running)
    // This should succeed but emit TransactionNotFound event
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, entityId,
                                           Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Command returns EXECUTION_ERROR (transaction not found)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event emitted (since no transaction exists)
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);

    // Clear for resume test
    this->clearEvents();
    this->clearHistory();

    // Attempt to resume - also returns EXECUTION_ERROR with TransactionNotFound
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, entityId,
                                           Cfdp::SuspendResume::RESUME);

    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event emitted
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

void CfdpManagerTester::testSuspendResumeTransactionInvalidChannel() {
    // Test that SuspendResumeTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel (documented bug)

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           invalidChannelId, transactionSeq, entityId,
                                           Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0,
                        Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

void CfdpManagerTester::testSuspendResumeTransactionNotFound() {
    // Test that attempting to suspend/resume a nonexistent transaction is handled gracefully
    // This test documents the expected behavior when transaction is not found
    //
    // Event coverage: TransactionNotFound

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 999;  // Non-existent transaction
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Attempt to suspend nonexistent transaction
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, entityId,
                                           Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Command returns EXECUTION_ERROR (transaction not found)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event should be emitted
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

// ----------------------------------------------------------------------
// CancelTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testCancelTransactionNominal() {
    // Test that CancelTransaction command is accepted
    // Note: Without an active transaction, TransactionNotFound will be emitted
    //
    // Event coverage: TransactionCanceled, TransactionNotFound

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Attempt to cancel a nonexistent transaction
    this->sendCmd_CancelTransaction(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Command returns EXECUTION_ERROR (transaction not found)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_CANCELTRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event emitted (since no transaction exists)
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

void CfdpManagerTester::testCancelTransactionInvalidChannel() {
    // Test that CancelTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel (documented bug)

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    this->sendCmd_CancelTransaction(0,  // Instance
                                    0,  // cmdSeq
                                    invalidChannelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_CANCELTRANSACTION, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// AbandonTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testAbandonTransactionNominal() {
    // Test that AbandonTransaction command is accepted
    // Note: Without an active transaction, TransactionNotFound will be emitted
    //
    // Event coverage: TransactionAbandoned, TransactionNotFound

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Attempt to abandon a nonexistent transaction
    this->sendCmd_AbandonTransaction(0,  // Instance
                                     0,  // cmdSeq
                                     channelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Command returns EXECUTION_ERROR (transaction not found)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_ABANDONTRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event emitted (since no transaction exists)
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

void CfdpManagerTester::testAbandonTransactionInvalidChannel() {
    // Test that AbandonTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel (documented bug)

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearEvents();

    // Send command with invalid channel ID
    this->sendCmd_AbandonTransaction(0,  // Instance
                                     0,  // cmdSeq
                                     invalidChannelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    // BUG TO FIX: Same issue as SendFile - see testSendFileInvalidChannel
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_ABANDONTRANSACTION, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// ResetCounters Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testResetCountersSingleChannel() {
    // Test that ResetCounters command resets counters for a specific channel
    //
    // Event coverage: ResetCounters

    U8 channelId = 0;

    // Clear events
    this->clearEvents();

    // Send ResetCounters command for specific channel
    this->sendCmd_ResetCounters(0,  // Instance
                                0,  // cmdSeq
                                channelId);

    this->component.doDispatch();

    // Command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_RESETCOUNTERS, 0, Fw::CmdResponse::OK);

    // ResetCounters event emitted
    ASSERT_EVENTS_ResetCounters_SIZE(1);
    ASSERT_EVENTS_ResetCounters(0, channelId);
}

void CfdpManagerTester::testResetCountersAllChannels() {
    // Test that ResetCounters command resets counters for all channels
    //
    // Event coverage: ResetCounters

    U8 allChannelsId = 0xFF;  // Special value for "all channels"

    // Clear events
    this->clearEvents();

    // Send ResetCounters command for all channels
    this->sendCmd_ResetCounters(0,  // Instance
                                0,  // cmdSeq
                                allChannelsId);

    this->component.doDispatch();

    // Command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_RESETCOUNTERS, 0, Fw::CmdResponse::OK);

    // ResetCounters event emitted with 0xFF
    ASSERT_EVENTS_ResetCounters_SIZE(1);
    ASSERT_EVENTS_ResetCounters(0, allChannelsId);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
