// ======================================================================
// \title  UdpReceiver/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for UdpReceiver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpReceiver_GTEST_BASE_HPP
#define UdpReceiver_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_UR_PacketsReceived_SIZE(size) \
  this->assertTlm_UR_PacketsReceived_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_UR_PacketsReceived(index, value) \
  this->assertTlm_UR_PacketsReceived(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_UR_BytesReceived_SIZE(size) \
  this->assertTlm_UR_BytesReceived_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_UR_BytesReceived(index, value) \
  this->assertTlm_UR_BytesReceived(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_UR_PacketsDropped_SIZE(size) \
  this->assertTlm_UR_PacketsDropped_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_UR_PacketsDropped(index, value) \
  this->assertTlm_UR_PacketsDropped(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_UR_DecodeErrors_SIZE(size) \
  this->assertTlm_UR_DecodeErrors_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_UR_DecodeErrors(index, value) \
  this->assertTlm_UR_DecodeErrors(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_PortOpened_SIZE(size) \
  this->assertEvents_UR_PortOpened_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_PortOpened(index, _port) \
  this->assertEvents_UR_PortOpened(__FILE__, __LINE__, index, _port)

#define ASSERT_EVENTS_UR_SocketError_SIZE(size) \
  this->assertEvents_UR_SocketError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_SocketError(index, _error) \
  this->assertEvents_UR_SocketError(__FILE__, __LINE__, index, _error)

#define ASSERT_EVENTS_UR_BindError_SIZE(size) \
  this->assertEvents_UR_BindError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_BindError(index, _error) \
  this->assertEvents_UR_BindError(__FILE__, __LINE__, index, _error)

#define ASSERT_EVENTS_UR_RecvError_SIZE(size) \
  this->assertEvents_UR_RecvError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_RecvError(index, _error) \
  this->assertEvents_UR_RecvError(__FILE__, __LINE__, index, _error)

#define ASSERT_EVENTS_UR_DecodeError_SIZE(size) \
  this->assertEvents_UR_DecodeError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_DecodeError(index, _stage, _error) \
  this->assertEvents_UR_DecodeError(__FILE__, __LINE__, index, _stage, _error)

#define ASSERT_EVENTS_UR_DroppedPacket_SIZE(size) \
  this->assertEvents_UR_DroppedPacket_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_UR_DroppedPacket(index, _diff) \
  this->assertEvents_UR_DroppedPacket(__FILE__, __LINE__, index, _diff)

namespace Svc {

  //! \class UdpReceiverGTestBase
  //! \brief Auto-generated base class for UdpReceiver component Google Test harness
  //!
  class UdpReceiverGTestBase :
    public UdpReceiverTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object UdpReceiverGTestBase
      //!
      UdpReceiverGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object UdpReceiverGTestBase
      //!
      virtual ~UdpReceiverGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_PacketsReceived
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_UR_PacketsReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_UR_PacketsReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_BytesReceived
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_UR_BytesReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_UR_BytesReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_PacketsDropped
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_UR_PacketsDropped_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_UR_PacketsDropped(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: UR_DecodeErrors
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_UR_DecodeErrors_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_UR_DecodeErrors(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_PortOpened
      // ----------------------------------------------------------------------

      void assertEvents_UR_PortOpened_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_PortOpened(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 port 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_SocketError
      // ----------------------------------------------------------------------

      void assertEvents_UR_SocketError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_SocketError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const error 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_BindError
      // ----------------------------------------------------------------------

      void assertEvents_UR_BindError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_BindError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const error 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_RecvError
      // ----------------------------------------------------------------------

      void assertEvents_UR_RecvError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_RecvError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const error 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_DecodeError
      // ----------------------------------------------------------------------

      void assertEvents_UR_DecodeError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_DecodeError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          UdpReceiverComponentBase::DecodeStage stage, 
          const I32 error 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: UR_DroppedPacket
      // ----------------------------------------------------------------------

      void assertEvents_UR_DroppedPacket_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_UR_DroppedPacket(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 diff 
      ) const;

  };

} // end namespace Svc

#endif
