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

void FileManagerTester ::shellCommandSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->shellCommand("touch test_file", LOG_FILE);
#else
    FAIL();  // Commands not implemented for this OS
#endif

    // Assert success
    this->assertSuccess(FileManager::OPCODE_SHELLCOMMAND);
    ASSERT_EVENTS_ShellCommandSucceeded_SIZE(1);
    ASSERT_EVENTS_ShellCommandSucceeded(0, "touch test_file");

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_file is there
    this->system("test -f test_file");

    // Clean up
    this->system("rm test_file");
#else
    FAIL();  // Commands not implemented for this OS
#endif
}

void FileManagerTester ::shellCommandFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Attempt to remove test_file (should fail)
    this->shellCommand("rm test_file", LOG_FILE);
#else
    FAIL();  // Commands not implemented for this OS
#endif
    {
        // Assert failure
        this->assertFailure(FileManager::OPCODE_SHELLCOMMAND);
        ASSERT_EVENTS_ShellCommandFailed_SIZE(1);
        const EventEntry_ShellCommandFailed& e = this->eventHistory_ShellCommandFailed->at(0);
        const U32 status = e.status;
        ASSERT_NE(static_cast<U32>(0), status);
        ASSERT_EVENTS_ShellCommandFailed(0, "rm test_file", status);
    }
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

void FileManagerTester ::shellCommand(const char* const command, const char* const logFileName) {
    Fw::CmdStringArg cmdStringCommand(command);
    Fw::CmdStringArg cmdStringLogFile(logFileName);
    this->sendCmd_ShellCommand(INSTANCE, CMD_SEQ, cmdStringCommand, cmdStringLogFile);
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
    // Simulate rate group execution for asynchronous directory listing operations.
    // This method mimics the behavior of Rate Group 2 (0.5Hz) by calling the
    // schedule handler repeatedly until the directory listing operation completes.
    // Each cycle processes one directory entry, ensuring bounded execution time.
    for (U32 i = 0; i < cycles; i++) {
        // Call the schedule handler to process one directory entry per cycle
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();

        // Check if directory listing operation has completed
        if (this->component.m_listState != FileManager::LISTING_IN_PROGRESS) {
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
