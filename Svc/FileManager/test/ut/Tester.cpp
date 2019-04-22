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
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      FileManagerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("FileManager")
#else
      FileManagerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    this->connectPorts();
    this->initComponents();
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
    createDirectorySucceed(void) 
  {

    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");

    // Create test_dir
    this->createDirectory("test_dir");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_CREATEDIRECTORY
    );

    // Check that test_dir exists
    this->system("test -d test_dir");

    // Clean up
    this->system("rmdir test_dir");

  }

  void Tester ::
    createDirectoryFail(void) 
  {

    // Create test_dir
    this->system("rm -rf test_dir");
    this->system("mkdir test_dir");

    // Attempt to create test_dir (should fail)
    this->createDirectory("test_dir");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_CREATEDIRECTORY
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DirectoryCreateError(
        0, 
        "test_dir", 
        Os::FileSystem::ALREADY_EXISTS
    );

  }

  void Tester ::
    moveFileSucceed(void) 
  {

    // Remove file1 and file2, if they exist
    this->system("rm -rf file1 file2");

    // Create file1
    this->system("touch file1");

    // Move file1 to file2
    this->moveFile("file1", "file2");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_MOVEFILE
    );

    // Check that file name changed
    this->system("! test -e file1");
    this->system("test -f file2");

    // Clean up
    this->system("rm file2");

  }

  void Tester ::
    moveFileFail(void) 
  {

    // Remove file1, if it exists
    this->system("rm -rf file1");

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
    removeDirectorySucceed(void) 
  {

    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");

    // Create test_dir
    this->system("mkdir test_dir");

    // Remove test_dir
    this->removeDirectory("test_dir");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_REMOVEDIRECTORY
    );

    // Check that test_dir is not there
    this->system("! test -e test_dir");

  }

  void Tester ::
    removeDirectoryFail(void) 
  {

    // Remove test_dir, if it exists
    this->system("rm -rf test_dir");

    // Attempt to remove test_dir (should fail)
    this->removeDirectory("test_dir");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_REMOVEDIRECTORY
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DirectoryRemoveError(
        0,
        "test_dir",
        Os::FileSystem::INVALID_PATH
    );

  }

  void Tester ::
    removeFileSucceed(void) 
  {

    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->system("touch test_file");

    // Remove test_file
    this->removeFile("test_file");

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_REMOVEFILE
    );

    // Check that test_file is not there
    this->system("! test -e test_file");

  }

  void Tester ::
    removeFileFail(void) 
  {

    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Attempt to remove test_file (should fail)
    this->removeFile("test_file");

    // Assert failure
    this->assertFailure(
        FileManager::OPCODE_REMOVEFILE
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileRemoveError(
        0,
        "test_file",
        Os::FileSystem::INVALID_PATH
    );

  }

  void Tester ::
    shellCommandSucceed(void) 
  {

    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Create test_file
    this->shellCommand("touch test_file", LOG_FILE);

    // Assert success
    this->assertSuccess(
        FileManager::OPCODE_SHELLCOMMAND,
        1
    );
    ASSERT_EVENTS_ShellCommandSucceeded_SIZE(1);
    ASSERT_EVENTS_ShellCommandSucceeded(0, "touch test_file");

    // Check that test_file is there
    this->system("test -f test_file");

    // Clean up
    this->system("rm test_file");

  }

  void Tester ::
    shellCommandFail(void) 
  {

    // Remove test_file, if it exists
    this->system("rm -rf test_file");

    // Attempt to remove test_file (should fail)
    this->shellCommand("rm test_file", LOG_FILE);

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

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
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
    initComponents(void) 
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
    removeFile(const char *const fileName) 
  {
    Fw::CmdStringArg cmdStringFile(fileName);
    this->sendCmd_RemoveFile(
        INSTANCE,
        CMD_SEQ,
        cmdStringFile
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
        Fw::COMMAND_OK
    );

    ASSERT_EVENTS_SIZE(eventSize);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CommandsExecuted_SIZE(1);
    ASSERT_TLM_CommandsExecuted(0, 1);
  }

  void Tester ::
    assertFailure(const FwOpcodeType opcode) const
  {
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0, 
        opcode,
        CMD_SEQ,
        Fw::COMMAND_EXECUTION_ERROR
    );

    ASSERT_EVENTS_SIZE(1);

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
