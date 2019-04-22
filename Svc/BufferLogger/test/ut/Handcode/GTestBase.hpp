// ======================================================================
// \title  BufferLogger/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for BufferLogger component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferLogger_GTEST_BASE_HPP
#define BufferLogger_GTEST_BASE_HPP

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

#define ASSERT_TLM_BufferLogger_NumLoggedBuffers_SIZE(size) \
  this->assertTlm_BufferLogger_NumLoggedBuffers_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_BufferLogger_NumLoggedBuffers(index, value) \
  this->assertTlm_BufferLogger_NumLoggedBuffers(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_LogFileClosed_SIZE(size) \
  this->assertEvents_BL_LogFileClosed_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_LogFileClosed(index, _file) \
  this->assertEvents_BL_LogFileClosed(__FILE__, __LINE__, index, _file)

#define ASSERT_EVENTS_BL_LogFileOpenError_SIZE(size) \
  this->assertEvents_BL_LogFileOpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_LogFileOpenError(index, _errornum, _file) \
  this->assertEvents_BL_LogFileOpenError(__FILE__, __LINE__, index, _errornum, _file)

#define ASSERT_EVENTS_BL_LogFileValidationError_SIZE(size) \
  this->assertEvents_BL_LogFileValidationError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_LogFileValidationError(index, _validationFile, _status) \
  this->assertEvents_BL_LogFileValidationError(__FILE__, __LINE__, index, _validationFile, _status)

#define ASSERT_EVENTS_BL_LogFileWriteError_SIZE(size) \
  this->assertEvents_BL_LogFileWriteError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_LogFileWriteError(index, _errornum, _bytesWritten, _bytesToWrite, _file) \
  this->assertEvents_BL_LogFileWriteError(__FILE__, __LINE__, index, _errornum, _bytesWritten, _bytesToWrite, _file)

#define ASSERT_EVENTS_BL_Activated_SIZE(size) \
  this->assertEvents_BL_Activated_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_Deativated_SIZE(size) \
  this->assertEvents_BL_Deativated_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_BL_NoLogFileOpenInitError_SIZE(size) \
  this->assertEvents_BL_NoLogFileOpenInitError_size(__FILE__, __LINE__, size)

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

  //! \class BufferLoggerGTestBase
  //! \brief Auto-generated base class for BufferLogger component Google Test harness
  //!
  class BufferLoggerGTestBase :
    public BufferLoggerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object BufferLoggerGTestBase
      //!
      BufferLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object BufferLoggerGTestBase
      //!
      virtual ~BufferLoggerGTestBase(void);

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
          const U32 __index, /*!< The index*/
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
      // Channel: BufferLogger_NumLoggedBuffers
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_BufferLogger_NumLoggedBuffers_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_BufferLogger_NumLoggedBuffers(
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
      // Event: BL_LogFileClosed
      // ----------------------------------------------------------------------

      void assertEvents_BL_LogFileClosed_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_BL_LogFileClosed(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const file /*!< The file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileOpenError
      // ----------------------------------------------------------------------

      void assertEvents_BL_LogFileOpenError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_BL_LogFileOpenError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 errornum, /*!< The error number returned from the open operation*/
          const char *const file /*!< The file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileValidationError
      // ----------------------------------------------------------------------

      void assertEvents_BL_LogFileValidationError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_BL_LogFileValidationError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const validationFile, /*!< The validation file*/
          const U32 status /*!< The Os::Validate::Status return*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_LogFileWriteError
      // ----------------------------------------------------------------------

      void assertEvents_BL_LogFileWriteError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_BL_LogFileWriteError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 errornum, /*!< The error number returned from the write operation*/
          const U32 bytesWritten, /*!< The number of bytes successfully written*/
          const U32 bytesToWrite, /*!< The number of bytes attempted*/
          const char *const file /*!< The file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_Activated
      // ----------------------------------------------------------------------

      void assertEvents_BL_Activated_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_Deativated
      // ----------------------------------------------------------------------

      void assertEvents_BL_Deativated_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: BL_NoLogFileOpenInitError
      // ----------------------------------------------------------------------

      void assertEvents_BL_NoLogFileOpenInitError_size(
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
