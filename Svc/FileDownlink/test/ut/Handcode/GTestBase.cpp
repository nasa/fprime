// ======================================================================
// \title  FileDownlink/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for FileDownlink component Google Test harness base class
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

  FileDownlinkGTestBase ::
    FileDownlinkGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        FileDownlinkTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  FileDownlinkGTestBase ::
    ~FileDownlinkGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertCmdResponse_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ((unsigned long) size, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of command response history\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertCmdResponse(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history (" 
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << index
      << " in command response history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertTlm_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: FileDownlink_FilesSent
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_FilesSent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileDownlink_FilesSent->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileDownlink_FilesSent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileDownlink_FilesSent->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_FilesSent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileDownlink_FilesSent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileDownlink_FilesSent\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileDownlink_FilesSent->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileDownlink_FilesSent& e =
      this->tlmHistory_FileDownlink_FilesSent->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileDownlink_FilesSent\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: FileDownlink_PacketsSent
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_PacketsSent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileDownlink_PacketsSent->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileDownlink_PacketsSent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileDownlink_PacketsSent->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_PacketsSent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileDownlink_PacketsSent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileDownlink_PacketsSent\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileDownlink_PacketsSent->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileDownlink_PacketsSent& e =
      this->tlmHistory_FileDownlink_PacketsSent->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileDownlink_PacketsSent\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: FileDownlink_Warnings
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_Warnings_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileDownlink_Warnings->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileDownlink_Warnings\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileDownlink_Warnings->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertTlm_FileDownlink_Warnings(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileDownlink_Warnings->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileDownlink_Warnings\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileDownlink_Warnings->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileDownlink_Warnings& e =
      this->tlmHistory_FileDownlink_Warnings->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileDownlink_Warnings\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertEvents_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileOpenError
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileOpenError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileDownlink_FileOpenError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileDownlink_FileOpenError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileDownlink_FileOpenError->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileOpenError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileDownlink_FileOpenError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileDownlink_FileOpenError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileDownlink_FileOpenError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileDownlink_FileOpenError& e =
      this->eventHistory_FileDownlink_FileOpenError->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileDownlink_FileOpenError\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileReadError
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileReadError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileDownlink_FileReadError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileDownlink_FileReadError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileDownlink_FileReadError->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileReadError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileDownlink_FileReadError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileDownlink_FileReadError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileDownlink_FileReadError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileDownlink_FileReadError& e =
      this->eventHistory_FileDownlink_FileReadError->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileDownlink_FileReadError\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_FileSent
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileSent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileDownlink_FileSent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileDownlink_FileSent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileDownlink_FileSent->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_FileSent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const sourceFileName,
        const char *const destFileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileDownlink_FileSent->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileDownlink_FileSent\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileDownlink_FileSent->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileDownlink_FileSent& e =
      this->eventHistory_FileDownlink_FileSent->at(index);
    ASSERT_STREQ(sourceFileName, e.sourceFileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument sourceFileName at index "
      << index
      << " in history of event FileDownlink_FileSent\n"
      << "  Expected: " << sourceFileName << "\n"
      << "  Actual:   " << e.sourceFileName.toChar() << "\n";
    ASSERT_STREQ(destFileName, e.destFileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument destFileName at index "
      << index
      << " in history of event FileDownlink_FileSent\n"
      << "  Expected: " << destFileName << "\n"
      << "  Actual:   " << e.destFileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileDownlink_DownlinkCanceled
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_DownlinkCanceled_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileDownlink_DownlinkCanceled->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileDownlink_DownlinkCanceled\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileDownlink_DownlinkCanceled->size() << "\n";
  }

  void FileDownlinkGTestBase ::
    assertEvents_FileDownlink_DownlinkCanceled(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const sourceFileName,
        const char *const destFileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileDownlink_DownlinkCanceled->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileDownlink_DownlinkCanceled\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileDownlink_DownlinkCanceled->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileDownlink_DownlinkCanceled& e =
      this->eventHistory_FileDownlink_DownlinkCanceled->at(index);
    ASSERT_STREQ(sourceFileName, e.sourceFileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument sourceFileName at index "
      << index
      << " in history of event FileDownlink_DownlinkCanceled\n"
      << "  Expected: " << sourceFileName << "\n"
      << "  Actual:   " << e.sourceFileName.toChar() << "\n";
    ASSERT_STREQ(destFileName, e.destFileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument destFileName at index "
      << index
      << " in history of event FileDownlink_DownlinkCanceled\n"
      << "  Expected: " << destFileName << "\n"
      << "  Actual:   " << e.destFileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assertFromPortHistory_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: bufferGetCaller
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assert_from_bufferGetCaller_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_bufferGetCaller->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_bufferGetCaller\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_bufferGetCaller->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: bufferSendOut
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assert_from_bufferSendOut_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_bufferSendOut->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_bufferSendOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_bufferSendOut->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: pingOut
  // ----------------------------------------------------------------------

  void FileDownlinkGTestBase ::
    assert_from_pingOut_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_pingOut->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_pingOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_pingOut->size() << "\n";
  }

} // end namespace Svc
