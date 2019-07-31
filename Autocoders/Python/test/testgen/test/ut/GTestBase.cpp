#include "GTestBase.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  MathSenderGTestBase ::
    MathSenderGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
        MathSenderTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  MathSenderGTestBase ::
    ~MathSenderGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertCmdResponse_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ((unsigned long) size, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of command response history\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertCmdResponse(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(__index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history ("
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(__index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << __index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << __index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << __index
      << " in command response history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertTlm_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->tlmSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all telemetry histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MS_VAL1
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertTlm_MS_VAL1_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MS_VAL1->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MS_VAL1\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MS_VAL1->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertTlm_MS_VAL1(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const F32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_MS_VAL1->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MS_VAL1\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_MS_VAL1->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_MS_VAL1& e =
      this->tlmHistory_MS_VAL1->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel MS_VAL1\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MS_VAL2
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertTlm_MS_VAL2_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MS_VAL2->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MS_VAL2\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MS_VAL2->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertTlm_MS_VAL2(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const F32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_MS_VAL2->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MS_VAL2\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_MS_VAL2->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_MS_VAL2& e =
      this->tlmHistory_MS_VAL2->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel MS_VAL2\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MS_OP
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertTlm_MS_OP_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MS_OP->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MS_OP\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MS_OP->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertTlm_MS_OP(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const MathSenderComponentBase::MathOpTlm& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_MS_OP->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MS_OP\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_MS_OP->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_MS_OP& e =
      this->tlmHistory_MS_OP->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel MS_OP\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MS_RES
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertTlm_MS_RES_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MS_RES->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MS_RES\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MS_RES->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertTlm_MS_RES(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const F32& val
    )
    const
  {
    ASSERT_LT(__index, this->tlmHistory_MS_RES->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MS_RES\n"
      << "  Expected: Less than size of history ("
      << this->tlmHistory_MS_RES->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const TlmEntry_MS_RES& e =
      this->tlmHistory_MS_RES->at(__index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << __index
      << " on telmetry channel MS_RES\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertEvents_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all event histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MS_COMMAND_RECV
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertEvents_MS_COMMAND_RECV_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MS_COMMAND_RECV->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MS_COMMAND_RECV\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MS_COMMAND_RECV->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertEvents_MS_COMMAND_RECV(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const F32 val1,
        const F32 val2,
        MathSenderComponentBase::MathOpEv op
    ) const
  {
    ASSERT_GT(this->eventHistory_MS_COMMAND_RECV->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MS_COMMAND_RECV\n"
      << "  Expected: Less than size of history ("
      << this->eventHistory_MS_COMMAND_RECV->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_MS_COMMAND_RECV& e =
      this->eventHistory_MS_COMMAND_RECV->at(__index);
    ASSERT_EQ(val1, e.val1)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument val1 at index "
      << __index
      << " in history of event MS_COMMAND_RECV\n"
      << "  Expected: " << val1 << "\n"
      << "  Actual:   " << e.val1 << "\n";
    ASSERT_EQ(val2, e.val2)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument val2 at index "
      << __index
      << " in history of event MS_COMMAND_RECV\n"
      << "  Expected: " << val2 << "\n"
      << "  Actual:   " << e.val2 << "\n";
    ASSERT_EQ(op, e.op)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument op at index "
      << __index
      << " in history of event MS_COMMAND_RECV\n"
      << "  Expected: " << op << "\n"
      << "  Actual:   " << e.op << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MS_RESULT
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertEvents_MS_RESULT_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MS_RESULT->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MS_RESULT\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MS_RESULT->size() << "\n";
  }

  void MathSenderGTestBase ::
    assertEvents_MS_RESULT(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 __index,
        const F32 result
    ) const
  {
    ASSERT_GT(this->eventHistory_MS_RESULT->size(), __index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MS_RESULT\n"
      << "  Expected: Less than size of history ("
      << this->eventHistory_MS_RESULT->size() << ")\n"
      << "  Actual:   " << __index << "\n";
    const EventEntry_MS_RESULT& e =
      this->eventHistory_MS_RESULT->at(__index);
    ASSERT_EQ(result, e.result)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument result at index "
      << __index
      << " in history of event MS_RESULT\n"
      << "  Expected: " << result << "\n"
      << "  Actual:   " << e.result << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assertFromPortHistory_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistorySize)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Total size of all from port histories\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistorySize << "\n";
  }

  // ----------------------------------------------------------------------
  // From port: mathOut
  // ----------------------------------------------------------------------

  void MathSenderGTestBase ::
    assert_from_mathOut_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->fromPortHistory_mathOut->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for from_mathOut\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->fromPortHistory_mathOut->size() << "\n";
  }

} // end namespace Ref
