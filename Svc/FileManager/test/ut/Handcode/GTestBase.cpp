// ======================================================================
// \title  FileManager/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for FileManager component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  FileManagerGTestBase ::
    FileManagerGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        FileManagerTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  FileManagerGTestBase ::
    ~FileManagerGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertCmdResponse_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ((unsigned long) size, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of command response history\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertCmdResponse(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history (" 
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << index
      << " in command response history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertTlm_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: CommandsExecuted
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertTlm_CommandsExecuted_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_CommandsExecuted->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel CommandsExecuted\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_CommandsExecuted->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertTlm_CommandsExecuted(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_CommandsExecuted->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel CommandsExecuted\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_CommandsExecuted->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_CommandsExecuted& e =
      this->tlmHistory_CommandsExecuted->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel CommandsExecuted\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: Errors
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertTlm_Errors_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_Errors->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel Errors\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_Errors->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertTlm_Errors(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_Errors->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel Errors\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_Errors->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_Errors& e =
      this->tlmHistory_Errors->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel Errors\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: DirectoryCreateError
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_DirectoryCreateError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_DirectoryCreateError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event DirectoryCreateError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_DirectoryCreateError->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_DirectoryCreateError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const dirName,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_DirectoryCreateError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event DirectoryCreateError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_DirectoryCreateError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_DirectoryCreateError& e =
      this->eventHistory_DirectoryCreateError->at(index);
    ASSERT_STREQ(dirName, e.dirName.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument dirName at index "
      << index
      << " in history of event DirectoryCreateError\n"
      << "  Expected: " << dirName << "\n"
      << "  Actual:   " << e.dirName.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event DirectoryCreateError\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: DirectoryRemoveError
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_DirectoryRemoveError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_DirectoryRemoveError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event DirectoryRemoveError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_DirectoryRemoveError->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_DirectoryRemoveError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const dirName,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_DirectoryRemoveError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event DirectoryRemoveError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_DirectoryRemoveError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_DirectoryRemoveError& e =
      this->eventHistory_DirectoryRemoveError->at(index);
    ASSERT_STREQ(dirName, e.dirName.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument dirName at index "
      << index
      << " in history of event DirectoryRemoveError\n"
      << "  Expected: " << dirName << "\n"
      << "  Actual:   " << e.dirName.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event DirectoryRemoveError\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileMoveError
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_FileMoveError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileMoveError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileMoveError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileMoveError->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_FileMoveError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const sourceFileName,
        const char *const destFileName,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_FileMoveError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileMoveError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileMoveError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileMoveError& e =
      this->eventHistory_FileMoveError->at(index);
    ASSERT_STREQ(sourceFileName, e.sourceFileName.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument sourceFileName at index "
      << index
      << " in history of event FileMoveError\n"
      << "  Expected: " << sourceFileName << "\n"
      << "  Actual:   " << e.sourceFileName.toChar() << "\n";
    ASSERT_STREQ(destFileName, e.destFileName.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument destFileName at index "
      << index
      << " in history of event FileMoveError\n"
      << "  Expected: " << destFileName << "\n"
      << "  Actual:   " << e.destFileName.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event FileMoveError\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileRemoveError
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_FileRemoveError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileRemoveError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileRemoveError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileRemoveError->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_FileRemoveError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const fileName,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_FileRemoveError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileRemoveError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileRemoveError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileRemoveError& e =
      this->eventHistory_FileRemoveError->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileRemoveError\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event FileRemoveError\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ShellCommandFailed
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_ShellCommandFailed_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ShellCommandFailed->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ShellCommandFailed\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ShellCommandFailed->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_ShellCommandFailed(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const command,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_ShellCommandFailed->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ShellCommandFailed\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ShellCommandFailed->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ShellCommandFailed& e =
      this->eventHistory_ShellCommandFailed->at(index);
    ASSERT_STREQ(command, e.command.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument command at index "
      << index
      << " in history of event ShellCommandFailed\n"
      << "  Expected: " << command << "\n"
      << "  Actual:   " << e.command.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event ShellCommandFailed\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: ShellCommandSucceeded
  // ----------------------------------------------------------------------

  void FileManagerGTestBase ::
    assertEvents_ShellCommandSucceeded_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_ShellCommandSucceeded->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event ShellCommandSucceeded\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_ShellCommandSucceeded->size() << "\n";
  }

  void FileManagerGTestBase ::
    assertEvents_ShellCommandSucceeded(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const command
    ) const
  {
    ASSERT_GT(this->eventHistory_ShellCommandSucceeded->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event ShellCommandSucceeded\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_ShellCommandSucceeded->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_ShellCommandSucceeded& e =
      this->eventHistory_ShellCommandSucceeded->at(index);
    ASSERT_STREQ(command, e.command.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument command at index "
      << index
      << " in history of event ShellCommandSucceeded\n"
      << "  Expected: " << command << "\n"
      << "  Actual:   " << e.command.toChar() << "\n";
  }

} // end namespace Svc
