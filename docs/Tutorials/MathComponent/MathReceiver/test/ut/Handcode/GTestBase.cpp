// ======================================================================
// \title  MathReceiver/test/ut/GTestBase.cpp
// \author Auto-generated
// \brief  cpp file for MathReceiver component Google Test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GTestBase.hpp"

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  MathReceiverGTestBase ::
    MathReceiverGTestBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) : 
        MathReceiverTesterBase (
#if FW_OBJECT_NAMES == 1
            compName,
#endif
            maxHistorySize
        )
  {

  }

  MathReceiverGTestBase ::
    ~MathReceiverGTestBase(void)
  {

  }

  // ----------------------------------------------------------------------
  // Commands
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
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

  void MathReceiverGTestBase ::
    assertCmdResponse(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
    const
  {
    ASSERT_LT(index, this->cmdResponseHistory->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into command response history\n"
      << "  Expected: Less than size of command response history (" 
      << this->cmdResponseHistory->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const CmdResponse& e = this->cmdResponseHistory->at(index);
    ASSERT_EQ(opCode, e.opCode)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Opcode at index "
      << index
      << " in command response history\n"
      << "  Expected: " << opCode << "\n"
      << "  Actual:   " << e.opCode << "\n";
    ASSERT_EQ(cmdSeq, e.cmdSeq)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command sequence number at index "
      << index
      << " in command response history\n"
      << "  Expected: " << cmdSeq << "\n"
      << "  Actual:   " << e.cmdSeq << "\n";
    ASSERT_EQ(response, e.response)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Command response at index "
      << index
      << " in command response history\n"
      << "  Expected: " << response << "\n"
      << "  Actual:   " << e.response << "\n";
  }

  // ----------------------------------------------------------------------
  // Telemetry
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
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
  // Channel: MR_OPERATION
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertTlm_MR_OPERATION_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MR_OPERATION->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MR_OPERATION\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MR_OPERATION->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertTlm_MR_OPERATION(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const Ref::MathOp& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_MR_OPERATION->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MR_OPERATION\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_MR_OPERATION->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_MR_OPERATION& e =
      this->tlmHistory_MR_OPERATION->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel MR_OPERATION\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MR_FACTOR1S
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR1S_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MR_FACTOR1S->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MR_FACTOR1S\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MR_FACTOR1S->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR1S(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const U32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_MR_FACTOR1S->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MR_FACTOR1S\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_MR_FACTOR1S->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_MR_FACTOR1S& e =
      this->tlmHistory_MR_FACTOR1S->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel MR_FACTOR1S\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MR_FACTOR1
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR1_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MR_FACTOR1->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MR_FACTOR1\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MR_FACTOR1->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR1(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const F32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_MR_FACTOR1->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MR_FACTOR1\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_MR_FACTOR1->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_MR_FACTOR1& e =
      this->tlmHistory_MR_FACTOR1->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel MR_FACTOR1\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Channel: MR_FACTOR2
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR2_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(this->tlmHistory_MR_FACTOR2->size(), size)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for telemetry channel MR_FACTOR2\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->tlmHistory_MR_FACTOR2->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertTlm_MR_FACTOR2(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const F32& val
    )
    const
  {
    ASSERT_LT(index, this->tlmHistory_MR_FACTOR2->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of telemetry channel MR_FACTOR2\n"
      << "  Expected: Less than size of history (" 
      << this->tlmHistory_MR_FACTOR2->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const TlmEntry_MR_FACTOR2& e =
      this->tlmHistory_MR_FACTOR2->at(index);
    ASSERT_EQ(val, e.arg)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value at index "
      << index
      << " on telmetry channel MR_FACTOR2\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.arg << "\n";
  }

  // ----------------------------------------------------------------------
  // Events
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
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
  // Event: MR_SET_FACTOR1
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertEvents_MR_SET_FACTOR1_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MR_SET_FACTOR1->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MR_SET_FACTOR1\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MR_SET_FACTOR1->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertEvents_MR_SET_FACTOR1(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const F32 val
    ) const
  {
    ASSERT_GT(this->eventHistory_MR_SET_FACTOR1->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MR_SET_FACTOR1\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_MR_SET_FACTOR1->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_MR_SET_FACTOR1& e =
      this->eventHistory_MR_SET_FACTOR1->at(index);
    ASSERT_EQ(val, e.val)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument val at index "
      << index
      << " in history of event MR_SET_FACTOR1\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.val << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MR_UPDATED_FACTOR2
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertEvents_MR_UPDATED_FACTOR2_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MR_UPDATED_FACTOR2->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MR_UPDATED_FACTOR2\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MR_UPDATED_FACTOR2->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertEvents_MR_UPDATED_FACTOR2(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const F32 val
    ) const
  {
    ASSERT_GT(this->eventHistory_MR_UPDATED_FACTOR2->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MR_UPDATED_FACTOR2\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_MR_UPDATED_FACTOR2->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_MR_UPDATED_FACTOR2& e =
      this->eventHistory_MR_UPDATED_FACTOR2->at(index);
    ASSERT_EQ(val, e.val)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument val at index "
      << index
      << " in history of event MR_UPDATED_FACTOR2\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.val << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MR_OPERATION_PERFORMED
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertEvents_MR_OPERATION_PERFORMED_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventHistory_MR_OPERATION_PERFORMED->size())
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MR_OPERATION_PERFORMED\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventHistory_MR_OPERATION_PERFORMED->size() << "\n";
  }

  void MathReceiverGTestBase ::
    assertEvents_MR_OPERATION_PERFORMED(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 index,
        const Ref::MathOp val
    ) const
  {
    ASSERT_GT(this->eventHistory_MR_OPERATION_PERFORMED->size(), index)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Index into history of event MR_OPERATION_PERFORMED\n"
      << "  Expected: Less than size of history (" 
      << this->eventHistory_MR_OPERATION_PERFORMED->size() << ")\n"
      << "  Actual:   " << index << "\n";
    const EventEntry_MR_OPERATION_PERFORMED& e =
      this->eventHistory_MR_OPERATION_PERFORMED->at(index);
    ASSERT_EQ(val, e.val)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Value of argument val at index "
      << index
      << " in history of event MR_OPERATION_PERFORMED\n"
      << "  Expected: " << val << "\n"
      << "  Actual:   " << e.val << "\n";
  }

  // ----------------------------------------------------------------------
  // Event: MR_THROTTLE_CLEARED
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
    assertEvents_MR_THROTTLE_CLEARED_size(
        const char *const __callSiteFileName,
        const U32 __callSiteLineNumber,
        const U32 size
    ) const
  {
    ASSERT_EQ(size, this->eventsSize_MR_THROTTLE_CLEARED)
      << "\n"
      << "  File:     " << __callSiteFileName << "\n"
      << "  Line:     " << __callSiteLineNumber << "\n"
      << "  Value:    Size of history for event MR_THROTTLE_CLEARED\n"
      << "  Expected: " << size << "\n"
      << "  Actual:   " << this->eventsSize_MR_THROTTLE_CLEARED << "\n";
  }

  // ----------------------------------------------------------------------
  // From ports
  // ----------------------------------------------------------------------

  void MathReceiverGTestBase ::
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

  void MathReceiverGTestBase ::
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
