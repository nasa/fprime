// ======================================================================
// \title  MathSender/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for MathSender component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef MathSender_GTEST_BASE_HPP
#define MathSender_GTEST_BASE_HPP

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

#define ASSERT_TLM_MS_VAL1_SIZE(size) \
  this->assertTlm_MS_VAL1_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MS_VAL1(index, value) \
  this->assertTlm_MS_VAL1(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MS_VAL2_SIZE(size) \
  this->assertTlm_MS_VAL2_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MS_VAL2(index, value) \
  this->assertTlm_MS_VAL2(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MS_OP_SIZE(size) \
  this->assertTlm_MS_OP_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MS_OP(index, value) \
  this->assertTlm_MS_OP(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MS_RES_SIZE(size) \
  this->assertTlm_MS_RES_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MS_RES(index, value) \
  this->assertTlm_MS_RES(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(size) \
  this->assertEvents_MS_COMMAND_RECV_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MS_COMMAND_RECV(index, _val1, _val2, _op) \
  this->assertEvents_MS_COMMAND_RECV(__FILE__, __LINE__, index, _val1, _val2, _op)

#define ASSERT_EVENTS_MS_RESULT_SIZE(size) \
  this->assertEvents_MS_RESULT_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MS_RESULT(index, _result) \
  this->assertEvents_MS_RESULT(__FILE__, __LINE__, index, _result)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_mathOut_SIZE(size) \
  this->assert_from_mathOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_mathOut(index, _val1, _val2, _operation) \
  { \
    ASSERT_GT(this->fromPortHistory_mathOut->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_mathOut\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_mathOut->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_mathOut& _e = \
      this->fromPortHistory_mathOut->at(index); \
    ASSERT_EQ(_val1, _e.val1) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument val1 at index " \
    << index \
    << " in history of from_mathOut\n" \
    << "  Expected: " << _val1 << "\n" \
    << "  Actual:   " << _e.val1 << "\n"; \
    ASSERT_EQ(_val2, _e.val2) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument val2 at index " \
    << index \
    << " in history of from_mathOut\n" \
    << "  Expected: " << _val2 << "\n" \
    << "  Actual:   " << _e.val2 << "\n"; \
    ASSERT_EQ(_operation, _e.operation) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument operation at index " \
    << index \
    << " in history of from_mathOut\n" \
    << "  Expected: " << _operation << "\n" \
    << "  Actual:   " << _e.operation << "\n"; \
  }

namespace Ref {

  //! \class MathSenderGTestBase
  //! \brief Auto-generated base class for MathSender component Google Test harness
  //!
  class MathSenderGTestBase :
    public MathSenderTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object MathSenderGTestBase
      //!
      MathSenderGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object MathSenderGTestBase
      //!
      virtual ~MathSenderGTestBase(void);

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
      // Channel: MS_VAL1
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MS_VAL1_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MS_VAL1(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MS_VAL2
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MS_VAL2_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MS_VAL2(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MS_OP
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MS_OP_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MS_OP(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const MathSenderComponentBase::MathOpTlm& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MS_RES
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MS_RES_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MS_RES(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32& val /*!< The channel value*/
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
      // Event: MS_COMMAND_RECV
      // ----------------------------------------------------------------------

      void assertEvents_MS_COMMAND_RECV_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MS_COMMAND_RECV(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32 val1, /*!< The val1 argument*/
          const F32 val2, /*!< The val1 argument*/
          MathSenderComponentBase::MathOpEv op /*!< The requested operation*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: MS_RESULT
      // ----------------------------------------------------------------------

      void assertEvents_MS_RESULT_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MS_RESULT(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32 result /*!< The math result*/
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
      // From port: mathOut 
      // ----------------------------------------------------------------------

      void assert_from_mathOut_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

  };

} // end namespace Ref

#endif
