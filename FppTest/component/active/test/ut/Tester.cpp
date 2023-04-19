// ======================================================================
// \title  ActiveTest.hpp
// \author tiffany
// \brief  cpp file for ActiveTest test harness implementation class
// ======================================================================

#include "Tester.hpp"


  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      ActiveTestGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("ActiveTest")
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Telemetry tests
  // ----------------------------------------------------------------------

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::U32Param& data
  )
  {
    component.tlmWrite_ChannelU32(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelU32_SIZE(1);
    ASSERT_TLM_ChannelU32(portNum, data.args.val);
  }

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::F32Param& data
  )
  {
    component.tlmWrite_ChannelF32(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelF32_SIZE(1);
    ASSERT_TLM_ChannelF32(portNum, data.args.val);
  }

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::TlmStringParam& data
  )
  {
    component.tlmWrite_ChannelString(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelString_SIZE(1);
//    ASSERT_TLM_ChannelString(portNum, data.args.val);
  }

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::EnumParam& data
  )
  {
    component.tlmWrite_ChannelEnum(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelEnum_SIZE(1);
    ASSERT_TLM_ChannelEnum(portNum, data.args.val);
  }

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::ArrayParam& data
  )
  {
    component.tlmWrite_ChannelArray(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelArray_SIZE(1);
    ASSERT_TLM_ChannelArray(portNum, data.args.val);
  }

  void Tester ::
    testTelemetry(
        NATIVE_INT_TYPE portNum,
        FppTest::Types::StructParam& data
  )
  {
    component.tlmWrite_ChannelStruct(data.args.val);

    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_ChannelStruct_SIZE(1);
    ASSERT_TLM_ChannelStruct(portNum, data.args.val);
  }

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::
  from_arrayArgsOut_handler(
      const NATIVE_INT_TYPE portNum,
      const FormalParamArray &a,
      FormalParamArray &aRef
  )
{
  this->pushFromPortEntry_arrayArgsOut(a, aRef);
}

FormalParamArray Tester ::
  from_arrayReturnOut_handler(
      const NATIVE_INT_TYPE portNum,
      const FormalParamArray &a,
      FormalParamArray &aRef
  )
{
  this->pushFromPortEntry_arrayReturnOut(a, aRef);
  // TODO: Return a value
}

void Tester ::
  from_enumArgsOut_handler(
      const NATIVE_INT_TYPE portNum,
      const FormalParamEnum &en,
      FormalParamEnum &enRef
  )
{
  this->pushFromPortEntry_enumArgsOut(en, enRef);
}

  FormalParamEnum Tester ::
    from_enumReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->pushFromPortEntry_enumReturnOut(en, enRef);
    // TODO: Return a value
  }

  void Tester ::
    from_noArgsOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsOut();
  }

  bool Tester ::
    from_noArgsReturnOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsReturnOut();
    // TODO: Return a value
  }

  void Tester ::
    from_primitiveArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveArgsOut(u32, u32Ref, f32, f32Ref, b, bRef);
  }

  U32 Tester ::
    from_primitiveReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveReturnOut(u32, u32Ref, f32, f32Ref, b, bRef);
    // TODO: Return a value
  }

  void Tester ::
    from_stringArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->pushFromPortEntry_stringArgsOut(str80, str80Ref, str100, str100Ref);
  }

  void Tester ::
    from_structArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->pushFromPortEntry_structArgsOut(s, sRef);
  }

  FormalParamStruct Tester ::
    from_structReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->pushFromPortEntry_structReturnOut(s, sRef);
    // TODO: Return a value
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_serialOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }


