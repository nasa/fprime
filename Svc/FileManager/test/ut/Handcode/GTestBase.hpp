// ======================================================================
// \title  FileManager/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for FileManager component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FileManager_GTEST_BASE_HPP
#define FileManager_GTEST_BASE_HPP

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

#define ASSERT_TLM_CommandsExecuted_SIZE(size) \
  this->assertTlm_CommandsExecuted_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CommandsExecuted(index, value) \
  this->assertTlm_CommandsExecuted(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_Errors_SIZE(size) \
  this->assertTlm_Errors_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_Errors(index, value) \
  this->assertTlm_Errors(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DirectoryCreateError_SIZE(size) \
  this->assertEvents_DirectoryCreateError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DirectoryCreateError(index, _dirName, _status) \
  this->assertEvents_DirectoryCreateError(__FILE__, __LINE__, index, _dirName, _status)

#define ASSERT_EVENTS_DirectoryRemoveError_SIZE(size) \
  this->assertEvents_DirectoryRemoveError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_DirectoryRemoveError(index, _dirName, _status) \
  this->assertEvents_DirectoryRemoveError(__FILE__, __LINE__, index, _dirName, _status)

#define ASSERT_EVENTS_FileMoveError_SIZE(size) \
  this->assertEvents_FileMoveError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileMoveError(index, _sourceFileName, _destFileName, _status) \
  this->assertEvents_FileMoveError(__FILE__, __LINE__, index, _sourceFileName, _destFileName, _status)

#define ASSERT_EVENTS_FileRemoveError_SIZE(size) \
  this->assertEvents_FileRemoveError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_FileRemoveError(index, _fileName, _status) \
  this->assertEvents_FileRemoveError(__FILE__, __LINE__, index, _fileName, _status)

#define ASSERT_EVENTS_ShellCommandFailed_SIZE(size) \
  this->assertEvents_ShellCommandFailed_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ShellCommandFailed(index, _command, _status) \
  this->assertEvents_ShellCommandFailed(__FILE__, __LINE__, index, _command, _status)

#define ASSERT_EVENTS_ShellCommandSucceeded_SIZE(size) \
  this->assertEvents_ShellCommandSucceeded_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ShellCommandSucceeded(index, _command) \
  this->assertEvents_ShellCommandSucceeded(__FILE__, __LINE__, index, _command)

namespace Svc {

  //! \class FileManagerGTestBase
  //! \brief Auto-generated base class for FileManager component Google Test harness
  //!
  class FileManagerGTestBase :
    public FileManagerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileManagerGTestBase
      //!
      FileManagerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object FileManagerGTestBase
      //!
      virtual ~FileManagerGTestBase(void);

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
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CommandsExecuted
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CommandsExecuted_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CommandsExecuted(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: Errors
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_Errors_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_Errors(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
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
      // Event: DirectoryCreateError
      // ----------------------------------------------------------------------

      void assertEvents_DirectoryCreateError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DirectoryCreateError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const dirName, /*!< The name of the directory*/
          const U32 status /*!< The error status*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: DirectoryRemoveError
      // ----------------------------------------------------------------------

      void assertEvents_DirectoryRemoveError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_DirectoryRemoveError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const dirName, /*!< The name of the directory*/
          const U32 status /*!< The error status*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileMoveError
      // ----------------------------------------------------------------------

      void assertEvents_FileMoveError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileMoveError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const sourceFileName, /*!< The name of the source file*/
          const char *const destFileName, /*!< The name of the destination file*/
          const U32 status /*!< The error status*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: FileRemoveError
      // ----------------------------------------------------------------------

      void assertEvents_FileRemoveError_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_FileRemoveError(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const fileName, /*!< The name of the file*/
          const U32 status /*!< The error status*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ShellCommandFailed
      // ----------------------------------------------------------------------

      void assertEvents_ShellCommandFailed_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ShellCommandFailed(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const command, /*!< The command string*/
          const U32 status /*!< The status code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ShellCommandSucceeded
      // ----------------------------------------------------------------------

      void assertEvents_ShellCommandSucceeded_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ShellCommandSucceeded(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const command /*!< The command string*/
      ) const;

  };

} // end namespace Svc

#endif
