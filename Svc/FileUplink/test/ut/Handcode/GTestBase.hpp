// ======================================================================
// \title  FileUplink/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for FileUplink component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FileUplink_GTEST_BASE_HPP
#define FileUplink_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileUplink_FilesReceived_SIZE(size) \
  this->assertTlm_FileUplink_FilesReceived_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileUplink_FilesReceived(index, value) \
  this->assertTlm_FileUplink_FilesReceived(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_FileUplink_PacketsReceived_SIZE(size) \
  this->assertTlm_FileUplink_PacketsReceived_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileUplink_PacketsReceived(index, value) \
  this->assertTlm_FileUplink_PacketsReceived(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_FileUplink_Warnings_SIZE(size) \
  this->assertTlm_FileUplink_Warnings_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileUplink_Warnings(index, value) \
  this->assertTlm_FileUplink_Warnings(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_BadChecksum_SIZE(size) \
  this->assertEvents_FileUplink_BadChecksum_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_BadChecksum(index, _fileName, _computed, _read) \
  this->assertEvents_FileUplink_BadChecksum(__FILE__, __LINE__, index, _fileName, _computed, _read)

#define ASSERT_EVENTS_FileUplink_FileOpenError_SIZE(size) \
  this->assertEvents_FileUplink_FileOpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_FileOpenError(index, _fileName) \
  this->assertEvents_FileUplink_FileOpenError(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_FileUplink_FileReceived_SIZE(size) \
  this->assertEvents_FileUplink_FileReceived_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_FileReceived(index, _fileName) \
  this->assertEvents_FileUplink_FileReceived(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_FileUplink_FileWriteError_SIZE(size) \
  this->assertEvents_FileUplink_FileWriteError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_FileWriteError(index, _fileName) \
  this->assertEvents_FileUplink_FileWriteError(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_FileUplink_InvalidReceiveMode_SIZE(size) \
  this->assertEvents_FileUplink_InvalidReceiveMode_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_InvalidReceiveMode(index, _packetType, _mode) \
  this->assertEvents_FileUplink_InvalidReceiveMode(__FILE__, __LINE__, index, _packetType, _mode)

#define ASSERT_EVENTS_FileUplink_PacketOutOfBounds_SIZE(size) \
  this->assertEvents_FileUplink_PacketOutOfBounds_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_PacketOutOfBounds(index, _packetIndex, _fileName) \
  this->assertEvents_FileUplink_PacketOutOfBounds(__FILE__, __LINE__, index, _packetIndex, _fileName)

#define ASSERT_EVENTS_FileUplink_PacketOutOfOrder_SIZE(size) \
  this->assertEvents_FileUplink_PacketOutOfOrder_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileUplink_PacketOutOfOrder(index, _packetIndex, _lastPacketIndex) \
  this->assertEvents_FileUplink_PacketOutOfOrder(__FILE__, __LINE__, index, _packetIndex, _lastPacketIndex)

#define ASSERT_EVENTS_FileUplink_UplinkCanceled_SIZE(size) \
  this->assertEvents_FileUplink_UplinkCanceled_size(__FILE__, __LINE__, size)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_bufferSendOut_SIZE(size) \
  this->assert_from_bufferSendOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_bufferSendOut(index, _fwBuffer) \
  { \
    ASSERT_GT(this->fromPortHistory_bufferSendOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_bufferSendOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_bufferSendOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_bufferSendOut& _e = \
      this->fromPortHistory_bufferSendOut->at(index); \
    ASSERT_EQ(_fwBuffer, _e.fwBuffer) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument fwBuffer at index " \
    << index \
    << " in history of from_bufferSendOut\n" \
    << "  Expected: " << _fwBuffer << "\n" \
    << "  Actual:   " << _e.fwBuffer << "\n"; \
  }

#define ASSERT_from_pingOut_SIZE(size) \
  this->assert_from_pingOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_pingOut(index, _key) \
  { \
    ASSERT_GT(this->fromPortHistory_pingOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_pingOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_pingOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_pingOut& _e = \
      this->fromPortHistory_pingOut->at(index); \
    ASSERT_EQ(_key, _e.key) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument key at index " \
    << index \
    << " in history of from_pingOut\n" \
    << "  Expected: " << _key << "\n" \
    << "  Actual:   " << _e.key << "\n"; \
  }

namespace Svc {

  //! \class FileUplinkGTestBase
  //! \brief Auto-generated base class for FileUplink component Google Test harness
  //!
  class FileUplinkGTestBase :
    public FileUplinkTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileUplinkGTestBase
      //!
      FileUplinkGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object FileUplinkGTestBase
      //!
      virtual ~FileUplinkGTestBase(void);

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
      // Channel: FileUplink_FilesReceived
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileUplink_FilesReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileUplink_FilesReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileUplink_PacketsReceived
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileUplink_PacketsReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileUplink_PacketsReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileUplink_Warnings
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileUplink_Warnings_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileUplink_Warnings(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
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
      // Event: FileUplink_BadChecksum
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_BadChecksum_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_BadChecksum(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The file name*/
          const U32 computed, /*!< The computed value*/
          const U32 read /*!< The value read*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileOpenError
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_FileOpenError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_FileOpenError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileReceived
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_FileReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_FileReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_FileWriteError
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_FileWriteError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_FileWriteError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_InvalidReceiveMode
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_InvalidReceiveMode_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_InvalidReceiveMode(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 packetType, /*!< The type of the packet received*/
          const U32 mode /*!< The receive mode*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_PacketOutOfBounds
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_PacketOutOfBounds_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_PacketOutOfBounds(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 packetIndex, /*!< The sequence index of the packet*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_PacketOutOfOrder
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_PacketOutOfOrder_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileUplink_PacketOutOfOrder(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 packetIndex, /*!< The sequence index of the out-of-order packet*/
          const U32 lastPacketIndex /*!< The sequence index of the last packet received before the out-of-order packet*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileUplink_UplinkCanceled
      // ----------------------------------------------------------------------

      void assertEvents_FileUplink_UplinkCanceled_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From ports 
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: bufferSendOut 
      // ----------------------------------------------------------------------

      void assert_from_bufferSendOut_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: pingOut 
      // ----------------------------------------------------------------------

      void assert_from_pingOut_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif
