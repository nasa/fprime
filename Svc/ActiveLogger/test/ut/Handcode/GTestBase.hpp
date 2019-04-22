// ======================================================================
// \title  ActiveLogger/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for ActiveLogger component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef ActiveLogger_GTEST_BASE_HPP
#define ActiveLogger_GTEST_BASE_HPP

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

#define ASSERT_EVENTS_ALOG_FILE_WRITE_ERR_SIZE(size) \
  this->assertEvents_ALOG_FILE_WRITE_ERR_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_FILE_WRITE_ERR(index, _stage, _error) \
  this->assertEvents_ALOG_FILE_WRITE_ERR(__FILE__, __LINE__, index, _stage, _error)

#define ASSERT_EVENTS_ALOG_FILE_WRITE_COMPLETE_SIZE(size) \
  this->assertEvents_ALOG_FILE_WRITE_COMPLETE_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_FILE_WRITE_COMPLETE(index, _records) \
  this->assertEvents_ALOG_FILE_WRITE_COMPLETE(__FILE__, __LINE__, index, _records)

#define ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE_SIZE(size) \
  this->assertEvents_ALOG_SEVERITY_FILTER_STATE_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_SEVERITY_FILTER_STATE(index, _severity, _recvEnabled, _sendEnabled) \
  this->assertEvents_ALOG_SEVERITY_FILTER_STATE(__FILE__, __LINE__, index, _severity, _recvEnabled, _sendEnabled)

#define ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED_SIZE(size) \
  this->assertEvents_ALOG_ID_FILTER_ENABLED_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_ID_FILTER_ENABLED(index, _ID) \
  this->assertEvents_ALOG_ID_FILTER_ENABLED(__FILE__, __LINE__, index, _ID)

#define ASSERT_EVENTS_ALOG_ID_FILTER_LIST_FULL_SIZE(size) \
  this->assertEvents_ALOG_ID_FILTER_LIST_FULL_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_ID_FILTER_LIST_FULL(index, _ID) \
  this->assertEvents_ALOG_ID_FILTER_LIST_FULL(__FILE__, __LINE__, index, _ID)

#define ASSERT_EVENTS_ALOG_ID_FILTER_REMOVED_SIZE(size) \
  this->assertEvents_ALOG_ID_FILTER_REMOVED_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_ID_FILTER_REMOVED(index, _ID) \
  this->assertEvents_ALOG_ID_FILTER_REMOVED(__FILE__, __LINE__, index, _ID)

#define ASSERT_EVENTS_ALOG_ID_FILTER_NOT_FOUND_SIZE(size) \
  this->assertEvents_ALOG_ID_FILTER_NOT_FOUND_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_ALOG_ID_FILTER_NOT_FOUND(index, _ID) \
  this->assertEvents_ALOG_ID_FILTER_NOT_FOUND(__FILE__, __LINE__, index, _ID)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_PktSend_SIZE(size) \
  this->assert_from_PktSend_size(__FILE__, __LINE__, size)

#define ASSERT_from_PktSend(index, _data, _context) \
  { \
    ASSERT_GT(this->fromPortHistory_PktSend->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_PktSend\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_PktSend->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_PktSend& _e = \
      this->fromPortHistory_PktSend->at(index); \
    ASSERT_EQ(_data, _e.data) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument data at index " \
    << index \
    << " in history of from_PktSend\n" \
    << "  Expected: " << _data << "\n" \
    << "  Actual:   " << _e.data << "\n"; \
    ASSERT_EQ(_context, _e.context) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument context at index " \
    << index \
    << " in history of from_PktSend\n" \
    << "  Expected: " << _context << "\n" \
    << "  Actual:   " << _e.context << "\n"; \
  }

#define ASSERT_from_FatalAnnounce_SIZE(size) \
  this->assert_from_FatalAnnounce_size(__FILE__, __LINE__, size)

#define ASSERT_from_FatalAnnounce(index, _Id) \
  { \
    ASSERT_GT(this->fromPortHistory_FatalAnnounce->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_FatalAnnounce\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_FatalAnnounce->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_FatalAnnounce& _e = \
      this->fromPortHistory_FatalAnnounce->at(index); \
    ASSERT_EQ(_Id, _e.Id) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument Id at index " \
    << index \
    << " in history of from_FatalAnnounce\n" \
    << "  Expected: " << _Id << "\n" \
    << "  Actual:   " << _e.Id << "\n"; \
  }

namespace Svc {

  //! \class ActiveLoggerGTestBase
  //! \brief Auto-generated base class for ActiveLogger component Google Test harness
  //!
  class ActiveLoggerGTestBase :
    public ActiveLoggerTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ActiveLoggerGTestBase
      //!
      ActiveLoggerGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object ActiveLoggerGTestBase
      //!
      virtual ~ActiveLoggerGTestBase(void);

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
      // Event: ALOG_FILE_WRITE_ERR
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_FILE_WRITE_ERR_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_FILE_WRITE_ERR(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          ActiveLoggerComponentBase::LogWriteError stage, /*!< The write stage*/
          const I32 error /*!< The error code*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_FILE_WRITE_COMPLETE
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_FILE_WRITE_COMPLETE_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_FILE_WRITE_COMPLETE(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 records /*!< number of records written*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_SEVERITY_FILTER_STATE
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_SEVERITY_FILTER_STATE_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_SEVERITY_FILTER_STATE(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          ActiveLoggerComponentBase::EventFilterState severity, /*!< The severity level*/
          const bool recvEnabled, 
          const bool sendEnabled 
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_ENABLED
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_ID_FILTER_ENABLED_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_ID_FILTER_ENABLED(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 ID /*!< The ID filtered*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_LIST_FULL
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_ID_FILTER_LIST_FULL_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_ID_FILTER_LIST_FULL(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 ID /*!< The ID filtered*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_REMOVED
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_ID_FILTER_REMOVED_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_ID_FILTER_REMOVED(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 ID /*!< The ID removed*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: ALOG_ID_FILTER_NOT_FOUND
      // ----------------------------------------------------------------------

      void assertEvents_ALOG_ID_FILTER_NOT_FOUND_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_ALOG_ID_FILTER_NOT_FOUND(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32 ID /*!< The ID removed*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From ports 
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: PktSend 
      // ----------------------------------------------------------------------

      void assert_from_PktSend_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: FatalAnnounce 
      // ----------------------------------------------------------------------

      void assert_from_FatalAnnounce_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif
