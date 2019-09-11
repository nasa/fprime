// ======================================================================
// \title  UdpReceiver/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for UdpReceiver component Google Test harness base class
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

  UdpReceiverGTestBase ::
    UdpReceiverGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        UdpReceiverTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  UdpReceiverGTestBase ::
    ~UdpReceiverGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
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
  // Channel: UR_PacketsReceived
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertTlm_UR_PacketsReceived_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_UR_PacketsReceived->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel UR_PacketsReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_UR_PacketsReceived->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertTlm_UR_PacketsReceived(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_UR_PacketsReceived->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel UR_PacketsReceived\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_UR_PacketsReceived->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_UR_PacketsReceived& e =
      this->tlmHistory_UR_PacketsReceived->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel UR_PacketsReceived\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: UR_BytesReceived
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertTlm_UR_BytesReceived_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_UR_BytesReceived->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel UR_BytesReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_UR_BytesReceived->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertTlm_UR_BytesReceived(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_UR_BytesReceived->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel UR_BytesReceived\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_UR_BytesReceived->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_UR_BytesReceived& e =
      this->tlmHistory_UR_BytesReceived->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel UR_BytesReceived\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: UR_PacketsDropped
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertTlm_UR_PacketsDropped_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_UR_PacketsDropped->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel UR_PacketsDropped\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_UR_PacketsDropped->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertTlm_UR_PacketsDropped(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_UR_PacketsDropped->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel UR_PacketsDropped\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_UR_PacketsDropped->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_UR_PacketsDropped& e =
      this->tlmHistory_UR_PacketsDropped->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel UR_PacketsDropped\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: UR_DecodeErrors
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertTlm_UR_DecodeErrors_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_UR_DecodeErrors->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel UR_DecodeErrors\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_UR_DecodeErrors->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertTlm_UR_DecodeErrors(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_UR_DecodeErrors->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel UR_DecodeErrors\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_UR_DecodeErrors->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_UR_DecodeErrors& e =
      this->tlmHistory_UR_DecodeErrors->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel UR_DecodeErrors\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
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
  // Event: UR_PortOpened
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_PortOpened_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_PortOpened->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_PortOpened\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_PortOpened->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_PortOpened(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32 port
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_PortOpened->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_PortOpened\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_PortOpened->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_PortOpened& e =
      this->eventHistory_UR_PortOpened->at(__index);
    ASSERT_EQ(port, e.port)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument port at index "
      << __index
      << " in history of event UR_PortOpened\n"
      << "  Expected: " << port << "\n"
      << "  Actual:   " << e.port << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: UR_SocketError
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_SocketError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_SocketError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_SocketError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_SocketError->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_SocketError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const error
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_SocketError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_SocketError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_SocketError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_SocketError& e =
      this->eventHistory_UR_SocketError->at(__index);
    ASSERT_STREQ(error, e.error.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event UR_SocketError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: UR_BindError
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_BindError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_BindError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_BindError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_BindError->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_BindError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const error
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_BindError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_BindError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_BindError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_BindError& e =
      this->eventHistory_UR_BindError->at(__index);
    ASSERT_STREQ(error, e.error.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event UR_BindError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: UR_RecvError
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_RecvError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_RecvError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_RecvError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_RecvError->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_RecvError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const error
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_RecvError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_RecvError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_RecvError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_RecvError& e =
      this->eventHistory_UR_RecvError->at(__index);
    ASSERT_STREQ(error, e.error.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event UR_RecvError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: UR_DecodeError
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_DecodeError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_DecodeError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_DecodeError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_DecodeError->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_DecodeError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        UdpReceiverComponentBase::DecodeStage stage,
        const I32 error
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_DecodeError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_DecodeError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_DecodeError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_DecodeError& e =
      this->eventHistory_UR_DecodeError->at(__index);
    ASSERT_EQ(stage, e.stage)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument stage at index "
      << __index
      << " in history of event UR_DecodeError\n"
      << "  Expected: " << stage << "\n"
      << "  Actual:   " << e.stage << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event UR_DecodeError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: UR_DroppedPacket
  // ----------------------------------------------------------------------

  void UdpReceiverGTestBase ::
    assertEvents_UR_DroppedPacket_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_UR_DroppedPacket->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event UR_DroppedPacket\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_UR_DroppedPacket->size() << "\n";
  }

  void UdpReceiverGTestBase ::
    assertEvents_UR_DroppedPacket(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32 diff
    ) const
  {
    ASSERT_GT(this->eventHistory_UR_DroppedPacket->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event UR_DroppedPacket\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_UR_DroppedPacket->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_UR_DroppedPacket& e =
      this->eventHistory_UR_DroppedPacket->at(__index);
    ASSERT_EQ(diff, e.diff)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument diff at index "
      << __index
      << " in history of event UR_DroppedPacket\n"
      << "  Expected: " << diff << "\n"
      << "  Actual:   " << e.diff << "\n";
  }

} // end namespace Svc
