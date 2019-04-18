// ======================================================================
// \title  FileDownlink/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for FileDownlink component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FileDownlink_GTEST_BASE_HPP
#define FileDownlink_GTEST_BASE_HPP

#include "TesterBase.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Macros for command history assertions
// ----------------------------------------------------------------------

#define ASSERT_CMD_RESPONSE_SIZE(size) \
  this->assertCmdResponse_size(__FILE__, __LINE__, size)

#define ASSERT_CMD_RESPONSE(index, opCode, cmdSeq, response) \
  this->assertCmdResponse(__FILE__, __LINE__, index, opCode, cmdSeq, response)

// ----------------------------------------------------------------------
// Macros for telemetry history assertions
// ----------------------------------------------------------------------

#define ASSERT_TLM_SIZE(size) \
  this->assertTlm_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileDownlink_FilesSent_SIZE(size) \
  this->assertTlm_FileDownlink_FilesSent_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileDownlink_FilesSent(index, value) \
  this->assertTlm_FileDownlink_FilesSent(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_FileDownlink_PacketsSent_SIZE(size) \
  this->assertTlm_FileDownlink_PacketsSent_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileDownlink_PacketsSent(index, value) \
  this->assertTlm_FileDownlink_PacketsSent(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_FileDownlink_Warnings_SIZE(size) \
  this->assertTlm_FileDownlink_Warnings_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_FileDownlink_Warnings(index, value) \
  this->assertTlm_FileDownlink_Warnings(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileDownlink_FileOpenError_SIZE(size) \
  this->assertEvents_FileDownlink_FileOpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileDownlink_FileOpenError(index, _fileName) \
  this->assertEvents_FileDownlink_FileOpenError(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_FileDownlink_FileReadError_SIZE(size) \
  this->assertEvents_FileDownlink_FileReadError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileDownlink_FileReadError(index, _fileName) \
  this->assertEvents_FileDownlink_FileReadError(__FILE__, __LINE__, index, _fileName)

#define ASSERT_EVENTS_FileDownlink_FileSent_SIZE(size) \
  this->assertEvents_FileDownlink_FileSent_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileDownlink_FileSent(index, _sourceFileName, _destFileName) \
  this->assertEvents_FileDownlink_FileSent(__FILE__, __LINE__, index, _sourceFileName, _destFileName)

#define ASSERT_EVENTS_FileDownlink_DownlinkCanceled_SIZE(size) \
  this->assertEvents_FileDownlink_DownlinkCanceled_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileDownlink_DownlinkCanceled(index, _sourceFileName, _destFileName) \
  this->assertEvents_FileDownlink_DownlinkCanceled(__FILE__, __LINE__, index, _sourceFileName, _destFileName)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_bufferGetCaller_SIZE(size) \
  this->assert_from_bufferGetCaller_size(__FILE__, __LINE__, size)

#define ASSERT_from_bufferGetCaller(index, _size) \
  { \
    ASSERT_GT(this->fromPortHistory_bufferGetCaller->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_bufferGetCaller\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_bufferGetCaller->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_bufferGetCaller& _e = \
      this->fromPortHistory_bufferGetCaller->at(index); \
    ASSERT_EQ(_size, _e.size) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument size at index " \
    << index \
    << " in history of from_bufferGetCaller\n" \
    << "  Expected: " << _size << "\n" \
    << "  Actual:   " << _e.size << "\n"; \
  }

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

  //! \class FileDownlinkGTestBase
  //! \brief Auto-generated base class for FileDownlink component Google Test harness
  //!
  class FileDownlinkGTestBase :
    public FileDownlinkTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileDownlinkGTestBase
      //!
      FileDownlinkGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object FileDownlinkGTestBase
      //!
      virtual ~FileDownlinkGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Commands
      // ----------------------------------------------------------------------

      //! Assert size of command response history
      //!
      void assertCmdResponse_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      //! Assert command response in history at index
      //!
      void assertCmdResponse(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CommandResponse response /*!< The command response*/
      ) const;

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
      // Channel: FileDownlink_FilesSent
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileDownlink_FilesSent_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileDownlink_FilesSent(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileDownlink_PacketsSent
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileDownlink_PacketsSent_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileDownlink_PacketsSent(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: FileDownlink_Warnings
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_FileDownlink_Warnings_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_FileDownlink_Warnings(
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
      // Event: FileDownlink_FileOpenError
      // ----------------------------------------------------------------------

      void assertEvents_FileDownlink_FileOpenError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileDownlink_FileOpenError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_FileReadError
      // ----------------------------------------------------------------------

      void assertEvents_FileDownlink_FileReadError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileDownlink_FileReadError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName /*!< The name of the file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_FileSent
      // ----------------------------------------------------------------------

      void assertEvents_FileDownlink_FileSent_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileDownlink_FileSent(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const sourceFileName, /*!< The source file name*/
          const char *const destFileName /*!< The destination file name*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileDownlink_DownlinkCanceled
      // ----------------------------------------------------------------------

      void assertEvents_FileDownlink_DownlinkCanceled_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileDownlink_DownlinkCanceled(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const sourceFileName, /*!< The source file name*/
          const char *const destFileName /*!< The destination file name*/
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
      // From port: bufferGetCaller 
      // ----------------------------------------------------------------------

      void assert_from_bufferGetCaller_size(
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
