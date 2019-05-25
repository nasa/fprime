// ======================================================================
// \title  FileUplink/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for FileUplink component Google Test harness base class
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

  FileUplinkGTestBase ::
    FileUplinkGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        FileUplinkTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  FileUplinkGTestBase ::
    ~FileUplinkGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
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
  // Channel: FileUplink_FilesReceived
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_FilesReceived_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileUplink_FilesReceived->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileUplink_FilesReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileUplink_FilesReceived->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_FilesReceived(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileUplink_FilesReceived->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileUplink_FilesReceived\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileUplink_FilesReceived->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileUplink_FilesReceived& e =
      this->tlmHistory_FileUplink_FilesReceived->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileUplink_FilesReceived\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: FileUplink_PacketsReceived
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_PacketsReceived_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileUplink_PacketsReceived->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileUplink_PacketsReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileUplink_PacketsReceived->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_PacketsReceived(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileUplink_PacketsReceived->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileUplink_PacketsReceived\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileUplink_PacketsReceived->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileUplink_PacketsReceived& e =
      this->tlmHistory_FileUplink_PacketsReceived->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileUplink_PacketsReceived\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: FileUplink_Warnings
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_Warnings_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_FileUplink_Warnings->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel FileUplink_Warnings\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_FileUplink_Warnings->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertTlm_FileUplink_Warnings(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_FileUplink_Warnings->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel FileUplink_Warnings\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_FileUplink_Warnings->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_FileUplink_Warnings& e =
      this->tlmHistory_FileUplink_Warnings->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel FileUplink_Warnings\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
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
  // Event: FileUplink_BadChecksum
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_BadChecksum_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_BadChecksum->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_BadChecksum\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_BadChecksum->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_BadChecksum(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName,
        const U32 computed,
        const U32 read
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_BadChecksum->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_BadChecksum\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_BadChecksum->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_BadChecksum& e =
      this->eventHistory_FileUplink_BadChecksum->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileUplink_BadChecksum\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
    ASSERT_EQ(computed, e.computed)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument computed at index "
      << index
      << " in history of event FileUplink_BadChecksum\n"
      << "  Expected: " << computed << "\n"
      << "  Actual:   " << e.computed << "\n";
    ASSERT_EQ(read, e.read)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument read at index "
      << index
      << " in history of event FileUplink_BadChecksum\n"
      << "  Expected: " << read << "\n"
      << "  Actual:   " << e.read << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileOpenError
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileOpenError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_FileOpenError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_FileOpenError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_FileOpenError->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileOpenError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_FileOpenError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_FileOpenError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_FileOpenError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_FileOpenError& e =
      this->eventHistory_FileUplink_FileOpenError->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileUplink_FileOpenError\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileReceived
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileReceived_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_FileReceived->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_FileReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_FileReceived->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileReceived(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_FileReceived->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_FileReceived\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_FileReceived->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_FileReceived& e =
      this->eventHistory_FileUplink_FileReceived->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileUplink_FileReceived\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_FileWriteError
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileWriteError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_FileWriteError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_FileWriteError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_FileWriteError->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_FileWriteError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_FileWriteError->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_FileWriteError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_FileWriteError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_FileWriteError& e =
      this->eventHistory_FileUplink_FileWriteError->at(index);
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileUplink_FileWriteError\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_InvalidReceiveMode
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_InvalidReceiveMode_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_InvalidReceiveMode->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_InvalidReceiveMode\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_InvalidReceiveMode->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_InvalidReceiveMode(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32 packetType,
        const U32 mode
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_InvalidReceiveMode->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_InvalidReceiveMode\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_InvalidReceiveMode->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_InvalidReceiveMode& e =
      this->eventHistory_FileUplink_InvalidReceiveMode->at(index);
    ASSERT_EQ(packetType, e.packetType)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument packetType at index "
      << index
      << " in history of event FileUplink_InvalidReceiveMode\n"
      << "  Expected: " << packetType << "\n"
      << "  Actual:   " << e.packetType << "\n";
    ASSERT_EQ(mode, e.mode)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument mode at index "
      << index
      << " in history of event FileUplink_InvalidReceiveMode\n"
      << "  Expected: " << mode << "\n"
      << "  Actual:   " << e.mode << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_PacketOutOfBounds
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_PacketOutOfBounds_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_PacketOutOfBounds->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_PacketOutOfBounds\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_PacketOutOfBounds->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_PacketOutOfBounds(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32 packetIndex,
        const char *const fileName
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_PacketOutOfBounds->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_PacketOutOfBounds\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_PacketOutOfBounds->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_PacketOutOfBounds& e =
      this->eventHistory_FileUplink_PacketOutOfBounds->at(index);
    ASSERT_EQ(packetIndex, e.packetIndex)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument packetIndex at index "
      << index
      << " in history of event FileUplink_PacketOutOfBounds\n"
      << "  Expected: " << packetIndex << "\n"
      << "  Actual:   " << e.packetIndex << "\n";
    ASSERT_STREQ(fileName, e.fileName.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument fileName at index "
      << index
      << " in history of event FileUplink_PacketOutOfBounds\n"
      << "  Expected: " << fileName << "\n"
      << "  Actual:   " << e.fileName.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_PacketOutOfOrder
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_PacketOutOfOrder_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_FileUplink_PacketOutOfOrder->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_PacketOutOfOrder\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_FileUplink_PacketOutOfOrder->size() << "\n";
  }

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_PacketOutOfOrder(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32 packetIndex,
        const U32 lastPacketIndex
    ) const
  {
    ASSERT_GT(this->eventHistory_FileUplink_PacketOutOfOrder->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event FileUplink_PacketOutOfOrder\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_FileUplink_PacketOutOfOrder->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_FileUplink_PacketOutOfOrder& e =
      this->eventHistory_FileUplink_PacketOutOfOrder->at(index);
    ASSERT_EQ(packetIndex, e.packetIndex)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument packetIndex at index "
      << index
      << " in history of event FileUplink_PacketOutOfOrder\n"
      << "  Expected: " << packetIndex << "\n"
      << "  Actual:   " << e.packetIndex << "\n";
    ASSERT_EQ(lastPacketIndex, e.lastPacketIndex)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument lastPacketIndex at index "
      << index
      << " in history of event FileUplink_PacketOutOfOrder\n"
      << "  Expected: " << lastPacketIndex << "\n"
      << "  Actual:   " << e.lastPacketIndex << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: FileUplink_UplinkCanceled
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
    assertEvents_FileUplink_UplinkCanceled_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_FileUplink_UplinkCanceled)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event FileUplink_UplinkCanceled\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_FileUplink_UplinkCanceled << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
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
  // From port: bufferSendOut
  // ----------------------------------------------------------------------

  void FileUplinkGTestBase ::
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

  void FileUplinkGTestBase ::
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
