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

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
