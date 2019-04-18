// ======================================================================
// \title  Example/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for Example component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_GTEST_BASE_HPP
#define Example_GTEST_BASE_HPP

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

#define ASSERT_TLM_somechan_SIZE(size) \
  this->assertTlm_somechan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_somechan(index, value) \
  this->assertTlm_somechan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_anotherchan_SIZE(size) \
  this->assertTlm_anotherchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_anotherchan(index, value) \
  this->assertTlm_anotherchan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_stringchan_SIZE(size) \
  this->assertTlm_stringchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_stringchan(index, value) \
  this->assertTlm_stringchan(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_enumchan_SIZE(size) \
  this->assertTlm_enumchan_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_enumchan(index, value) \
  this->assertTlm_enumchan(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent_SIZE(size) \
  this->assertEvents_SomeEvent_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_SomeEvent(index, _arg1, _arg2, _arg3, _arg4, _arg5) \
  this->assertEvents_SomeEvent(__FILE__, __LINE__, index, _arg1, _arg2, _arg3, _arg4, _arg5)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_exampleOutput_SIZE(size) \
  this->assert_from_exampleOutput_size(__FILE__, __LINE__, size)

#define ASSERT_from_exampleOutput(index, _arg1, _arg2, _arg3, _arg4, _arg5) \
  { \
    ASSERT_GT(this->fromPortHistory_exampleOutput->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_exampleOutput\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_exampleOutput->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_exampleOutput& _e = \
      this->fromPortHistory_exampleOutput->at(index); \
    ASSERT_EQ(_arg1, _e.arg1) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg1 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg1 << "\n" \
    << "  Actual:   " << _e.arg1 << "\n"; \
    ASSERT_EQ(_arg2, _e.arg2) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg2 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg2 << "\n" \
    << "  Actual:   " << _e.arg2 << "\n"; \
    ASSERT_EQ(_arg3, _e.arg3) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg3 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg3 << "\n" \
    << "  Actual:   " << _e.arg3 << "\n"; \
    ASSERT_EQ(_arg4, _e.arg4) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg4 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg4 << "\n" \
    << "  Actual:   " << _e.arg4 << "\n"; \
    ASSERT_EQ(_arg5, _e.arg5) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument arg5 at index " \
    << index \
    << " in history of from_exampleOutput\n" \
    << "  Expected: " << _arg5 << "\n" \
    << "  Actual:   " << _e.arg5 << "\n"; \
  }

namespace ExampleComponents {

  //! \class ExampleGTestBase
  //! \brief Auto-generated base class for Example component Google Test harness
  //!
  class ExampleGTestBase :
    public ExampleTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object ExampleGTestBase
      //!
      ExampleGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, //!< The component name
          const U32 maxHistorySize //!< The maximum size of each history
#else
          const U32 maxHistorySize //!< The maximum size of each history
#endif
      );

      //! Destroy object ExampleGTestBase
      //!
      virtual ~ExampleGTestBase(void);

    protected:

      // ----------------------------------------------------------------------
      // Commands
      // ----------------------------------------------------------------------

      //! Assert size of command response history
      //!
      void assertCmdResponse_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      //! Assert command response in history at index
      //!
      void assertCmdResponse(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CommandResponse response //!< The command response
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Telemetry
      // ----------------------------------------------------------------------

      //! Assert size of telemetry history
      //!
      void assertTlm_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: somechan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_somechan_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertTlm_somechan(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const U32& val //!< The channel value
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: anotherchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_anotherchan_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertTlm_anotherchan(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const U32& val //!< The channel value
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: stringchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_stringchan_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertTlm_stringchan(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const U32& val //!< The channel value
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: enumchan
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_enumchan_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertTlm_enumchan(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const ExampleComponentBase::SomeTlmEnum& val //!< The channel value
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Events
      // ----------------------------------------------------------------------

      void assertEvents_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: SomeEvent
      // ----------------------------------------------------------------------

      void assertEvents_SomeEvent_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

      void assertEvents_SomeEvent(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 index, //!< The index
          const I32 arg1, //!< The I32 event argument
          const F32 arg2, //!< The F32 event argument
          const U32 arg3, //!< The U32 event argument
          const char *const arg4, //!< The string event argument
          ExampleComponentBase::SomeEventEnum arg5 //!< The enum event argument
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From ports
      // ----------------------------------------------------------------------

      void assertFromPortHistory_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: exampleOutput
      // ----------------------------------------------------------------------

      void assert_from_exampleOutput_size(
          const char *const __ISF_callSiteFileName, //!< The name of the file containing the call site
          const U32 __ISF_callSiteLineNumber, //!< The line number of the call site
          const U32 size //!< The asserted size
      ) const;

  };

} // end namespace ExampleComponents

#endif
