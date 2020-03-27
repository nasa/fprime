// ======================================================================
// \title  CommandDispatcher/test/ut/GTestBase.hpp
// \author Auto-generated
// \brief  hpp file for CommandDispatcher component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CommandDispatcher_GTEST_BASE_HPP
#define CommandDispatcher_GTEST_BASE_HPP

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

#define ASSERT_TLM_CommandsDispatched_SIZE(size) \
  this->assertTlm_CommandsDispatched_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CommandsDispatched(index, value) \
  this->assertTlm_CommandsDispatched(__FILE__, __LINE__, index, value)

#define ASSERT_TLM_CommandErrors_SIZE(size) \
  this->assertTlm_CommandErrors_size(__FILE__, __LINE__, size)

#define ASSERT_TLM_CommandErrors(index, value) \
  this->assertTlm_CommandErrors(__FILE__, __LINE__, index, value)

// ----------------------------------------------------------------------
// Macros for event history assertions
// ----------------------------------------------------------------------

#define ASSERT_EVENTS_SIZE(size) \
  this->assertEvents_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_OpCodeRegistered_SIZE(size) \
  this->assertEvents_OpCodeRegistered_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_OpCodeRegistered(index, _Opcode, _port, _slot) \
  this->assertEvents_OpCodeRegistered(__FILE__, __LINE__, index, _Opcode, _port, _slot)

#define ASSERT_EVENTS_OpCodeDispatched_SIZE(size) \
  this->assertEvents_OpCodeDispatched_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_OpCodeDispatched(index, _Opcode, _port) \
  this->assertEvents_OpCodeDispatched(__FILE__, __LINE__, index, _Opcode, _port)

#define ASSERT_EVENTS_OpCodeCompleted_SIZE(size) \
  this->assertEvents_OpCodeCompleted_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_OpCodeCompleted(index, _Opcode) \
  this->assertEvents_OpCodeCompleted(__FILE__, __LINE__, index, _Opcode)

#define ASSERT_EVENTS_OpCodeError_SIZE(size) \
  this->assertEvents_OpCodeError_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_OpCodeError(index, _Opcode, _error) \
  this->assertEvents_OpCodeError(__FILE__, __LINE__, index, _Opcode, _error)

#define ASSERT_EVENTS_MalformedCommand_SIZE(size) \
  this->assertEvents_MalformedCommand_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_MalformedCommand(index, _Status) \
  this->assertEvents_MalformedCommand(__FILE__, __LINE__, index, _Status)

#define ASSERT_EVENTS_InvalidCommand_SIZE(size) \
  this->assertEvents_InvalidCommand_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_InvalidCommand(index, _Opcode) \
  this->assertEvents_InvalidCommand(__FILE__, __LINE__, index, _Opcode)

#define ASSERT_EVENTS_TooManyCommands_SIZE(size) \
  this->assertEvents_TooManyCommands_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_TooManyCommands(index, _Opcode) \
  this->assertEvents_TooManyCommands(__FILE__, __LINE__, index, _Opcode)

#define ASSERT_EVENTS_NoOpReceived_SIZE(size) \
  this->assertEvents_NoOpReceived_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_NoOpStringReceived_SIZE(size) \
  this->assertEvents_NoOpStringReceived_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_NoOpStringReceived(index, _message) \
  this->assertEvents_NoOpStringReceived(__FILE__, __LINE__, index, _message)

#define ASSERT_EVENTS_TestCmd1Args_SIZE(size) \
  this->assertEvents_TestCmd1Args_size(__FILE__, __LINE__, size)

#define ASSERT_EVENTS_TestCmd1Args(index, _arg1, _arg2, _arg3) \
  this->assertEvents_TestCmd1Args(__FILE__, __LINE__, index, _arg1, _arg2, _arg3)

// ----------------------------------------------------------------------
// Macros for typed user from port history assertions
// ----------------------------------------------------------------------

#define ASSERT_FROM_PORT_HISTORY_SIZE(size) \
  this->assertFromPortHistory_size(__FILE__, __LINE__, size)

#define ASSERT_from_compCmdSend_SIZE(size) \
  this->assert_from_compCmdSend_size(__FILE__, __LINE__, size)

#define ASSERT_from_compCmdSend(index, _opCode, _cmdSeq, _args) \
  { \
    ASSERT_GT(this->fromPortHistory_compCmdSend->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_compCmdSend\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_compCmdSend->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_compCmdSend& _e = \
      this->fromPortHistory_compCmdSend->at(index); \
    ASSERT_EQ(_opCode, _e.opCode) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument opCode at index " \
    << index \
    << " in history of from_compCmdSend\n" \
    << "  Expected: " << _opCode << "\n" \
    << "  Actual:   " << _e.opCode << "\n"; \
    ASSERT_EQ(_cmdSeq, _e.cmdSeq) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cmdSeq at index " \
    << index \
    << " in history of from_compCmdSend\n" \
    << "  Expected: " << _cmdSeq << "\n" \
    << "  Actual:   " << _e.cmdSeq << "\n"; \
    ASSERT_EQ(_args, _e.args) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument args at index " \
    << index \
    << " in history of from_compCmdSend\n" \
    << "  Expected: " << _args << "\n" \
    << "  Actual:   " << _e.args << "\n"; \
  }

#define ASSERT_from_seqCmdStatus_SIZE(size) \
  this->assert_from_seqCmdStatus_size(__FILE__, __LINE__, size)

#define ASSERT_from_seqCmdStatus(index, _opCode, _cmdSeq, _response) \
  { \
    ASSERT_GT(this->fromPortHistory_seqCmdStatus->size(), static_cast<U32>(index)) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Index into history of from_seqCmdStatus\n" \
    << "  Expected: Less than size of history (" \
    << this->fromPortHistory_seqCmdStatus->size() << ")\n" \
    << "  Actual:   " << index << "\n"; \
    const FromPortEntry_seqCmdStatus& _e = \
      this->fromPortHistory_seqCmdStatus->at(index); \
    ASSERT_EQ(_opCode, _e.opCode) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument opCode at index " \
    << index \
    << " in history of from_seqCmdStatus\n" \
    << "  Expected: " << _opCode << "\n" \
    << "  Actual:   " << _e.opCode << "\n"; \
    ASSERT_EQ(_cmdSeq, _e.cmdSeq) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument cmdSeq at index " \
    << index \
    << " in history of from_seqCmdStatus\n" \
    << "  Expected: " << _cmdSeq << "\n" \
    << "  Actual:   " << _e.cmdSeq << "\n"; \
    ASSERT_EQ(_response, _e.response) \
    << "\n" \
    << "  File:     " << __FILE__ << "\n" \
    << "  Line:     " << __LINE__ << "\n" \
    << "  Value:    Value of argument response at index " \
    << index \
    << " in history of from_seqCmdStatus\n" \
    << "  Expected: " << _response << "\n" \
    << "  Actual:   " << _e.response << "\n"; \
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

  //! \class CommandDispatcherGTestBase
  //! \brief Auto-generated base class for CommandDispatcher component Google Test harness
  //!
  class CommandDispatcherGTestBase :
    public CommandDispatcherTesterBase
  {

    protected:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object CommandDispatcherGTestBase
      //!
      CommandDispatcherGTestBase(
#if FW_OBJECT_NAMES == 1
          const char *const compName, /*!< The component name*/
          const U32 maxHistorySize /*!< The maximum size of each history*/
#else
          const U32 maxHistorySize /*!< The maximum size of each history*/
#endif
      );

      //! Destroy object CommandDispatcherGTestBase
      //!
      virtual ~CommandDispatcherGTestBase(void);

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
      // Channel: CommandsDispatched
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CommandsDispatched_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CommandsDispatched(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32& val /*!< The channel value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Channel: CommandErrors
      // ----------------------------------------------------------------------

      //! Assert telemetry value in history at index
      //!
      void assertTlm_CommandErrors_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertTlm_CommandErrors(
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
      // Event: OpCodeRegistered
      // ----------------------------------------------------------------------

      void assertEvents_OpCodeRegistered_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_OpCodeRegistered(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode, /*!< The opcode to register*/
          const I32 port, /*!< The registration port*/
          const I32 slot /*!< The dispatch slot it was placed in*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeDispatched
      // ----------------------------------------------------------------------

      void assertEvents_OpCodeDispatched_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_OpCodeDispatched(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode, /*!< The opcode dispatched*/
          const I32 port /*!< The port dispatched to*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeCompleted
      // ----------------------------------------------------------------------

      void assertEvents_OpCodeCompleted_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_OpCodeCompleted(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode /*!< The I32 command argument*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: OpCodeError
      // ----------------------------------------------------------------------

      void assertEvents_OpCodeError_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_OpCodeError(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode, /*!< The opcode with the error*/
          CommandDispatcherComponentBase::ErrorResponse error /*!< The error value*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: MalformedCommand
      // ----------------------------------------------------------------------

      void assertEvents_MalformedCommand_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_MalformedCommand(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          CommandDispatcherComponentBase::CmdSerError Status /*!< The deserialization error*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: InvalidCommand
      // ----------------------------------------------------------------------

      void assertEvents_InvalidCommand_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_InvalidCommand(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode /*!< Invalid opcode*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: TooManyCommands
      // ----------------------------------------------------------------------

      void assertEvents_TooManyCommands_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_TooManyCommands(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const U32 Opcode /*!< The opcode that overflowed the list*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: NoOpReceived
      // ----------------------------------------------------------------------

      void assertEvents_NoOpReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: NoOpStringReceived
      // ----------------------------------------------------------------------

      void assertEvents_NoOpStringReceived_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_NoOpStringReceived(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const char *const message /*!< The NO-OP string that is generated*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // Event: TestCmd1Args
      // ----------------------------------------------------------------------

      void assertEvents_TestCmd1Args_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

      void assertEvents_TestCmd1Args(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 __index, /*!< The index*/
          const I32 arg1, /*!< Arg1*/
          const F32 arg2, /*!< Arg2*/
          const U8 arg3 /*!< Arg3*/
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
      // From port: compCmdSend
      // ----------------------------------------------------------------------

      void assert_from_compCmdSend_size(
          const char *const __callSiteFileName, /*!< The name of the file containing the call site*/
          const U32 __callSiteLineNumber, /*!< The line number of the call site*/
          const U32 size /*!< The asserted size*/
      ) const;

    protected:

      // ----------------------------------------------------------------------
      // From port: seqCmdStatus
      // ----------------------------------------------------------------------

      void assert_from_seqCmdStatus_size(
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
