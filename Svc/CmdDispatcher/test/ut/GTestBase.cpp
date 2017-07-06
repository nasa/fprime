// ======================================================================
// \title  CommandDispatcher/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for CommandDispatcher component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ======================================================================

#include "GTestBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  CommandDispatcherGTestBase ::
    CommandDispatcherGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        CommandDispatcherTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  CommandDispatcherGTestBase ::
    ~CommandDispatcherGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertCmdResponse_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ((unsigned long) size, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of command response history\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertCmdResponse(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history (" 
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << index
      << " in command resopnse history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertTlm_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: CommandsDispatched
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertTlm_CommandsDispatched_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_CommandsDispatched->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel CommandsDispatched\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_CommandsDispatched->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertTlm_CommandsDispatched(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_CommandsDispatched->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel CommandsDispatched\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_CommandsDispatched->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_CommandsDispatched& e =
      this->tlmHistory_CommandsDispatched->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel CommandsDispatched\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: CommandErrors
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertTlm_CommandErrors_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_CommandErrors->size(), size)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel CommandErrors\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_CommandErrors->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertTlm_CommandErrors(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_CommandErrors->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel CommandErrors\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_CommandErrors->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_CommandErrors& e =
      this->tlmHistory_CommandErrors->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel CommandErrors\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: OpCodeRegistered
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeRegistered_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_OpCodeRegistered->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event OpCodeRegistered\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_OpCodeRegistered->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeRegistered(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode,
        const I32 port,
        const I32 slot
    ) const
  {
    ASSERT_GT(this->eventHistory_OpCodeRegistered->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event OpCodeRegistered\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_OpCodeRegistered->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_OpCodeRegistered& e =
      this->eventHistory_OpCodeRegistered->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event OpCodeRegistered\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
    ASSERT_EQ(port, e.port)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument port at index "
      << index
      << " in history of event OpCodeRegistered\n"
      << "  Expected: " << port << "\n"
      << "  Actual:   " << e.port << "\n";
    ASSERT_EQ(slot, e.slot)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument slot at index "
      << index
      << " in history of event OpCodeRegistered\n"
      << "  Expected: " << slot << "\n"
      << "  Actual:   " << e.slot << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: OpCodeDispatched
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeDispatched_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_OpCodeDispatched->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event OpCodeDispatched\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_OpCodeDispatched->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeDispatched(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode,
        const I32 port
    ) const
  {
    ASSERT_GT(this->eventHistory_OpCodeDispatched->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event OpCodeDispatched\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_OpCodeDispatched->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_OpCodeDispatched& e =
      this->eventHistory_OpCodeDispatched->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event OpCodeDispatched\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
    ASSERT_EQ(port, e.port)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument port at index "
      << index
      << " in history of event OpCodeDispatched\n"
      << "  Expected: " << port << "\n"
      << "  Actual:   " << e.port << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: OpCodeCompleted
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeCompleted_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_OpCodeCompleted->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event OpCodeCompleted\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_OpCodeCompleted->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeCompleted(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode
    ) const
  {
    ASSERT_GT(this->eventHistory_OpCodeCompleted->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event OpCodeCompleted\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_OpCodeCompleted->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_OpCodeCompleted& e =
      this->eventHistory_OpCodeCompleted->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event OpCodeCompleted\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: OpCodeError
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeError_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_OpCodeError->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event OpCodeError\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_OpCodeError->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_OpCodeError(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode,
        CommandDispatcherComponentBase::ErrorResponse error
    ) const
  {
    ASSERT_GT(this->eventHistory_OpCodeError->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event OpCodeError\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_OpCodeError->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_OpCodeError& e =
      this->eventHistory_OpCodeError->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event OpCodeError\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
    ASSERT_EQ(error, e.error)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument error at index "
      << index
      << " in history of event OpCodeError\n"
      << "  Expected: " << error << "\n"
      << "  Actual:   " << e.error << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MalformedCommand
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_MalformedCommand_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MalformedCommand->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MalformedCommand\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MalformedCommand->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_MalformedCommand(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        CommandDispatcherComponentBase::CmdSerError Status
    ) const
  {
    ASSERT_GT(this->eventHistory_MalformedCommand->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MalformedCommand\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_MalformedCommand->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_MalformedCommand& e =
      this->eventHistory_MalformedCommand->at(index);
    ASSERT_EQ(Status, e.Status)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Status at index "
      << index
      << " in history of event MalformedCommand\n"
      << "  Expected: " << Status << "\n"
      << "  Actual:   " << e.Status << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: InvalidCommand
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_InvalidCommand_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_InvalidCommand->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event InvalidCommand\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_InvalidCommand->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_InvalidCommand(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode
    ) const
  {
    ASSERT_GT(this->eventHistory_InvalidCommand->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event InvalidCommand\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_InvalidCommand->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_InvalidCommand& e =
      this->eventHistory_InvalidCommand->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event InvalidCommand\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: TooManyCommands
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_TooManyCommands_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_TooManyCommands->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event TooManyCommands\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_TooManyCommands->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_TooManyCommands(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const U32 Opcode
    ) const
  {
    ASSERT_GT(this->eventHistory_TooManyCommands->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event TooManyCommands\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_TooManyCommands->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_TooManyCommands& e =
      this->eventHistory_TooManyCommands->at(index);
    ASSERT_EQ(Opcode, e.Opcode)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument Opcode at index "
      << index
      << " in history of event TooManyCommands\n"
      << "  Expected: " << Opcode << "\n"
      << "  Actual:   " << e.Opcode << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: NoOpReceived
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_NoOpReceived_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_NoOpReceived)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event NoOpReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_NoOpReceived << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: NoOpStringReceived
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_NoOpStringReceived_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_NoOpStringReceived->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event NoOpStringReceived\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_NoOpStringReceived->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_NoOpStringReceived(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const char *const No_op_string
    ) const
  {
    ASSERT_GT(this->eventHistory_NoOpStringReceived->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event NoOpStringReceived\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_NoOpStringReceived->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_NoOpStringReceived& e =
      this->eventHistory_NoOpStringReceived->at(index);
    ASSERT_STREQ(No_op_string, e.No_op_string.toChar())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument No_op_string at index "
      << index
      << " in history of event NoOpStringReceived\n"
      << "  Expected: " << No_op_string << "\n"
      << "  Actual:   " << e.No_op_string.toChar() << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: TestCmd1Args
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertEvents_TestCmd1Args_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_TestCmd1Args->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for event TestCmd1Args\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_TestCmd1Args->size() << "\n";
  }

  void CommandDispatcherGTestBase ::
    assertEvents_TestCmd1Args(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 index,
        const I32 arg1,
        const F32 arg2,
        const U8 arg3
    ) const
  {
    ASSERT_GT(this->eventHistory_TestCmd1Args->size(), index)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Index into history of event TestCmd1Args\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_TestCmd1Args->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_TestCmd1Args& e =
      this->eventHistory_TestCmd1Args->at(index);
    ASSERT_EQ(arg1, e.arg1)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg1 at index "
      << index
      << " in history of event TestCmd1Args\n"
      << "  Expected: " << arg1 << "\n"
      << "  Actual:   " << e.arg1 << "\n";
    ASSERT_EQ(arg2, e.arg2)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg2 at index "
      << index
      << " in history of event TestCmd1Args\n"
      << "  Expected: " << arg2 << "\n"
      << "  Actual:   " << e.arg2 << "\n";
    ASSERT_EQ(arg3, e.arg3)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Value of argument arg3 at index "
      << index
      << " in history of event TestCmd1Args\n"
      << "  Expected: " << arg3 << "\n"
      << "  Actual:   " << e.arg3 << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assertFromPortHistory_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: compCmdSend
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assert_from_compCmdSend_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_compCmdSend->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_compCmdSend\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_compCmdSend->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: seqCmdStatus
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assert_from_seqCmdStatus_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_seqCmdStatus->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_seqCmdStatus\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_seqCmdStatus->size() << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: pingOut
  // ----------------------------------------------------------------------

  void CommandDispatcherGTestBase ::
    assert_from_pingOut_size(
        const char *const __ISF_callSiteFileName,
        const U32 __ISF_callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_pingOut->size())
      << "\n"
      << "  File:     " << __ISF_callSiteFileName << "\n"
      << "  Line:     " << __ISF_callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_pingOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_pingOut->size() << "\n";
  }

} // end namespace Svc
