// ======================================================================
// \title  Tester.cpp
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

#include "Tester.hpp"

#define INSTANCE 0
#define CMD_SEQ 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10
#define LOG_FILE "log.txt"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      FileManagerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("FileManager")
  {
    this->connectPorts();
    this->initComponents();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    createDirectorySucceed()
  {

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");
    // Create test_dir
    this->createDirectory("test_dir");

#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_CREATEDIRECTORY
    );

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_dir exists
    this->system("test -d test_dir");

    // Clean up
    this->system("rmdir test_dir");
#else
    FAIL(); // Commands not implemented for this OS
#endif

  }

  void Tester ::
    createDirectoryFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Create test_dir
    this->system("rm -rf test_dir");
    this->system("mkdir test_dir");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Attempt to create test_dir (should fail)
    this->createDirectory("test_dir");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_CREATEDIRECTORY
    );
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_DirectoryCreateError(
        0,
        "test_dir",
        Os::FileSystem::ALREADY_EXISTS
    );

  }

  void Tester ::
    moveFileSucceed()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove file1 and file2, if they exist
    this->system("rm -rf file1 file2");

    // Create file1
    this->system("touch file1");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Move file1 to file2
    this->moveFile("file1", "file2");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_MOVEFILE
    );

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that file name changed
    this->system("! test -e file1");
    this->system("test -f file2");

    // Clean up
    this->system("rm file2");
#else
    FAIL(); // Commands not implemented for this OS
#endif

  }

  void Tester ::
    moveFileFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove file1, if it exists
    this->system("rm -rf file1");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Attempt to move file1 to file2 (should fail)
    this->moveFile("file1", "file2");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_MOVEFILE
    );
    ASSERT_EVENTS_FileMoveError_SIZE(1);
    ASSERT_EVENTS_FileMoveError(
        0,
        "file1",
        "file2",
        Os::FileSystem::INVALID_PATH
    );

  }

  void Tester ::
    removeDirectorySucceed()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");

    // Create test_dir
    this->system("mkdir test_dir");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Remove test_dir
    this->removeDirectory("test_dir");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_REMOVEDIRECTORY
    );

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_dir is not there
    this->system("! test -e test_dir");
#else
    FAIL(); // Commands not implemented for this OS
#endif
  }

  void Tester ::
    removeDirectoryFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Attempt to remove test_dir (should fail)
    this->removeDirectory("test_dir");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_REMOVEDIRECTORY
    );
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_DirectoryRemoveError(
        0,
        "test_dir",
        Os::FileSystem::INVALID_PATH
    );

  }

  void Tester ::
    removeFileSucceed()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->system("touch test_file");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Remove test_file
    this->removeFile("test_file", false);

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_REMOVEFILE
    );

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Check that test_file is not there
    this->system("! test -e test_file");
#else
    FAIL(); // Commands not implemented for this OS
#endif
  }

  void Tester ::
    removeFileFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Attempt to remove test_file (should fail)
    this->removeFile("test_file", false);

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_REMOVEFILE
    );
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_FileRemoveError(
        0,
        "test_file",
        Os::FileSystem::INVALID_PATH
    );

  }

  void Tester ::
    shellCommandSucceed()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->shellCommand("touch test_file", LOG_FILE);
#else
    FAIL(); // Commands not implemented for this OS
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
    FAIL(); // Commands not implemented for this OS
#endif
  }

  void Tester ::
    shellCommandFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Attempt to remove test_file (should fail)
    this->shellCommand("rm test_file", LOG_FILE);
#else
    FAIL(); // Commands not implemented for this OS
#endif
    {
      // Assert failure
      this->assertFailure(
          FileManager::OPCODE_SHELLCOMMAND
      );
      ASSERT_EVENTS_ShellCommandFailed_SIZE(1);
      const EventEntry_ShellCommandFailed& e =
        this->eventHistory_ShellCommandFailed->at(0);
      const U32 status = e.status;
      ASSERT_NE(static_cast<U32>(0), status);
      ASSERT_EVENTS_ShellCommandFailed(
          0,
          "rm test_file",
          status
      );
    }
  }

  void Tester ::
    appendFileSucceed_newFile()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");

    //================================================================
    // Case 1: 1 normal files appended, new file created
    this->system("echo 'file1 text' > file1");
#else
    FAIL(); // Commands not implemented for this OS
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
    FAIL(); // Commands not implemented for this OS
#endif
  }

  void Tester ::
    appendFileSucceed_existingFile()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");

    //================================================================
    // Case 2: 2 normal files appended, stored in existing file
    // create existing files
    this->system("echo 'file1 text' > file1");
    this->system("echo 'file2 text' > file2");
#else
    FAIL(); // Commands not implemented for this OS
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
    FAIL(); // Commands not implemented for this OS
#endif
  }

  void Tester ::
    appendFileFail()
  {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1 file2");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    // Attempt to append from a non-existing source
    this->appendFile("file1", "file2");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_APPENDFILE
    );
    ASSERT_EVENTS_SIZE(2);  // Starting event + Error
    ASSERT_EVENTS_AppendFileFailed(
        0,
        "file1",
        "file2",
        Os::FileSystem::INVALID_PATH
    );
  }

  void Tester ::
    fileSizeSucceed() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1");

    this->system("echo 'file1 text' > file1");
#else
    FAIL(); // Commands not implemented for this OS
#endif
    Fw::CmdStringArg cmdStringFile("file1");
    this->sendCmd_FileSize(
        INSTANCE,
        CMD_SEQ,
        cmdStringFile
    );
    this->component.doDispatch();

    this->assertSuccess(FileManager::OPCODE_FILESIZE, 2);
    ASSERT_EVENTS_FileSizeSucceeded(0, "file1", 11);
  }

  void Tester ::
    fileSizeFail() {
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    // Remove testing files, if they exist
    this->system("rm -rf file1");
#else
    FAIL(); // Commands not implemented for this OS
#endif

    Fw::CmdStringArg cmdStringFile("file1");
    this->sendCmd_FileSize(
        INSTANCE,
        CMD_SEQ,
        cmdStringFile
    );
    this->component.doDispatch();

    this->assertFailure(
        FileManager::OPCODE_FILESIZE
    );
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0,
        this->get_from_timeCaller(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(
        0,
        this->get_from_cmdResponseOut(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(
        0,
        this->get_from_cmdRegOut(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0,
        this->get_from_LogText(0)
    );

  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH,
        INSTANCE
    );
  }

  void Tester ::
    system(const char *const cmd)
  {
    const NATIVE_INT_TYPE status = ::system(cmd);
    ASSERT_EQ(static_cast<NATIVE_INT_TYPE>(0), status);
  }

  void Tester ::
    createDirectory(const char *const dirName)
  {
    Fw::CmdStringArg cmdStringDir(dirName);
    this->sendCmd_CreateDirectory(
        INSTANCE,
        CMD_SEQ,
        cmdStringDir
    );
    this->component.doDispatch();
  }

  void Tester ::
    moveFile(
        const char *const sourceFileName,
        const char *const destFileName
    )
  {
    Fw::CmdStringArg cmdStringSource(sourceFileName);
    Fw::CmdStringArg cmdStringDest(destFileName);
    this->sendCmd_MoveFile(
        INSTANCE,
        CMD_SEQ,
        cmdStringSource,
        cmdStringDest
    );
    this->component.doDispatch();
  }

  void Tester ::
    removeDirectory(const char *const dirName)
  {
    Fw::CmdStringArg cmdStringDir(dirName);
    this->sendCmd_RemoveDirectory(
        INSTANCE,
        CMD_SEQ,
        cmdStringDir
    );
    this->component.doDispatch();
  }

  void Tester ::
    removeFile(const char *const fileName, bool ignoreErrors)
  {
    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_RemoveFile(
        INSTANCE,
        CMD_SEQ,
        cmdStringFile,
        ignoreErrors
    );
    this->component.doDispatch();
  }

  void Tester ::
    shellCommand(
        const char *const command,
        const char *const logFileName
    )
  {
    Fw::CmdStringArg cmdStringCommand(command);
    Fw::CmdStringArg cmdStringLogFile(logFileName);
    this->sendCmd_ShellCommand(
        INSTANCE,
        CMD_SEQ,
        cmdStringCommand,
        cmdStringLogFile
    );
    this->component.doDispatch();
  }

  void Tester ::
    appendFile(
        const char *const source,
        const char *const target
    )
  {
    Fw::CmdStringArg cmdSource(source);
    Fw::CmdStringArg cmdTarget(target);
    this->sendCmd_AppendFile(
        INSTANCE,
        CMD_SEQ,
        cmdSource,
        cmdTarget
    );
    this->component.doDispatch();
  }

  void Tester ::
    assertSuccess(
        const FwOpcodeType opcode,
        const U32 eventSize
    ) const
  {
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        opcode,
        CMD_SEQ,
        Fw::CmdResponse::OK
    );

    ASSERT_EVENTS_SIZE(eventSize);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CommandsExecuted_SIZE(1);
    ASSERT_TLM_CommandsExecuted(0, 1);
  }

  void Tester ::
    assertFileContent(
          const char *const fileName,
          const char *const expectedString,
          const U32 length
      ) const {
    char fileString[length];
    memset(fileString, 0, length);
    std::ifstream file;
    file.open(fileName);

    file.read(fileString, length);
    file.close();

    ASSERT_STREQ(expectedString, fileString);
  }

  void Tester ::
    assertFailure(const FwOpcodeType opcode) const
  {
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        opcode,
        CMD_SEQ,
        Fw::CmdResponse::EXECUTION_ERROR
    );

    ASSERT_EVENTS_SIZE(2);  // Starting event + Error

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_Errors_SIZE(1);
    ASSERT_TLM_Errors(0, 1);
  }
  void Tester ::
    from_pingOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    this->pushFromPortEntry_pingOut(key);
  }

} // end namespace Svc
