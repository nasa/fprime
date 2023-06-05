#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Parameter tests
// ----------------------------------------------------------------------

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
