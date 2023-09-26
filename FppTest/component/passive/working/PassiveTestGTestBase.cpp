// ======================================================================
// \title  PassiveTestGTestBase.cpp
// \author Generated by fpp-to-cpp
// \brief  cpp file for PassiveTest component Google Test harness base class
// ======================================================================

#include "FppTest/component/passive/PassiveTestGTestBase.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

PassiveTestGTestBase ::
  PassiveTestGTestBase(
      const char* const compName,
      const U32 maxHistorySize
  ) :
    PassiveTestTesterBase(compName, maxHistorySize)
{

}

PassiveTestGTestBase ::
  ~PassiveTestGTestBase()
{

}

// ----------------------------------------------------------------------
// From ports
// ----------------------------------------------------------------------

void PassiveTestGTestBase ::
  assertFromPortHistory_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistorySize)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Total size of all from port histories\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistorySize << "\n";
}

void PassiveTestGTestBase ::
  assert_from_arrayArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_arrayArgsOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for arrayArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_arrayArgsOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_arrayReturnOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_arrayReturnOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for arrayReturnOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_arrayReturnOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_enumArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_enumArgsOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for enumArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_enumArgsOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_enumReturnOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_enumReturnOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for enumReturnOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_enumReturnOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_noArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistorySize_noArgsOut)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for noArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistorySize_noArgsOut << "\n";
}

void PassiveTestGTestBase ::
  assert_from_noArgsReturnOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistorySize_noArgsReturnOut)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for noArgsReturnOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistorySize_noArgsReturnOut << "\n";
}

void PassiveTestGTestBase ::
  assert_from_primitiveArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_primitiveArgsOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for primitiveArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_primitiveArgsOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_primitiveReturnOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_primitiveReturnOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for primitiveReturnOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_primitiveReturnOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_prmGetIn_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_prmGetIn->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for prmGetIn\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_prmGetIn->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_prmSetIn_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_prmSetIn->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for prmSetIn\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_prmSetIn->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_stringArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_stringArgsOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for stringArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_stringArgsOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_structArgsOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_structArgsOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for structArgsOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_structArgsOut->size() << "\n";
}

void PassiveTestGTestBase ::
  assert_from_structReturnOut_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->fromPortHistory_structReturnOut->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for structReturnOut\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->fromPortHistory_structReturnOut->size() << "\n";
}

// ----------------------------------------------------------------------
// Commands
// ----------------------------------------------------------------------

void PassiveTestGTestBase ::
  assertCmdResponse_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->cmdResponseHistory->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of command response history\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->cmdResponseHistory->size() << "\n";
}

void PassiveTestGTestBase ::
  assertCmdResponse(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      FwOpcodeType opCode,
      U32 cmdSeq,
      Fw::CmdResponse response
  ) const
{
  ASSERT_LT(__index, this->cmdResponseHistory->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into command response history\n"
    << "  Expected: Less than size of command response history ("
    << this->cmdResponseHistory->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const CmdResponse& e = this->cmdResponseHistory->at(__index);
  ASSERT_EQ(opCode, e.opCode)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Opcode at index "
    << __index
    << " in command response history\n"
    << "  Expected: " << opCode << "\n"
    << "  Actual:   " << e.opCode << "\n";
  ASSERT_EQ(cmdSeq, e.cmdSeq)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Command sequence number at index "
    << __index
    << " in command response history\n"
    << "  Expected: " << cmdSeq << "\n"
    << "  Actual:   " << e.cmdSeq << "\n";
  ASSERT_EQ(response, e.response)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Command response at index "
    << __index
    << " in command response history\n"
    << "  Expected: " << response << "\n"
    << "  Actual:   " << e.response << "\n";
}

// ----------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------

void PassiveTestGTestBase ::
  assertEvents_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventsSize)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Total size of all event histories\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventsSize << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventNoArgs_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventsSize_EventNoArgs)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventNoArgs\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventsSize_EventNoArgs << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventPrimitive_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventPrimitive->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventPrimitive\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventPrimitive->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventPrimitive(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const U32 u32_1,
      const U32 u32_2,
      const F32 f32_1,
      const F32 f32_2,
      const bool b1,
      const bool b2
  ) const
{
  ASSERT_GT(this->eventHistory_EventPrimitive->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventPrimitive\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventPrimitive->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventPrimitive& _e =
    this->eventHistory_EventPrimitive->at(__index);
  ASSERT_EQ(u32_1, _e.u32_1)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument u32_1 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << u32_1 << "\n"
    << "  Actual:   " << _e.u32_1 << "\n";
  ASSERT_EQ(u32_2, _e.u32_2)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument u32_2 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << u32_2 << "\n"
    << "  Actual:   " << _e.u32_2 << "\n";
  ASSERT_EQ(f32_1, _e.f32_1)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument f32_1 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << f32_1 << "\n"
    << "  Actual:   " << _e.f32_1 << "\n";
  ASSERT_EQ(f32_2, _e.f32_2)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument f32_2 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << f32_2 << "\n"
    << "  Actual:   " << _e.f32_2 << "\n";
  ASSERT_EQ(b1, _e.b1)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument b1 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << b1 << "\n"
    << "  Actual:   " << _e.b1 << "\n";
  ASSERT_EQ(b2, _e.b2)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument b2 at index "
    << __index
    << " in history of event EventPrimitive\n"
    << "  Expected: " << b2 << "\n"
    << "  Actual:   " << _e.b2 << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventString_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventString->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventString\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventString->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventString(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const char* const str1,
      const char* const str2
  ) const
{
  ASSERT_GT(this->eventHistory_EventString->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventString\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventString->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventString& _e =
    this->eventHistory_EventString->at(__index);
  ASSERT_STREQ(str1, _e.str1.toChar())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument str1 at index "
    << __index
    << " in history of event EventString\n"
    << "  Expected: " << str1 << "\n"
    << "  Actual:   " << _e.str1.toChar() << "\n";
  ASSERT_STREQ(str2, _e.str2.toChar())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument str2 at index "
    << __index
    << " in history of event EventString\n"
    << "  Expected: " << str2 << "\n"
    << "  Actual:   " << _e.str2.toChar() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventEnum_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventEnum->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventEnum\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventEnum->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventEnum(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamEnum& en
  ) const
{
  ASSERT_GT(this->eventHistory_EventEnum->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventEnum\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventEnum->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventEnum& _e =
    this->eventHistory_EventEnum->at(__index);
  ASSERT_EQ(en, _e.en)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument en at index "
    << __index
    << " in history of event EventEnum\n"
    << "  Expected: " << en << "\n"
    << "  Actual:   " << _e.en << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventArray_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventArray->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventArray\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventArray->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventArray(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamArray& arr
  ) const
{
  ASSERT_GT(this->eventHistory_EventArray->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventArray\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventArray->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventArray& _e =
    this->eventHistory_EventArray->at(__index);
  ASSERT_EQ(arr, _e.arr)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument arr at index "
    << __index
    << " in history of event EventArray\n"
    << "  Expected: " << arr << "\n"
    << "  Actual:   " << _e.arr << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventStruct_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventStruct->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventStruct\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventStruct->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventStruct(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamStruct& str
  ) const
{
  ASSERT_GT(this->eventHistory_EventStruct->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventStruct\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventStruct->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventStruct& _e =
    this->eventHistory_EventStruct->at(__index);
  ASSERT_EQ(str, _e.str)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument str at index "
    << __index
    << " in history of event EventStruct\n"
    << "  Expected: " << str << "\n"
    << "  Actual:   " << _e.str << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventBool_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->eventHistory_EventBool->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for event EventBool\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->eventHistory_EventBool->size() << "\n";
}

void PassiveTestGTestBase ::
  assertEvents_EventBool(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const bool b
  ) const
{
  ASSERT_GT(this->eventHistory_EventBool->size(), __index)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of event EventBool\n"
    << "  Expected: Less than size of history ("
    << this->eventHistory_EventBool->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const EventEntry_EventBool& _e =
    this->eventHistory_EventBool->at(__index);
  ASSERT_EQ(b, _e.b)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value of argument b at index "
    << __index
    << " in history of event EventBool\n"
    << "  Expected: " << b << "\n"
    << "  Actual:   " << _e.b << "\n";
}

// ----------------------------------------------------------------------
// Telemetry
// ----------------------------------------------------------------------

void PassiveTestGTestBase ::
  assertTlm_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(size, this->tlmSize)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Total size of all telemetry histories\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmSize << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelEnum_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelEnum->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelEnum\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelEnum->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelEnum(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamEnum& val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelEnum->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelEnum\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelEnum->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelEnum& _e =
    this->tlmHistory_ChannelEnum->at(__index);
  ASSERT_EQ(val, _e.arg)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelEnum\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelArray_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelArray->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelArray\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelArray->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelArray(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamArray& val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelArray->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelArray\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelArray->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelArray& _e =
    this->tlmHistory_ChannelArray->at(__index);
  ASSERT_EQ(val, _e.arg)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelArray\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelStruct_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelStruct->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelStruct\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelStruct->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelStruct(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const FormalParamStruct& val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelStruct->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelStruct\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelStruct->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelStruct& _e =
    this->tlmHistory_ChannelStruct->at(__index);
  ASSERT_EQ(val, _e.arg)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelStruct\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelU32_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelU32->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelU32\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelU32->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelU32(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const U32 val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelU32->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelU32\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelU32->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelU32& _e =
    this->tlmHistory_ChannelU32->at(__index);
  ASSERT_EQ(val, _e.arg)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelU32\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelF32_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelF32->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelF32\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelF32->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelF32(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const F32 val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelF32->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelF32\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelF32->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelF32& _e =
    this->tlmHistory_ChannelF32->at(__index);
  ASSERT_EQ(val, _e.arg)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelF32\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelString_size(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 size
  ) const
{
  ASSERT_EQ(this->tlmHistory_ChannelString->size(), size)
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Size of history for telemetry channel ChannelString\n"
    << "  Expected: " << size << "\n"
    << "  Actual:   " << this->tlmHistory_ChannelString->size() << "\n";
}

void PassiveTestGTestBase ::
  assertTlm_ChannelString(
      const char* const __callSiteFileName,
      const U32 __callSiteLineNumber,
      const U32 __index,
      const char* const val
  ) const
{
  ASSERT_LT(__index, this->tlmHistory_ChannelString->size())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Index into history of telemetry channel ChannelString\n"
    << "  Expected: Less than size of history ("
    << this->tlmHistory_ChannelString->size() << ")\n"
    << "  Actual:   " << __index << "\n";
  const TlmEntry_ChannelString& _e =
    this->tlmHistory_ChannelString->at(__index);
  ASSERT_STREQ(val, _e.arg.toChar())
    << "\n"
    << __callSiteFileName << ":" << __callSiteLineNumber << "\n"
    << "  Value:    Value at index "
    << __index
    << " on telemetry channel ChannelString\n"
    << "  Expected: " << val << "\n"
    << "  Actual:   " << _e.arg << "\n";
}