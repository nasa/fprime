#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Event tests
// ----------------------------------------------------------------------

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::NoParams& data
)
{
  component.log_ACTIVITY_HI_EventNoArgs();

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_EventNoArgs_SIZE(1);
}

void Tester ::
  testEventHelper(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::PrimitiveParams& data,
      NATIVE_UINT_TYPE size
)
{
  component.log_ACTIVITY_LO_EventPrimitive(
    data.args.val1,
    data.args.val2,
    data.args.val3,
    data.args.val4,
    data.args.val5,
    data.args.val6
  );

  ASSERT_EVENTS_SIZE(size);
  ASSERT_EVENTS_EventPrimitive_SIZE(size);
  ASSERT_EVENTS_EventPrimitive(
    portNum,
    data.args.val1,
    data.args.val2,
    data.args.val3,
    data.args.val4,
    data.args.val5,
    data.args.val6
  );
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::PrimitiveParams& data
)
{
  for (U32 i = 0; i < component.EVENTID_EVENTPRIMITIVE_THROTTLE; i++) {
    testEventHelper(portNum, data, i + 1);
  }

  // Test that throttling works
  testEventHelper(portNum, data, component.EVENTID_EVENTPRIMITIVE_THROTTLE);

  // Test throttle reset
  component.log_ACTIVITY_LO_EventPrimitive_ThrottleClear();
  testEventHelper(portNum, data, component.EVENTID_EVENTPRIMITIVE_THROTTLE + 1);
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::LogStringParams& data
)
{
  component.log_COMMAND_EventString(
    data.args.val1,
    data.args.val2
  );

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_EventString_SIZE(1);
  ASSERT_EVENTS_EventString(
    portNum,
    data.args.val1.toChar(),
    data.args.val2.toChar()
  );
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::EnumParam& data
)
{
  component.log_DIAGNOSTIC_EventEnum(
    data.args.val
  );

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_EventEnum_SIZE(1);
  ASSERT_EVENTS_EventEnum(
    portNum,
    data.args.val
  );
}

void Tester ::
  testEventHelper(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::ArrayParam& data,
      NATIVE_UINT_TYPE size
)
{
  component.log_FATAL_EventArray(
    data.args.val
  );

  ASSERT_EVENTS_SIZE(size);
  ASSERT_EVENTS_EventArray_SIZE(size);
  ASSERT_EVENTS_EventArray(
    portNum,
    data.args.val
  );
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::ArrayParam& data
)
{
  for (U32 i = 0; i < component.EVENTID_EVENTARRAY_THROTTLE; i++) {
    testEventHelper(portNum, data, i + 1);
  }

  // Test that throttling works
  testEventHelper(portNum, data, component.EVENTID_EVENTARRAY_THROTTLE);

  // Test throttle reset
  component.log_FATAL_EventArray_ThrottleClear();
  testEventHelper(portNum, data, component.EVENTID_EVENTARRAY_THROTTLE + 1);
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::StructParam& data
)
{
  component.log_WARNING_HI_EventStruct(
    data.args.val
  );

  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_EventStruct_SIZE(1);
  ASSERT_EVENTS_EventStruct(
    portNum,
    data.args.val
  );
}

void Tester ::
  testEventHelper(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::BoolParam& data,
      NATIVE_UINT_TYPE size
)
{
  component.log_WARNING_LO_EventBool(
    data.args.val
  );

  ASSERT_EVENTS_SIZE(size);
  ASSERT_EVENTS_EventBool_SIZE(size);
  ASSERT_EVENTS_EventBool(
    portNum,
    data.args.val
  );
}

void Tester ::
  testEvent(
      NATIVE_INT_TYPE portNum,
      FppTest::Types::BoolParam& data
)
{
  for (U32 i = 0; i < component.EVENTID_EVENTBOOL_THROTTLE; i++) {
    testEventHelper(portNum, data, i + 1);
  }

  // Test that throttling works
  testEventHelper(portNum, data, component.EVENTID_EVENTBOOL_THROTTLE);

  // Test throttle reset
  component.log_WARNING_LO_EventBool_ThrottleClear();
  testEventHelper(portNum, data, component.EVENTID_EVENTBOOL_THROTTLE + 1);
}

// ----------------------------------------------------------------------
// Telemetry tests
// ----------------------------------------------------------------------

TLM_TEST_DEFS

// ----------------------------------------------------------------------
// Parameter tests
// ----------------------------------------------------------------------

Fw::ParamValid Tester ::
  from_prmGetIn_handler(
      const NATIVE_INT_TYPE portNum,
      FwPrmIdType id,
      Fw::ParamBuffer &val
  )
{
  val.resetSer();

  Fw::SerializeStatus status;
  U32 id_base = component.getIdBase();

  switch (id - id_base) {
    case component.PARAMID_PARAMBOOL: 
      status = val.serialize(boolPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case component.PARAMID_PARAMU32:
      status = val.serialize(u32Prm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case component.PARAMID_PARAMSTRING:
      status = val.serialize(stringPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case component.PARAMID_PARAMENUM:
      status = val.serialize(enumPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case component.PARAMID_PARAMARRAY:
      status = val.serialize(arrayPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case component.PARAMID_PARAMSTRUCT:
      status = val.serialize(structPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;
  }

  this->pushFromPortEntry_prmGetIn(id, val);

  return Fw::ParamValid::VALID;
}

void Tester ::
  testParam()
{
  component.loadParameters();

  Fw::ParamValid valid;

  bool boolVal = component.paramGet_ParamBool(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(boolVal, boolPrm.args.val);

  U32 u32Val = component.paramGet_ParamU32(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(u32Val, u32Prm.args.val);

  Fw::ParamString stringVal = component.paramGet_ParamString(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(stringVal, stringPrm.args.val);

  FormalParamEnum enumVal = component.paramGet_ParamEnum(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(enumVal, enumPrm.args.val);

  FormalParamArray arrayVal = component.paramGet_ParamArray(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(arrayVal, arrayPrm.args.val);

  FormalParamStruct structVal = component.paramGet_ParamStruct(valid);
  ASSERT_EQ(valid, Fw::ParamValid::VALID);
  ASSERT_EQ(structVal, structPrm.args.val);

  ASSERT_from_prmGetIn_SIZE(6);
}
