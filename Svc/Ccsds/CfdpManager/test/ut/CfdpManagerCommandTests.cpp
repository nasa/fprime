// ======================================================================
// \title  CfdpManagerCommandTests.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component command unit tests
//
// ======================================================================

#include <sys/stat.h>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Svc/Ccsds/CfdpManager/Utils.hpp>
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
    this->clearHistory();

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
    // Total event count intentionally not pinned: running run1Hz cycles drives the
    // transaction state machine, which emits additional state-dependent events.
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
    this->clearHistory();

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
    // Total event count intentionally not pinned: running run1Hz cycles drives the
    // transaction state machine, which emits additional state-dependent events.
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
    this->clearHistory();

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
    ASSERT_EVENTS_SIZE(1);
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
    this->clearHistory();

    // Start a polling directory
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify start succeeded
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Clear for stop command
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
    ASSERT_EVENTS_SIZE(1);
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
    this->clearHistory();

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
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SENDFILE, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted with correct parameters
    ASSERT_EVENTS_SIZE(1);
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
    this->clearHistory();

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
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PlaybackInitiated_SIZE(1);
    ASSERT_EVENTS_PlaybackInitiated(0, srcDir);

    // Clean up
    Os::FileSystem::removeFile(testFilePath.toChar());
    Os::FileSystem::removeDirectory(srcDir);
}

void CfdpManagerTester::testPlaybackDirectoryInvalidChannel() {
    // Test that PlaybackDirectory command with invalid channel returns VALIDATION_ERROR
    //
    // Event coverage: InvalidChannel

    const char* srcDir = "test/ut/output/playback_src";
    const char* dstDir = "test/ut/output/playback_dst";

    // Clear events
    this->clearHistory();

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
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_PLAYBACKDIRECTORY, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
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
    this->clearHistory();

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
    ASSERT_EVENTS_SIZE(1);
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
    this->clearHistory();

    // Send PollDirectory command
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Verify PollDirInitiated event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PollDirInitiated_SIZE(1);
    ASSERT_EVENTS_PollDirInitiated(0, srcDir.toChar());

    // Clean up - stop the poll
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
    this->clearHistory();

    // Send command with invalid channel ID
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                invalidChannelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir,
                                dstDir);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // InvalidChannel event should be emitted for invalid channel
    // (or poll-specific invalid channel event if one exists)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
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
    this->clearHistory();

    // Start first poll on channel 0, pollId 0
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Verify first command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_POLLDIRECTORY, 0, Fw::CmdResponse::OK);

    // Clear for second attempt
    this->clearHistory();

    // Attempt to start another poll on the same channel 0, pollId 0
    this->sendCmd_PollDirectory(0,  // Instance
                                0,  // cmdSeq
                                channelId, pollId, destEid, Cfdp::Class::CLASS_1, priority, interval, srcDir, dstDir);

    this->component.doDispatch();

    // Command may be accepted or rejected depending on implementation
    // But PollDirBusy event should be emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PollDirBusy_SIZE(1);
    ASSERT_EVENTS_PollDirBusy(0, channelId, pollId);

    // Clean up - stop the first poll
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
    this->clearHistory();

    // Send SetChannelFlow command
    this->sendCmd_SetChannelFlow(0,  // Instance
                                 0,  // cmdSeq
                                 channelId, flowState);

    this->component.doDispatch();

    // Verify command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SETCHANNELFLOW, 0, Fw::CmdResponse::OK);

    // Verify SetFlowState event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_SetFlowState_SIZE(1);
    ASSERT_EVENTS_SetFlowState(0, channelId, flowState);
}

void CfdpManagerTester::testSetChannelFlowInvalidChannel() {
    // Test that SetChannelFlow command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::Flow flowState = Cfdp::Flow::FROZEN;

    // Clear events
    this->clearHistory();

    // Send command with invalid channel ID
    this->sendCmd_SetChannelFlow(0,  // Instance
                                 0,  // cmdSeq
                                 invalidChannelId, flowState);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SETCHANNELFLOW, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// SuspendResumeTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testSuspendResumeTransactionNominal() {
    // Test that SuspendResumeTransaction command can suspend and resume a transaction
    //
    // Event coverage: TransactionSuspended, TransactionResumed

    const char* srcFile = "test/ut/output/suspend_resume_test.bin";
    const char* destFile = "/dest/suspend_resume.bin";
    U8 channelId = 0;
    EntityId destEid = TEST_GROUND_EID;

    // Create source file for transaction
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    U8 testData[100];
    for (U8 i = 0; i < 100; i++) {
        testData[i] = i;
    }
    FwSizeType sizeToWrite = 100;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearHistory();

    // Start a file transfer to create an active transaction
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, channelId, destEid, Cfdp::Class::CLASS_2, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Run cycles to start transaction (but not complete it)
    for (U32 i = 0; i < 3; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Get the transaction sequence number from the component's local entity ID
    Cfdp::EntityId localEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1;  // First transaction

    // Clear events before suspend command
    this->clearHistory();

    // Suspend the active transaction
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, localEid, Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Command should succeed
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0, Fw::CmdResponse::OK);

    // TransactionSuspended event emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionSuspended_SIZE(1);

    // Clear for resume test
    this->clearHistory();

    // Resume the suspended transaction
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, localEid, Cfdp::SuspendResume::RESUME);

    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0, Fw::CmdResponse::OK);

    // TransactionResumed event emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionResumed_SIZE(1);

    // Clean up test file
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testSuspendResumeTransactionInvalidChannel() {
    // Test that SuspendResumeTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearHistory();

    // Send command with invalid channel ID
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           invalidChannelId, transactionSeq, entityId, Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0,
                        Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

void CfdpManagerTester::testSuspendResumeTransactionNotFound() {
    // Test that attempting to suspend/resume a nonexistent transaction is handled gracefully
    // This test documents the expected behavior when transaction is not found
    //
    // Event coverage: TransactionNotFound

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 999;  // Nonexistent transaction
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearHistory();

    // Attempt to suspend nonexistent transaction
    this->sendCmd_SuspendResumeTransaction(0,  // Instance
                                           0,  // cmdSeq
                                           channelId, transactionSeq, entityId, Cfdp::SuspendResume::SUSPEND);

    this->component.doDispatch();

    // Command returns EXECUTION_ERROR (transaction not found)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_SUSPENDRESUMETRANSACTION, 0,
                        Fw::CmdResponse::EXECUTION_ERROR);

    // TransactionNotFound event should be emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

// ----------------------------------------------------------------------
// CancelTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testCancelTransactionNominal() {
    // Test that CancelTransaction command cancels an active transaction
    //
    // Event coverage: TransactionCanceled

    const char* srcFile = "test/ut/output/cancel_test.bin";
    const char* destFile = "/dest/cancel.bin";
    U8 channelId = 0;
    EntityId destEid = TEST_GROUND_EID;

    // Create source file for transaction
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    U8 testData[100];
    for (U8 i = 0; i < 100; i++) {
        testData[i] = i;
    }
    FwSizeType sizeToWrite = 100;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearHistory();

    // Start a file transfer to create an active transaction
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, channelId, destEid, Cfdp::Class::CLASS_2, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Run cycles to start transaction
    for (U32 i = 0; i < 3; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Get transaction information
    Cfdp::EntityId localEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1;

    // Clear events before cancel command
    this->clearHistory();

    // Cancel the active transaction
    this->sendCmd_CancelTransaction(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, transactionSeq, localEid);

    this->component.doDispatch();

    // Command should succeed
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_CANCELTRANSACTION, 0, Fw::CmdResponse::OK);

    // TransactionCanceled event emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionCanceled_SIZE(1);

    // Clean up test file
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testCancelTransactionInvalidChannel() {
    // Test that CancelTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearHistory();

    // Send command with invalid channel ID
    this->sendCmd_CancelTransaction(0,  // Instance
                                    0,  // cmdSeq
                                    invalidChannelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_CANCELTRANSACTION, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels);
}

// ----------------------------------------------------------------------
// AbandonTransaction Command Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testAbandonTransactionNominal() {
    // Test that AbandonTransaction command abandons an active transaction
    //
    // Event coverage: TransactionAbandoned

    const char* srcFile = "test/ut/output/abandon_test.bin";
    const char* destFile = "/dest/abandon.bin";
    U8 channelId = 0;
    EntityId destEid = TEST_GROUND_EID;

    // Create source file for transaction
    Os::File file;
    file.open(srcFile, Os::File::OPEN_CREATE, Os::File::OVERWRITE);
    U8 testData[100];
    for (U8 i = 0; i < 100; i++) {
        testData[i] = i;
    }
    FwSizeType sizeToWrite = 100;
    file.write(testData, sizeToWrite);
    file.close();

    this->clearHistory();

    // Start a file transfer to create an active transaction
    Fw::String srcFileStr(srcFile);
    Fw::String destFileStr(destFile);
    this->sendCmd_SendFile(0, 0, channelId, destEid, Cfdp::Class::CLASS_2, Cfdp::Keep::DELETE, 0, srcFileStr,
                           destFileStr);
    this->component.doDispatch();

    // Run cycles to start transaction
    for (U32 i = 0; i < 3; i++) {
        this->invoke_to_run1Hz(0, 0);
        this->component.doDispatch();
    }

    // Get transaction information
    Cfdp::EntityId localEid = this->component.getLocalEidParam();
    Cfdp::TransactionSeq transactionSeq = 1;

    // Clear events before abandon command
    this->clearHistory();

    // Abandon the active transaction
    this->sendCmd_AbandonTransaction(0,  // Instance
                                     0,  // cmdSeq
                                     channelId, transactionSeq, localEid);

    this->component.doDispatch();

    // Command should succeed
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_ABANDONTRANSACTION, 0, Fw::CmdResponse::OK);

    // TransactionAbandoned event emitted
    // Total event count intentionally not pinned: abandoning an active transaction
    // cascades additional state-dependent transaction lifecycle events.
    ASSERT_EVENTS_TransactionAbandoned_SIZE(1);

    // Clean up test file
    Os::FileSystem::removeFile(srcFile);
}

void CfdpManagerTester::testAbandonTransactionInvalidChannel() {
    // Test that AbandonTransaction command with invalid channel returns validation error
    //
    // Event coverage: InvalidChannel

    U8 invalidChannelId = Cfdp::NumChannels;
    Cfdp::TransactionSeq transactionSeq = 1;
    Cfdp::EntityId entityId = 100;

    // Clear events
    this->clearHistory();

    // Send command with invalid channel ID
    this->sendCmd_AbandonTransaction(0,  // Instance
                                     0,  // cmdSeq
                                     invalidChannelId, transactionSeq, entityId);

    this->component.doDispatch();

    // Verify VALIDATION_ERROR response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_ABANDONTRANSACTION, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Verify InvalidChannel event was emitted
    ASSERT_EVENTS_SIZE(1);
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
    this->clearHistory();

    // Send ResetCounters command for specific channel
    this->sendCmd_ResetCounters(0,  // Instance
                                0,  // cmdSeq
                                channelId);

    this->component.doDispatch();

    // Command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_RESETCOUNTERS, 0, Fw::CmdResponse::OK);

    // ResetCounters event emitted
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ResetCounters_SIZE(1);
    ASSERT_EVENTS_ResetCounters(0, channelId);
}

void CfdpManagerTester::testResetCountersAllChannels() {
    // Test that ResetCounters command resets counters for all channels
    //
    // Event coverage: ResetCounters

    U8 allChannelsId = 0xFF;  // Special value for "all channels"

    // Clear events
    this->clearHistory();

    // Send ResetCounters command for all channels
    this->sendCmd_ResetCounters(0,  // Instance
                                0,  // cmdSeq
                                allChannelsId);

    this->component.doDispatch();

    // Command accepted
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_RESETCOUNTERS, 0, Fw::CmdResponse::OK);

    // ResetCounters event emitted with 0xFF
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ResetCounters_SIZE(1);
    ASSERT_EVENTS_ResetCounters(0, allChannelsId);
}

// ----------------------------------------------------------------------
// Port Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testDataReturnInChannel0() {
    // Test that dataReturnIn port can be invoked on channel 0
    // Note: This is a basic test that the port is wired correctly.

    this->clearHistory();

    // Create a buffer to return
    U8 testData[100] = {0};
    Fw::Buffer testBuffer(testData, sizeof(testData));

    // Invoke dataReturnIn port on channel 0
    this->invoke_to_dataReturnIn(0, testBuffer);
    this->component.doDispatch();

    // dataReturnIn_handler unconditionally forwards the buffer to
    // bufferDeallocate exactly once, on the matching channel port.
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_from_bufferDeallocate(0, testBuffer);
    ASSERT_EVENTS_SIZE(0);
}

void CfdpManagerTester::testDataReturnInChannel1() {
    // Test that dataReturnIn port can be invoked on channel 1
    // This verifies multi-channel port array indexing works correctly

    this->clearHistory();

    // Create a buffer to return
    U8 testData[100] = {0};
    Fw::Buffer testBuffer(testData, sizeof(testData));

    // Invoke dataReturnIn port on channel 1
    this->invoke_to_dataReturnIn(1, testBuffer);
    this->component.doDispatch();

    // dataReturnIn_handler unconditionally forwards the buffer to
    // bufferDeallocate exactly once, on the matching channel port.
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_from_bufferDeallocate(0, testBuffer);
    ASSERT_EVENTS_SIZE(0);
}

// ----------------------------------------------------------------------
// Coverage Tests
// ----------------------------------------------------------------------

void CfdpManagerTester::testDataInBufferTooSmall() {
    // dataIn_handler early-return: buffer smaller than the 2-byte packet descriptor.
    // The buffer is returned via dataInReturn and the engine is never invoked.

    this->clearHistory();

    U8 tinyData[1] = {0x00};
    Fw::Buffer tinyBuffer(tinyData, sizeof(tinyData));

    this->invoke_to_dataIn(0, tinyBuffer);
    this->component.doDispatch();

    // Buffer returned to the sender, no PDU processed, no events emitted
    ASSERT_from_dataInReturn_SIZE(1);
    ASSERT_EVENTS_SIZE(0);
}

void CfdpManagerTester::testDataInWrongDescriptor() {
    // dataIn_handler early-return: valid-size buffer whose leading descriptor is
    // not FW_PACKET_FILE. The buffer is returned and the engine is never invoked.

    this->clearHistory();

    // First two bytes are a packet descriptor != FW_PACKET_FILE
    const FwPacketDescriptorType wrongDescriptor =
        static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND);
    U8 data[8];
    data[0] = static_cast<U8>((wrongDescriptor >> 8) & 0xFF);
    data[1] = static_cast<U8>(wrongDescriptor & 0xFF);
    memset(&data[2], 0, sizeof(data) - 2);
    Fw::Buffer buffer(data, sizeof(data));

    this->invoke_to_dataIn(0, buffer);
    this->component.doDispatch();

    ASSERT_from_dataInReturn_SIZE(1);
    ASSERT_EVENTS_SIZE(0);
}

void CfdpManagerTester::testGetPduBufferMaxOutgoing() {
    // getPduBuffer: when the channel's outgoing PDU counter has reached the
    // per-cycle maximum, no buffer is allocated and SEND_PDU_NO_BUF_AVAIL_ERROR
    // is returned.

    this->clearHistory();

    Channel* chan = this->component.m_engine->m_channels[0];
    ASSERT_NE(nullptr, chan);

    // Drive the outgoing counter up to the configured per-cycle maximum
    U32 maxPdus = this->component.getMaxOutgoingPdusPerCycleParam(0);
    while (chan->getOutgoingCounter() < maxPdus) {
        chan->incrementOutgoingCounter();
    }

    this->clearHistory();

    Fw::Buffer buffer;
    Status::T status = this->component.getPduBuffer(buffer, *chan, 64);

    EXPECT_EQ(Status::SEND_PDU_NO_BUF_AVAIL_ERROR, status);
    EXPECT_EQ(0U, buffer.getSize());
    // No allocation should have been attempted
    ASSERT_from_bufferAllocate_SIZE(0);
}

void CfdpManagerTester::testCancelTransactionNotFound() {
    // CancelTransaction on a nonexistent transaction returns EXECUTION_ERROR and
    // emits TransactionNotFound.

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 999;  // Nonexistent transaction
    Cfdp::EntityId entityId = 100;

    this->clearHistory();

    this->sendCmd_CancelTransaction(0,  // Instance
                                    0,  // cmdSeq
                                    channelId, transactionSeq, entityId);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_CANCELTRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

void CfdpManagerTester::testAbandonTransactionNotFound() {
    // AbandonTransaction on a nonexistent transaction returns EXECUTION_ERROR and
    // emits TransactionNotFound.

    U8 channelId = 0;
    Cfdp::TransactionSeq transactionSeq = 999;  // Nonexistent transaction
    Cfdp::EntityId entityId = 100;

    this->clearHistory();

    this->sendCmd_AbandonTransaction(0,  // Instance
                                     0,  // cmdSeq
                                     channelId, transactionSeq, entityId);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_ABANDONTRANSACTION, 0, Fw::CmdResponse::EXECUTION_ERROR);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound_SIZE(1);
    ASSERT_EVENTS_TransactionNotFound(0, transactionSeq, entityId);
}

void CfdpManagerTester::testResetCountersInvalidChannel() {
    // ResetCounters with a channel that is neither 0xFF (all) nor in range returns
    // VALIDATION_ERROR and emits InvalidChannel (early return, no telemetry write).

    U8 invalidChannelId = Cfdp::NumChannels;  // First out-of-range channel

    this->clearHistory();

    this->sendCmd_ResetCounters(0,  // Instance
                                0,  // cmdSeq
                                invalidChannelId);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, CfdpManagerComponentBase::OPCODE_RESETCOUNTERS, 0, Fw::CmdResponse::VALIDATION_ERROR);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidChannel_SIZE(1);
    ASSERT_EVENTS_InvalidChannel(0, invalidChannelId, Cfdp::NumChannels - 1);
}

void CfdpManagerTester::testIncrementRecvDropped() {
    // Telemetry helper coverage: incrementRecvDropped bumps the per-channel counter.
    Cfdp::ChannelTelemetry& tlm = this->component.getChannelTelemetryRef(0);
    U32 before = tlm.get_recvDropped();

    this->component.incrementRecvDropped(0);

    EXPECT_EQ(before + 1, this->component.getChannelTelemetryRef(0).get_recvDropped());
}

void CfdpManagerTester::testIncrementSentEofCanceled() {
    // Telemetry helper coverage: incrementSentEofCanceled bumps the per-channel counter.
    Cfdp::ChannelTelemetry& tlm = this->component.getChannelTelemetryRef(0);
    U32 before = tlm.get_sentEofCanceled();

    this->component.incrementSentEofCanceled(0);

    EXPECT_EQ(before + 1, this->component.getChannelTelemetryRef(0).get_sentEofCanceled());
}

void CfdpManagerTester::testGetTxnStatusStates() {
    // GetTxnStatus maps transaction state to ACK transaction status.
    Transaction* txn = this->setupTestTransaction(TxnState::TXN_STATE_S2, 0, "src.bin", "dst.bin", 100, 1, 100);
    ASSERT_NE(nullptr, txn);

    // Active states
    const TxnState activeStates[] = {TxnState::TXN_STATE_S1, TxnState::TXN_STATE_R1, TxnState::TXN_STATE_S2,
                                     TxnState::TXN_STATE_R2};
    for (const auto& state : activeStates) {
        txn->m_state = state;
        EXPECT_EQ(AckTxnStatus::ACK_TXN_STATUS_ACTIVE, GetTxnStatus(txn))
            << "Expected ACTIVE for state " << static_cast<int>(state);
    }

    // Terminated states
    txn->m_state = TxnState::TXN_STATE_DROP;
    EXPECT_EQ(AckTxnStatus::ACK_TXN_STATUS_TERMINATED, GetTxnStatus(txn));
    txn->m_state = TxnState::TXN_STATE_HOLD;
    EXPECT_EQ(AckTxnStatus::ACK_TXN_STATUS_TERMINATED, GetTxnStatus(txn));

    // Any other state is INVALID (default arm)
    txn->m_state = TxnState::TXN_STATE_INIT;
    EXPECT_EQ(AckTxnStatus::ACK_TXN_STATUS_INVALID, GetTxnStatus(txn));
}

void CfdpManagerTester::testFindBySequenceNumberCallback() {
    // Transaction::findBySequenceNumberCallback matches on (src_eid, seq_num).
    Transaction* txn = this->setupTestTransaction(TxnState::TXN_STATE_S2, 0, "src.bin", "dst.bin", 100, 42, 100);
    ASSERT_NE(nullptr, txn);
    txn->m_history->src_eid = 5;
    txn->m_history->seq_num = 42;

    // Matching search terminates traversal (EXIT) and sets the output pointer
    CfdpTraverseTransSeqArg matchArg;
    matchArg.transaction_sequence_number = 42;
    matchArg.src_eid = 5;
    matchArg.txn = nullptr;
    EXPECT_EQ(CLIST_TRAVERSE_EXIT, Transaction::findBySequenceNumberCallback(&txn->m_cl_node, &matchArg));
    EXPECT_EQ(txn, matchArg.txn);

    // Non-matching search continues traversal and leaves the output pointer unchanged
    CfdpTraverseTransSeqArg missArg;
    missArg.transaction_sequence_number = 7;
    missArg.src_eid = 5;
    missArg.txn = nullptr;
    EXPECT_EQ(CLIST_TRAVERSE_CONTINUE, Transaction::findBySequenceNumberCallback(&txn->m_cl_node, &missArg));
    EXPECT_EQ(nullptr, missArg.txn);
}

void CfdpManagerTester::testPrioritySearchCallback() {
    // Transaction::prioritySearchCallback finds the first transaction whose priority
    // is <= the sought priority.
    Transaction* txn = this->setupTestTransaction(TxnState::TXN_STATE_S2, 0, "src.bin", "dst.bin", 100, 1, 100);
    ASSERT_NE(nullptr, txn);
    txn->m_priority = 3;

    // txn priority (3) <= sought (5): found -> EXIT
    CfdpTraversePriorityArg hitArg;
    hitArg.txn = nullptr;
    hitArg.priority = 5;
    EXPECT_EQ(CLIST_TRAVERSE_EXIT, Transaction::prioritySearchCallback(&txn->m_cl_node, &hitArg));
    EXPECT_EQ(txn, hitArg.txn);

    // txn priority (3) > sought (1): not found -> CONTINUE
    CfdpTraversePriorityArg missArg;
    missArg.txn = nullptr;
    missArg.priority = 1;
    EXPECT_EQ(CLIST_TRAVERSE_CONTINUE, Transaction::prioritySearchCallback(&txn->m_cl_node, &missArg));
    EXPECT_EQ(nullptr, missArg.txn);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
