// ======================================================================
// \title  Health/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for Health component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Health_GTEST_BASE_HPP
#define Health_GTEST_BASE_HPP

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

#define ASSERT_TLM_PingLateWarnings_SIZE(size) \
  this->assertTlm_PingLateWarnings_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_PingLateWarnings(index, value) \
  this->assertTlm_PingLateWarnings(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_WARN_SIZE(size) \
  this->assertEvents_HLTH_PING_WARN_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_WARN(index, _entry) \
  this->assertEvents_HLTH_PING_WARN(__FILE__, __LINE__, index, _entry)

#define ASSERT_EVENTS_HLTH_PING_LATE_SIZE(size) \
  this->assertEvents_HLTH_PING_LATE_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_LATE(index, _entry) \
  this->assertEvents_HLTH_PING_LATE(__FILE__, __LINE__, index, _entry)

#define ASSERT_EVENTS_HLTH_PING_WRONG_KEY_SIZE(size) \
  this->assertEvents_HLTH_PING_WRONG_KEY_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_WRONG_KEY(index, _entry, _badKey) \
  this->assertEvents_HLTH_PING_WRONG_KEY(__FILE__, __LINE__, index, _entry, _badKey)

#define ASSERT_EVENTS_HLTH_CHECK_ENABLE_SIZE(size) \
  this->assertEvents_HLTH_CHECK_ENABLE_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_CHECK_ENABLE(index, _enabled) \
  this->assertEvents_HLTH_CHECK_ENABLE(__FILE__, __LINE__, index, _enabled)

#define ASSERT_EVENTS_HLTH_CHECK_PING_SIZE(size) \
  this->assertEvents_HLTH_CHECK_PING_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_CHECK_PING(index, _enabled, _entry) \
  this->assertEvents_HLTH_CHECK_PING(__FILE__, __LINE__, index, _enabled, _entry)

#define ASSERT_EVENTS_HLTH_CHECK_LOOKUP_ERROR_SIZE(size) \
  this->assertEvents_HLTH_CHECK_LOOKUP_ERROR_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_CHECK_LOOKUP_ERROR(index, _entry) \
  this->assertEvents_HLTH_CHECK_LOOKUP_ERROR(__FILE__, __LINE__, index, _entry)

#define ASSERT_EVENTS_HLTH_PING_UPDATED_SIZE(size) \
  this->assertEvents_HLTH_PING_UPDATED_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_UPDATED(index, _entry, _warn, _fatal) \
  this->assertEvents_HLTH_PING_UPDATED(__FILE__, __LINE__, index, _entry, _warn, _fatal)

#define ASSERT_EVENTS_HLTH_PING_INVALID_VALUES_SIZE(size) \
  this->assertEvents_HLTH_PING_INVALID_VALUES_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_HLTH_PING_INVALID_VALUES(index, _entry, _warn, _fatal) \
  this->assertEvents_HLTH_PING_INVALID_VALUES(__FILE__, __LINE__, index, _entry, _warn, _fatal)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_PingSend_SIZE(size) \
  this->assert_from_PingSend_size(__FILE__, __LINE__, size)

#define ASSERT_from_PingSend(index, _key) \
  { \
    ASSERT_GT(this->fromPortHistory_PingSend->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_PingSend\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_PingSend->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_PingSend& _e = \
      this->fromPortHistory_PingSend->at(index); \
    ASSERT_EQ(_key, _e.key) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument key at index " \
    << index \
    << " in history of from_PingSend\n" \
    << "  Expected: " << _key << "\n" \
    << "  Actual:   " << _e.key << "\n"; \
  }

#define ASSERT_from_WdogStroke_SIZE(size) \
  this->assert_from_WdogStroke_size(__FILE__, __LINE__, size)

#define ASSERT_from_WdogStroke(index, _code) \
  { \
    ASSERT_GT(this->fromPortHistory_WdogStroke->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_WdogStroke\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_WdogStroke->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_WdogStroke& _e = \
      this->fromPortHistory_WdogStroke->at(index); \
    ASSERT_EQ(_code, _e.code) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument code at index " \
    << index \
    << " in history of from_WdogStroke\n" \
    << "  Expected: " << _code << "\n" \
    << "  Actual:   " << _e.code << "\n"; \
  }

namespace Svc {

  //! \class HealthGTestBase
  //! \brief Auto-generated base class for Health component Google Test harness
  //!
  class HealthGTestBase :
    public HealthTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object HealthGTestBase
      //!
      HealthGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object HealthGTestBase
      //!
      virtual ~HealthGTestBase(void);

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
      // Channel: PingLateWarnings
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_PingLateWarnings_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_PingLateWarnings(
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
      // Event: HLTH_PING_WARN
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_PING_WARN_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_PING_WARN(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry /*!< The entry passing the warning level*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_LATE
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_PING_LATE_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_PING_LATE(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry /*!< The entry passing the warning level*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_WRONG_KEY
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_PING_WRONG_KEY_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_PING_WRONG_KEY(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry, /*!< The entry passing the warning level*/
          const U32 badKey /*!< The incorrect key value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_ENABLE
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_CHECK_ENABLE_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_CHECK_ENABLE(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          HealthComponentBase::HealthIsEnabled enabled /*!< If health checking is enabled*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_PING
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_CHECK_PING_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_CHECK_PING(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          HealthComponentBase::HealthPingIsEnabled enabled, /*!< If health pinging is enabled for a particular entry*/
          const char *const entry /*!< The entry passing the warning level*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_CHECK_LOOKUP_ERROR
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_CHECK_LOOKUP_ERROR_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_CHECK_LOOKUP_ERROR(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry /*!< The entry passing the warning level*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_UPDATED
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_PING_UPDATED_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_PING_UPDATED(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry, /*!< The entry changed*/
          const U32 warn, /*!< The new warning value*/
          const U32 fatal /*!< The new FATAL value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: HLTH_PING_INVALID_VALUES
      // ----------------------------------------------------------------------

      void assertEvents_HLTH_PING_INVALID_VALUES_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_HLTH_PING_INVALID_VALUES(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const char *const entry, /*!< The entry changed*/
          const U32 warn, /*!< The new warning value*/
          const U32 fatal /*!< The new FATAL value*/
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
      // From port: PingSend 
      // ----------------------------------------------------------------------

      void assert_from_PingSend_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: WdogStroke 
      // ----------------------------------------------------------------------

      void assert_from_WdogStroke_size(
          const char *const __ISF_callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __ISF_callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Svc

#endif
