// ======================================================================
// \title  ComLogger/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for ComLogger component Google Test harness base class
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

  ComLoggerGTestBase ::
    ComLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        ComLoggerTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  ComLoggerGTestBase ::
    ~ComLoggerGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
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

  void ComLoggerGTestBase ::
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
  // Events
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
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
  // Event: FileOpenError
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
    assertEvents_FileOpenError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileOpenError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileOpenError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileOpenError->size() << "\n";
  }

  void ComLoggerGTestBase ::
    assertEvents_FileOpenError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 errornum,
        const char *const file
    ) const
  {
    ASSERT_GT(this->eventHistory_FileOpenError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileOpenError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileOpenError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileOpenError& e =
      this->eventHistory_FileOpenError->at(index);
    ASSERT_EQ(errornum, e.errornum)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument errornum at index "
      << index
      << " in history of event FileOpenError\n"
      << "  Expected: " << errornum << "\n"
      << "  Actual:   " << e.errornum << "\n";
    ASSERT_STREQ(file, e.file.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument file at index "
      << index
      << " in history of event FileOpenError\n"
      << "  Expected: " << file << "\n"
      << "  Actual:   " << e.file.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileWriteError
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
    assertEvents_FileWriteError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileWriteError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileWriteError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileWriteError->size() << "\n";
  }

  void ComLoggerGTestBase ::
    assertEvents_FileWriteError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 errornum,
        const U32 bytesWritten,
        const U32 bytesToWrite,
        const char *const file
    ) const
  {
    ASSERT_GT(this->eventHistory_FileWriteError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileWriteError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileWriteError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileWriteError& e =
      this->eventHistory_FileWriteError->at(index);
    ASSERT_EQ(errornum, e.errornum)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument errornum at index "
      << index
      << " in history of event FileWriteError\n"
      << "  Expected: " << errornum << "\n"
      << "  Actual:   " << e.errornum << "\n";
    ASSERT_EQ(bytesWritten, e.bytesWritten)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument bytesWritten at index "
      << index
      << " in history of event FileWriteError\n"
      << "  Expected: " << bytesWritten << "\n"
      << "  Actual:   " << e.bytesWritten << "\n";
    ASSERT_EQ(bytesToWrite, e.bytesToWrite)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument bytesToWrite at index "
      << index
      << " in history of event FileWriteError\n"
      << "  Expected: " << bytesToWrite << "\n"
      << "  Actual:   " << e.bytesToWrite << "\n";
    ASSERT_STREQ(file, e.file.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument file at index "
      << index
      << " in history of event FileWriteError\n"
      << "  Expected: " << file << "\n"
      << "  Actual:   " << e.file.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileValidationError
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
    assertEvents_FileValidationError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileValidationError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileValidationError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileValidationError->size() << "\n";
  }

  void ComLoggerGTestBase ::
    assertEvents_FileValidationError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const validationFile,
        const char *const file,
        const U32 status
    ) const
  {
    ASSERT_GT(this->eventHistory_FileValidationError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileValidationError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileValidationError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileValidationError& e =
      this->eventHistory_FileValidationError->at(index);
    ASSERT_STREQ(validationFile, e.validationFile.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument validationFile at index "
      << index
      << " in history of event FileValidationError\n"
      << "  Expected: " << validationFile << "\n"
      << "  Actual:   " << e.validationFile.toChar() << "\n";
    ASSERT_STREQ(file, e.file.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument file at index "
      << index
      << " in history of event FileValidationError\n"
      << "  Expected: " << file << "\n"
      << "  Actual:   " << e.file.toChar() << "\n";
    ASSERT_EQ(status, e.status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument status at index "
      << index
      << " in history of event FileValidationError\n"
      << "  Expected: " << status << "\n"
      << "  Actual:   " << e.status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileClosed
  // ----------------------------------------------------------------------

  void ComLoggerGTestBase ::
    assertEvents_FileClosed_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileClosed->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileClosed\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileClosed->size() << "\n";
  }

  void ComLoggerGTestBase ::
    assertEvents_FileClosed(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const file
    ) const
  {
    ASSERT_GT(this->eventHistory_FileClosed->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileClosed\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileClosed->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileClosed& e =
      this->eventHistory_FileClosed->at(index);
    ASSERT_STREQ(file, e.file.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument file at index "
      << index
      << " in history of event FileClosed\n"
      << "  Expected: " << file << "\n"
      << "  Actual:   " << e.file.toChar() << "\n";
  }

} // end namespace Svc
