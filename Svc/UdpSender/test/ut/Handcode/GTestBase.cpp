// ======================================================================
// \title  UdpSender/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for UdpSender component Google Test harness base class
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

  UdpSenderGTestBase ::
    UdpSenderGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        UdpSenderTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  UdpSenderGTestBase ::
    ~UdpSenderGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
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
  // Channel: US_PacketsSent
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
    assertTlm_US_PacketsSent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_US_PacketsSent->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel US_PacketsSent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_US_PacketsSent->size() << "\n";
  }

  void UdpSenderGTestBase ::
    assertTlm_US_PacketsSent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_US_PacketsSent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel US_PacketsSent\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_US_PacketsSent->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_US_PacketsSent& e =
      this->tlmHistory_US_PacketsSent->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel US_PacketsSent\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: US_BytesSent
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
    assertTlm_US_BytesSent_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_US_BytesSent->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel US_BytesSent\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_US_BytesSent->size() << "\n";
  }

  void UdpSenderGTestBase ::
    assertTlm_US_BytesSent(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const U32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_US_BytesSent->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel US_BytesSent\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_US_BytesSent->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_US_BytesSent& e =
      this->tlmHistory_US_BytesSent->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel US_BytesSent\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
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
  // Event: US_PortOpened
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
    assertEvents_US_PortOpened_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_US_PortOpened->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event US_PortOpened\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_US_PortOpened->size() << "\n";
  }

  void UdpSenderGTestBase ::
    assertEvents_US_PortOpened(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const server,
        const U32 port
    ) const
  {
    ASSERT_GT(this->eventHistory_US_PortOpened->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event US_PortOpened\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_US_PortOpened->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_US_PortOpened& e =
      this->eventHistory_US_PortOpened->at(__index);
    ASSERT_STREQ(server, e.server.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument server at index "
      << __index
      << " in history of event US_PortOpened\n"
      << "  Expected: " << server << "\n"
      << "  Actual:   " << e.server.toChar() << "\n";
    ASSERT_EQ(port, e.port)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument port at index "
      << __index
      << " in history of event US_PortOpened\n"
      << "  Expected: " << port << "\n"
      << "  Actual:   " << e.port << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: US_SocketError
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
    assertEvents_US_SocketError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_US_SocketError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event US_SocketError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_US_SocketError->size() << "\n";
  }

  void UdpSenderGTestBase ::
    assertEvents_US_SocketError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const error
    ) const
  {
    ASSERT_GT(this->eventHistory_US_SocketError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event US_SocketError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_US_SocketError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_US_SocketError& e =
      this->eventHistory_US_SocketError->at(__index);
    ASSERT_STREQ(error, e.error.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event US_SocketError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: US_SendError
  // ----------------------------------------------------------------------

  void UdpSenderGTestBase ::
    assertEvents_US_SendError_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_US_SendError->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event US_SendError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_US_SendError->size() << "\n";
  }

  void UdpSenderGTestBase ::
    assertEvents_US_SendError(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const char *const error
    ) const
  {
    ASSERT_GT(this->eventHistory_US_SendError->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event US_SendError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_US_SendError->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_US_SendError& e =
      this->eventHistory_US_SendError->at(__index);
    ASSERT_STREQ(error, e.error.toChar())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << __index
      << " in history of event US_SendError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error.toChar() << "\n";
  }

} // end namespace Svc
