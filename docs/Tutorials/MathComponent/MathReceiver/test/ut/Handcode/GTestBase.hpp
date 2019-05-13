// ======================================================================
// \title  MathReceiver/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for MathReceiver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef MathReceiver_GTEST_BASE_HPP
#define MathReceiver_GTEST_BASE_HPP

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

#define ASSERT_TLM_MR_OPERATION_SIZE(size) \
  this->assertTlm_MR_OPERATION_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MR_OPERATION(index, value) \
  this->assertTlm_MR_OPERATION(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MR_FACTOR1S_SIZE(size) \
  this->assertTlm_MR_FACTOR1S_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MR_FACTOR1S(index, value) \
  this->assertTlm_MR_FACTOR1S(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MR_FACTOR1_SIZE(size) \
  this->assertTlm_MR_FACTOR1_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MR_FACTOR1(index, value) \
  this->assertTlm_MR_FACTOR1(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_MR_FACTOR2_SIZE(size) \
  this->assertTlm_MR_FACTOR2_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_MR_FACTOR2(index, value) \
  this->assertTlm_MR_FACTOR2(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions 
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(size) \
  this->assertEvents_MR_SET_FACTOR1_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MR_SET_FACTOR1(index, _val) \
  this->assertEvents_MR_SET_FACTOR1(__FILE__, __LINE__, index, _val)

#define ASSERT_EVENTS_MR_UPDATED_FACTOR2_SIZE(size) \
  this->assertEvents_MR_UPDATED_FACTOR2_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MR_UPDATED_FACTOR2(index, _val) \
  this->assertEvents_MR_UPDATED_FACTOR2(__FILE__, __LINE__, index, _val)

#define ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(size) \
  this->assertEvents_MR_OPERATION_PERFORMED_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MR_OPERATION_PERFORMED(index, _val) \
  this->assertEvents_MR_OPERATION_PERFORMED(__FILE__, __LINE__, index, _val)

#define ASSERT_EVENTS_MR_THROTTLE_CLEARED_SIZE(size) \
  this->assertEvents_MR_THROTTLE_CLEARED_size(__FILE__, __LINE__, size)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_mathOut_SIZE(size) \
  this->assert_from_mathOut_size(__FILE__, __LINE__, size)

#define ASSERT_from_mathOut(index, _result) \
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
    ASSERT_EQ(_result, _e.result) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument result at index " \
    << index \
    << " in history of from_mathOut\n" \
    << "  Expected: " << _result << "\n" \
    << "  Actual:   " << _e.result << "\n"; \
  }

namespace Ref {

  //! \class MathReceiverGTestBase
  //! \brief Auto-generated base class for MathReceiver component Google Test harness
  //!
  class MathReceiverGTestBase :
    public MathReceiverTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object MathReceiverGTestBase
      //!
      MathReceiverGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object MathReceiverGTestBase
      //!
      virtual ~MathReceiverGTestBase(void);

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
      // Channel: MR_OPERATION
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MR_OPERATION_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MR_OPERATION(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const Ref::MathOp& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MR_FACTOR1S
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MR_FACTOR1S_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MR_FACTOR1S(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MR_FACTOR1
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MR_FACTOR1_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MR_FACTOR1(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: MR_FACTOR2
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_MR_FACTOR2_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_MR_FACTOR2(
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
      // Event: MR_SET_FACTOR1
      // ----------------------------------------------------------------------

      void assertEvents_MR_SET_FACTOR1_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MR_SET_FACTOR1(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32 val /*!< The factor value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: MR_UPDATED_FACTOR2
      // ----------------------------------------------------------------------

      void assertEvents_MR_UPDATED_FACTOR2_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MR_UPDATED_FACTOR2(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const F32 val /*!< The factor value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: MR_OPERATION_PERFORMED
      // ----------------------------------------------------------------------

      void assertEvents_MR_OPERATION_PERFORMED_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MR_OPERATION_PERFORMED(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 index, /*!< The index*/
          const Ref::MathOp val /*!< The operation*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: MR_THROTTLE_CLEARED
      // ----------------------------------------------------------------------

      void assertEvents_MR_THROTTLE_CLEARED_size(
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
