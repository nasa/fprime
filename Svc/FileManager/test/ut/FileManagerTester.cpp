// ======================================================================
// \title  FileManagerTester.cpp
// \author bocchino
// \brief  cpp file for FileManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <fstream>

#include "FileManagerTester.hpp"
#include "Os/File.hpp"

#define INSTANCE 0
#define CMD_SEQ 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10
#define LOG_FILE "log.txt"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FileManagerTester ::FileManagerTester() : FileManagerGTestBase("Tester", MAX_HISTORY_SIZE), component("FileManager") {
    this->connectPorts();
    this->initComponents();
}

FileManagerTester ::~FileManagerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FileManagerTester ::createDirectorySucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");
    // Create test_dir
    this->createDirectory("test_dir");

#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Assert success
    this->assertSuccess(FileManager::OPCODE_CREATEDIRECTORY);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_dir exists
    this->system("test -d test_dir");

    // Clean up
    this->system("rmdir test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::createDirectoryFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Create test_dir
    this->system("rm -rf test_dir");
    this->system("mkdir test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to create test_dir (should fail)
    this->createDirectory("test_dir");

    // Assert failure
    this->assertFailure(FileManager::OPCODE_CREATEDIRECTORY);
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_DirectoryCreateError(0, "test_dir", Os::FileSystem::ALREADY_EXISTS);
}

void FileManagerTester ::moveFileSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove file1 and file2, if they exist
    this->system("rm -rf file1 file2");

    // Create file1
    this->system("touch file1");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Move file1 to file2
    this->moveFile("file1", "file2");

    // Assert success
    this->assertSuccess(FileManager::OPCODE_MOVEFILE);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that file name changed
    this->system("! test -e file1");
    this->system("test -f file2");

    // Clean up
    this->system("rm file2");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::moveFileFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove file1, if it exists
    this->system("rm -rf file1");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to move file1 to file2 (should fail)
    this->moveFile("file1", "file2");

    // Assert failure
    this->assertFailure(FileManager::OPCODE_MOVEFILE);
    ASSERT_EVENTS_FileMoveError_SIZE(1);
    ASSERT_EVENTS_FileMoveError(0, "file1", "file2", Os::FileSystem::DOESNT_EXIST);
}

void FileManagerTester ::removeDirectorySucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");

    // Create test_dir
    this->system("mkdir test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Remove test_dir
    this->removeDirectory("test_dir");

    // Assert success
    this->assertSuccess(FileManager::OPCODE_REMOVEDIRECTORY);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_dir is not there
    this->system("! test -e test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::removeDirectoryFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to remove test_dir (should fail)
    this->removeDirectory("test_dir");

    // Assert failure
    this->assertFailure(FileManager::OPCODE_REMOVEDIRECTORY);
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_DirectoryRemoveError(0, "test_dir", Os::FileSystem::DOESNT_EXIST);
}

void FileManagerTester ::removeFileSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->system("touch test_file");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Remove test_file
    this->removeFile("test_file", false);

    // Assert success
    this->assertSuccess(FileManager::OPCODE_REMOVEFILE);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_file is not there
    this->system("! test -e test_file");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::removeFileFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to remove test_file (should fail)
    this->removeFile("test_file", false);

    // Assert failure
    this->assertFailure(FileManager::OPCODE_REMOVEFILE);
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_FileRemoveError(0, "test_file", Os::FileSystem::DOESNT_EXIST);
}

void FileManagerTester ::appendFileSucceed_newFile() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");

    //================================================================
    // Case 1: 1 normal files appended, new file created
    this->system("echo 'file1 text' > file1");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    this->appendFile("file1", "file2");
    this->assertSuccess(FileManager::OPCODE_APPENDFILE);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // check new file exists and has correct text inside
    this->system("test -e file2");
    assertFileContent("file2", "file1 text\n", 12);

    // Clean up
    this->system("rm -rf file1 file2");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::appendFileSucceed_existingFile() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");

    //================================================================
    // Case 2: 2 normal files appended, stored in existing file
    // create existing files
    this->system("echo 'file1 text' > file1");
    this->system("echo 'file2 text' > file2");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    this->appendFile("file1", "file2");
    this->assertSuccess(FileManager::OPCODE_APPENDFILE);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // check file still exists and has new text inside
    this->system("test -e file2");
    assertFileContent("file2", "file2 text\nfile1 text\n", 23);

    // Clean up
    this->system("rm -rf file1 file2");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::appendFileFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to append from a non-existing source
    this->appendFile("file1", "file2");

    // Assert failure
    this->assertFailure(FileManager::OPCODE_APPENDFILE);
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_AppendFileFailed(0, "file1", "file2", Os::FileSystem::DOESNT_EXIST);
}

void FileManagerTester ::fileSizeSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1");

    this->system("echo 'file1 text' > file1");
#else
    FAIL();  // Commands not implemented for this OS
#endif
    Fw::CmdStringArg cmdStringFile("file1");
    this->sendCmd_FileSize(INSTANCE, CMD_SEQ, cmdStringFile);
    this->component.doDispatch();

    this->assertSuccess(FileManager::OPCODE_FILESIZE, 2);
    ASSERT_EVENTS_FileSizeSucceeded(0, "file1", 11);
}

void FileManagerTester ::fileSizeFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    Fw::CmdStringArg cmdStringFile("file1");
    this->sendCmd_FileSize(INSTANCE, CMD_SEQ, cmdStringFile);
    this->component.doDispatch();

    this->assertFailure(FileManager::OPCODE_FILESIZE);
}

void FileManagerTester ::calculateCrcSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "crc_test_file";

    // Remove and recreate the test file with deterministic content
    this->system("rm -f crc_test_file");
    {
        std::ofstream outFile(fileName, std::ios::binary);
        ASSERT_TRUE(outFile.is_open());
        outFile << "123456789";  // Payload with a well-known CRC32
    }

    // Compute expected CRC using the same file utility as the component
    Os::File crcFile;
    U32 expectedCrc = 0;
    ASSERT_EQ(Os::File::OP_OK, crcFile.open(fileName, Os::File::OPEN_READ));
    ASSERT_EQ(Os::File::OP_OK, crcFile.calculateCrc(expectedCrc));
    crcFile.close();

    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_CalculateCrc(INSTANCE, CMD_SEQ, cmdStringFile);
    this->component.doDispatch();

    // Validate command response and emitted events
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_CALCULATECRC, CMD_SEQ, Fw::CmdResponse::OK);

    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_CalculateCrcStarted(0, fileName);
    ASSERT_EVENTS_CalculateCrcSucceeded(0, fileName, expectedCrc);

    this->system("rm -f crc_test_file");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::resetDpState() {
    this->m_dpSendCount = 0;
    this->m_dpBytesSent = 0;
    this->m_dpGetShouldFail = false;
    this->m_dpGetUndersizedBuffer = false;
    this->m_dpLastPriority = 0;
    // Construct the buffer over the backing store: setData adjusts an existing
    // allocation rather than establishing one, so it cannot initialize a
    // default-constructed buffer
    this->m_dpContainerBuffer = Fw::Buffer(this->m_dpContainerData, sizeof this->m_dpContainerData);
}

Fw::Success::T FileManagerTester ::productGet_handler(FwDpIdType id, FwSizeType size, Fw::Buffer& buffer) {
    if (this->m_dpGetShouldFail || (size > sizeof this->m_dpContainerData)) {
        return Fw::Success::FAILURE;
    }
    // An undersized buffer lets the serialization failure path be exercised
    const FwSizeType bufferSize =
        this->m_dpGetUndersizedBuffer ? Fw::DpContainer::MIN_PACKET_SIZE : sizeof this->m_dpContainerData;
    this->m_dpContainerBuffer = Fw::Buffer(this->m_dpContainerData, bufferSize);
    buffer = this->m_dpContainerBuffer;
    return Fw::Success::SUCCESS;
}

void FileManagerTester ::productSend_handler(FwDpIdType id, const Fw::Buffer& buffer) {
    this->m_dpSendCount++;
    this->m_dpBytesSent += buffer.getSize();

    // Recover the priority and payload size from the serialized container.
    // Each container holds one chunk: a FileChunkHeaderRecord followed by a
    // FileChunkDataRecord, so a non-zero data size confirms the chunk actually
    // carried header-plus-data content rather than an empty container.
    Fw::DpContainer container(id, buffer);
    if (container.deserializeHeader() == Fw::FW_SERIALIZE_OK) {
        this->m_dpLastPriority = container.getPriority();
        const FwSizeType dataSize = container.getDataSize();
        this->m_dpPayloadBytes += dataSize;
        if ((this->m_dpMinPayloadBytes == 0) || (dataSize < this->m_dpMinPayloadBytes)) {
            this->m_dpMinPayloadBytes = dataSize;
        }
    }
}

void FileManagerTester ::generateDpSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_test_file";
    this->system("rm -f dp_test_file");
    // 100 bytes of deterministic content
    this->system("printf '0123456789%.0s' $(seq 1 10) > dp_test_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // 32 byte chunks over a 100 byte file yields 4 chunks (32 + 32 + 32 + 4)
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 32, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();

    // Only the start event so far; the response is deferred
    ASSERT_EVENTS_GenerateDpStarted_SIZE(1);
    ASSERT_EVENTS_GenerateDpStarted(0, fileName, 100);
    ASSERT_CMD_RESPONSE_SIZE(0);

    // Drive the rate group until the file is fully packaged
    this->runRateGroupCycles(10);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpComplete_SIZE(1);
    ASSERT_EVENTS_GenerateDpComplete(0, fileName, 4);
    ASSERT_EQ(4u, this->m_dpSendCount);

    // Every container carried a real header-plus-data payload (none empty), and
    // the total payload exceeds the 100 file bytes by the per-chunk record
    // overhead, confirming the chunks hold actual content and not just headers
    ASSERT_GT(this->m_dpMinPayloadBytes, 0u);
    ASSERT_GT(this->m_dpPayloadBytes, 100u);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_test_file");
#endif
}

void FileManagerTester ::generateDpFileNotFound() {
    this->resetDpState();

    const char* const missingName = "no_such_dp_file";

    // Recover the exact open status the component will observe for the missing
    // file, so the assertion checks the real value rather than a hardcoded one
    Os::File probeFile;
    const Os::File::Status openStatus = probeFile.open(missingName, Os::File::OPEN_READ);
    ASSERT_NE(Os::File::OP_OK, openStatus);
    probeFile.close();

    Fw::CmdStringArg cmdStringFile(missingName);
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 32, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpFailed_SIZE(1);
    ASSERT_EVENTS_GenerateDpFailed(0, missingName, FileManager_GenerateDpStage::OPEN, static_cast<U32>(openStatus));
    ASSERT_EQ(0u, this->m_dpSendCount);
}

void FileManagerTester ::generateDpEmptyFile() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_empty_file";
    this->system("rm -f dp_empty_file");
    this->system("touch dp_empty_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 32, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();

    // An empty file completes immediately with no chunks
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpStarted_SIZE(1);
    ASSERT_EQ(0u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_empty_file");
#endif
}

void FileManagerTester ::generateDpChunkSizeClamped() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_clamp_file";
    this->system("rm -f dp_clamp_file");
    this->system("printf '0123456789%.0s' $(seq 1 5) > dp_clamp_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // A chunk size beyond the configured maximum is clamped, so the 50 byte
    // file still fits in a single chunk
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE * 4, 0, 0,
                             0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();
    this->runRateGroupCycles(5);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpComplete(0, fileName, 1);
    ASSERT_EQ(1u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_clamp_file");
#endif
}

void FileManagerTester ::generateDpPartialRange() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_range_file";
    this->system("rm -f dp_range_file");
    this->system("printf '0123456789%.0s' $(seq 1 10) > dp_range_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // Bytes [20, 60) of a 100 byte file with 16 byte chunks yields 3 chunks
    // (16 + 16 + 8), which lets an operator retransmit part of a file
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 20, 60, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();
    this->runRateGroupCycles(10);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpComplete(0, fileName, 3);
    ASSERT_EQ(3u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_range_file");
#endif
}

void FileManagerTester ::generateDpInvalidRange() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_bad_range_file";
    this->system("rm -f dp_bad_range_file");
    this->system("printf '0123456789%.0s' $(seq 1 10) > dp_bad_range_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // A begin offset at or past the end offset has nothing to package
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 60, 20, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpInvalidRange_SIZE(1);
    ASSERT_EVENTS_GenerateDpInvalidRange(0, fileName, 60, 20, 100);
    ASSERT_EQ(0u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_bad_range_file");
#endif
}

void FileManagerTester ::generateDpBufferFailure() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_buffer_fail_file";
    this->system("rm -f dp_buffer_fail_file");
    this->system("printf '0123456789%.0s' $(seq 1 5) > dp_buffer_fail_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();
    this->m_dpGetShouldFail = true;

    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();
    this->runRateGroupCycles(5);

    // A container allocation failure is reported but does not fail the command
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpBufferFailed_SIZE(1);
    ASSERT_EVENTS_GenerateDpBufferFailed(0, fileName);
    ASSERT_EQ(0u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_buffer_fail_file");
#endif
}

void FileManagerTester ::generateDpWhileBusy() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_busy_file";
    this->system("rm -f dp_busy_file");
    this->system("printf '0123456789%.0s' $(seq 1 10) > dp_busy_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // Start a generation and leave it in progress by not running the rate group
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(0);

    // A second request while one is in progress is reported and rejected
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpFailed_SIZE(1);
    ASSERT_EVENTS_GenerateDpFailed(0, fileName, FileManager_GenerateDpStage::BUSY, 0);

    // Let the first generation finish so the component returns to idle, and
    // confirm the busy rejection did not corrupt the in-progress generation
    this->runRateGroupCycles(10);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpComplete_SIZE(1);
    ASSERT_EVENTS_GenerateDpComplete(0, fileName, 7);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_busy_file");
#endif
}

void FileManagerTester ::generateDpSerializationFailure() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_serialize_fail_file";
    this->system("rm -f dp_serialize_fail_file");
    this->system("printf '0123456789%.0s' $(seq 1 5) > dp_serialize_fail_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();
    this->m_dpGetUndersizedBuffer = true;

    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 16, 0, 0, 0, FileManager_GenerateDpMode::PACED);
    this->component.doDispatch();
    this->runRateGroupCycles(5);

    // A serialization failure is reported but does not fail the command
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpFailed_SIZE(1);
    // The undersized buffer leaves no room to serialize the chunk, so the
    // failure status is the serialize error the handler forwards
    ASSERT_EVENTS_GenerateDpFailed(0, fileName, FileManager_GenerateDpStage::SERIALIZE,
                                   static_cast<U32>(Fw::FW_SERIALIZE_NO_ROOM_LEFT));

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_serialize_fail_file");
#endif
}

void FileManagerTester ::generateDpImmediateMode() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_immediate_file";
    this->system("rm -f dp_immediate_file");
    this->system("printf '0123456789%.0s' $(seq 1 10) > dp_immediate_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // In immediate mode the whole file is emitted without the rate group
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 32, 0, 0, 0, FileManager_GenerateDpMode::IMMEDIATE);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GenerateDpComplete(0, fileName, 4);
    ASSERT_EQ(4u, this->m_dpSendCount);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_immediate_file");
#endif
}

void FileManagerTester ::generateDpCustomPriority() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    const char* const fileName = "dp_priority_file";
    this->system("rm -f dp_priority_file");
    this->system("printf '0123456789%.0s' $(seq 1 5) > dp_priority_file");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    this->resetDpState();

    Fw::CmdStringArg cmdStringFile(fileName);
    // A non-zero priority overrides the configured default
    this->sendCmd_GenerateDp(INSTANCE, CMD_SEQ, cmdStringFile, 64, 0, 0, 42, FileManager_GenerateDpMode::IMMEDIATE);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_GENERATEDP, CMD_SEQ, Fw::CmdResponse::OK);
    ASSERT_EQ(1u, this->m_dpSendCount);
    ASSERT_EQ(42u, this->m_dpLastPriority);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    this->system("rm -f dp_priority_file");
#endif
}

void FileManagerTester ::listDirectorySucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir and create it with some files
    this->system("rm -rf test_dir");
    this->system("mkdir test_dir");
    this->system("touch test_dir/file1.txt");
    this->system("touch test_dir/file2.txt");
    this->system("touch test_dir/file3.dat");
#else
    SKIP();  // Commands not implemented for this OS
#endif

    // List the directory
    this->listDirectory("test_dir");

    // At this point, only the "Starting" event should be present
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ListDirectoryStarted_SIZE(1);

    // No command response yet (still in progress)
    ASSERT_CMD_RESPONSE_SIZE(0);

    // Run rate group cycles to process the directory listing asynchronously
    // We need enough cycles to process all files (3) plus completion
    this->runRateGroupCycles(10);  // Give it plenty of cycles to complete

    // Assert success - 5 events: Starting + 3 DirectoryListing + Success
    this->assertSuccess(FileManager::OPCODE_LISTDIRECTORY, 5);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Clean up
    this->system("rm -rf test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::listDirectoryWithSubdirs() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir and create a more complex directory structure with files and subdirectories
    this->system("rm -rf test_dir");

    // Create main directory structure with multiple levels of subdirectories
    this->system("mkdir -p test_dir/subdir1/nested1");
    this->system("mkdir -p test_dir/subdir2/nested2");
    this->system("mkdir -p test_dir/subdir3/nested3/deep1");
    this->system("mkdir -p test_dir/emptydir");  // An empty directory

    // Create various files with different sizes in root directory
    this->system("echo 'Small file content' > test_dir/file1.txt");
    this->system("echo 'Medium sized file with more content than the first one' > test_dir/file2.txt");
    this->system("dd if=/dev/zero bs=1K count=4 of=test_dir/binaryfile.dat 2>/dev/null");  // 4KB binary file

    // Create files in subdirectories
    this->system("echo 'Subdir1 file 1' > test_dir/subdir1/sub1_file1.txt");
    this->system("echo 'Subdir1 file 2' > test_dir/subdir1/sub1_file2.txt");
    this->system("echo 'Nested1 file' > test_dir/subdir1/nested1/nested_file.txt");

    this->system("echo 'Subdir2 file 1' > test_dir/subdir2/sub2_file1.txt");
    this->system("echo 'Nested2 file' > test_dir/subdir2/nested2/nested_file.txt");

    this->system("echo 'Subdir3 file 1' > test_dir/subdir3/sub3_file1.txt");
    this->system("echo 'Deep1 file' > test_dir/subdir3/nested3/deep1/deep_file.txt");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // List the directory
    this->listDirectory("test_dir");

    // At this point, only the "Starting" event should be present
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ListDirectoryStarted_SIZE(1);

    // No command response yet (still in progress)
    ASSERT_CMD_RESPONSE_SIZE(0);

    // Run rate group cycles to process the directory listing asynchronously
    // This directory has many files and subdirectories, so give it more cycles
    this->runRateGroupCycles(20);  // Give it plenty of cycles to complete

    // Check command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FileManager::OPCODE_LISTDIRECTORY, 0, Fw::CmdResponse::OK);

    // For this test, we'll just verify that events were emitted
    ASSERT_GT(this->eventHistory_DirectoryListing->size(), 0U);
    ASSERT_GT(this->eventHistory_DirectoryListingSubdir->size(), 0U);

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Clean up
    this->system("rm -rf test_dir");
#else
    SKIP();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::listDirectoryFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir to ensure it doesn't exist
    this->system("rm -rf test_dir");
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Attempt to list nonexistent directory
    this->listDirectory("test_dir");

    // Assert failure
    this->assertFailure(FileManager::OPCODE_LISTDIRECTORY);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void FileManagerTester ::system(const char* const cmd) {
    const int status = ::system(cmd);
    ASSERT_EQ(static_cast<int>(0), status);
}

void FileManagerTester ::createDirectory(const char* const dirName) {
    Fw::CmdStringArg cmdStringDir(dirName);
    this->sendCmd_CreateDirectory(INSTANCE, CMD_SEQ, cmdStringDir);
    this->component.doDispatch();
}

void FileManagerTester ::moveFile(const char* const sourceFileName, const char* const destFileName) {
    Fw::CmdStringArg cmdStringSource(sourceFileName);
    Fw::CmdStringArg cmdStringDest(destFileName);
    this->sendCmd_MoveFile(INSTANCE, CMD_SEQ, cmdStringSource, cmdStringDest);
    this->component.doDispatch();
}

void FileManagerTester ::removeDirectory(const char* const dirName) {
    Fw::CmdStringArg cmdStringDir(dirName);
    this->sendCmd_RemoveDirectory(INSTANCE, CMD_SEQ, cmdStringDir);
    this->component.doDispatch();
}

void FileManagerTester ::removeFile(const char* const fileName, bool ignoreErrors) {
    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_RemoveFile(INSTANCE, CMD_SEQ, cmdStringFile, ignoreErrors);
    this->component.doDispatch();
}

void FileManagerTester ::appendFile(const char* const source, const char* const target) {
    Fw::CmdStringArg cmdSource(source);
    Fw::CmdStringArg cmdTarget(target);
    this->sendCmd_AppendFile(INSTANCE, CMD_SEQ, cmdSource, cmdTarget);
    this->component.doDispatch();
}

void FileManagerTester ::listDirectory(const char* const dirName) {
    Fw::CmdStringArg cmdStringDir(dirName);
    this->sendCmd_ListDirectory(INSTANCE, CMD_SEQ, cmdStringDir);
    this->component.doDispatch();
}

void FileManagerTester ::runRateGroupCycles(const U32 cycles) {
    // Simulate rate group execution for asynchronous operations (directory
    // listing and data product generation).
    // This method mimics the behavior of Rate Group 2 (0.5Hz) by calling the
    // schedule handler repeatedly until the directory listing operation completes.
    // Each cycle processes one directory entry, ensuring bounded execution time.
    for (U32 i = 0; i < cycles; i++) {
        // Call the schedule handler to process one directory entry per cycle
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();

        // Check if the asynchronous operation has completed
        const bool listingActive = (this->component.m_listState == FileManager::LISTING_IN_PROGRESS);
        const bool dpActive = (this->component.m_dpState == FileManager::DP_IN_PROGRESS);
        if (!listingActive && !dpActive) {
            // Operation finished, no need to continue cycling
            break;
        }
    }
}

void FileManagerTester ::assertSuccess(const FwOpcodeType opcode, const U32 eventSize) const {
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, opcode, CMD_SEQ, Fw::CmdResponse::OK);

    ASSERT_EVENTS_SIZE(eventSize);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CommandsExecuted_SIZE(1);
    ASSERT_TLM_CommandsExecuted(0, 1);
}

void FileManagerTester ::assertFileContent(const char* const fileName,
                                           const char* const expectedString,
                                           const U32 length) const {
    char fileString[length];
    memset(fileString, 0, length);
    std::ifstream file;
    file.open(fileName);

    file.read(fileString, length);
    file.close();

    ASSERT_STREQ(expectedString, fileString);
}

void FileManagerTester ::assertFailure(const FwOpcodeType opcode) const {
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, opcode, CMD_SEQ, Fw::CmdResponse::EXECUTION_ERROR);

    ASSERT_EVENTS_SIZE(2);  // Starting event + Error

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_Errors_SIZE(1);
    ASSERT_TLM_Errors(0, 1);
}
void FileManagerTester ::from_pingOut_handler(const FwIndexType portNum, U32 key) {
    this->pushFromPortEntry_pingOut(key);
}

}  // end namespace Svc
