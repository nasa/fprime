// ======================================================================
// \title  ComLogger/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for ComLogger component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ComLogger_GTEST_BASE_HPP
#define ComLogger_GTEST_BASE_HPP

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
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileOpenError_SIZE(size) \
  this->assertEvents_FileOpenError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileOpenError(index, _errornum, _file) \
  this->assertEvents_FileOpenError(__FILE__, __LINE__, index, _errornum, _file)

#define ASSERT_EVENTS_FileWriteError_SIZE(size) \
  this->assertEvents_FileWriteError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileWriteError(index, _errornum, _bytesWritten, _bytesToWrite, _file) \
  this->assertEvents_FileWriteError(__FILE__, __LINE__, index, _errornum, _bytesWritten, _bytesToWrite, _file)

#define ASSERT_EVENTS_FileValidationError_SIZE(size) \
  this->assertEvents_FileValidationError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileValidationError(index, _validationFile, _file, _status) \
  this->assertEvents_FileValidationError(__FILE__, __LINE__, index, _validationFile, _file, _status)

#define ASSERT_EVENTS_FileClosed_SIZE(size) \
  this->assertEvents_FileClosed_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileClosed(index, _file) \
  this->assertEvents_FileClosed(__FILE__, __LINE__, index, _file)

namespace Svc {

  //! \class ComLoggerGTestBase
  //! \brief Auto-generated base class for ComLogger component Google Test harness
  //!
  class ComLoggerGTestBase :
    public ComLoggerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ComLoggerGTestBase
      //!
      ComLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ComLoggerGTestBase
      //!
      virtual ~ComLoggerGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Commands
      // ----------------------------------------------------------------------

      //! Assert size of command response history
      //!
      void assertCmdResponse_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      //! Assert command response in history at index
      //!
      void assertCmdResponse(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CommandResponse response /*!< The command response*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileOpenError
      // ----------------------------------------------------------------------

      void assertEvents_FileOpenError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileOpenError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 errornum, /*!< The error number returned from open file*/
          const char *const file /*!< The file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileWriteError
      // ----------------------------------------------------------------------

      void assertEvents_FileWriteError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileWriteError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 errornum, /*!< The error number returned from write file*/
          const U32 bytesWritten, /*!< The number of bytes successfully written to file*/
          const U32 bytesToWrite, /*!< The number of bytes attempted to write to file*/
          const char *const file /*!< The file*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileValidationError
      // ----------------------------------------------------------------------

      void assertEvents_FileValidationError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileValidationError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const validationFile, /*!< The validation file*/
          const char *const file, /*!< The file*/
          const U32 status /*!< The Os::Validate::Status return*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileClosed
      // ----------------------------------------------------------------------

      void assertEvents_FileClosed_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileClosed(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const file /*!< The file*/
      ) const;

  };

} // end namespace Svc

#endif
