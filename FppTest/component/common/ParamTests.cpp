#include "test/ut/Tester.hpp"

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

  FW_ASSERT(id >= id_base);

  switch (id - id_base) {
    case TestComponentComponentBase::PARAMID_PARAMBOOL: 
      status = val.serialize(boolPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case TestComponentComponentBase::PARAMID_PARAMU32:
      status = val.serialize(u32Prm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case TestComponentComponentBase::PARAMID_PARAMSTRING:
      status = val.serialize(stringPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case TestComponentComponentBase::PARAMID_PARAMENUM:
      status = val.serialize(enumPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case TestComponentComponentBase::PARAMID_PARAMARRAY:
      status = val.serialize(arrayPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;

    case TestComponentComponentBase::PARAMID_PARAMSTRUCT:
      status = val.serialize(structPrm.args.val);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
      break;
  }

  this->pushFromPortEntry_prmGetIn(id, val);

  return prmValid;
}

void Tester ::
  testParam()
{
  component.loadParameters();

  Fw::ParamValid valid;

  bool boolVal = component.paramGet_ParamBool(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(boolVal, boolPrm.args.val);
  }

  U32 u32Val = component.paramGet_ParamU32(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(u32Val, u32Prm.args.val);
  }

  Fw::ParamString stringVal = component.paramGet_ParamString(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(stringVal, stringPrm.args.val);
  }

  FormalParamEnum enumVal = component.paramGet_ParamEnum(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(enumVal, enumPrm.args.val);
  }

  FormalParamArray arrayVal = component.paramGet_ParamArray(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(arrayVal, arrayPrm.args.val);
  }

  FormalParamStruct structVal = component.paramGet_ParamStruct(valid);
  ASSERT_EQ(valid, prmValid);
  if (valid == Fw::ParamValid::VALID) {
    ASSERT_EQ(structVal, structPrm.args.val);
  }

  ASSERT_from_prmGetIn_SIZE(6);
}
