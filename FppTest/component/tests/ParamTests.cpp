// ======================================================================
// \title  ParamTests.cpp
// \author T. Chieu
// \brief  cpp file for parameter tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Parameter tests
// ----------------------------------------------------------------------

void Tester ::testParam() {
    ASSERT_TRUE(component.isConnected_prmGetOut_OutputPort(0));
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
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(stringVal, stringPrm.args.val);
    } else {
        ASSERT_EQ(valid, Fw::ParamValid::DEFAULT);
    }

    FormalParamEnum enumVal = component.paramGet_ParamEnum(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(enumVal, enumPrm.args.val);
    }

    FormalParamArray arrayVal = component.paramGet_ParamArray(valid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(arrayVal, arrayPrm.args.val);
    } else {
        ASSERT_EQ(valid, Fw::ParamValid::DEFAULT);
    }

    FormalParamStruct structVal = component.paramGet_ParamStruct(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(structVal, structPrm.args.val);
    }
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::BoolParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMBOOL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMBOOL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMBOOL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMBOOL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamBool(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamBool(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMBOOL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamBool(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMBOOL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(boolPrm.args.val, data.args.val);
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::U32Param& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMU32_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMU32_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMU32_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMU32_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamU32(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamU32(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMU32_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamU32(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMU32_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(u32Prm.args.val, data.args.val);
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::PrmStringParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMSTRING_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMSTRING_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMSTRING_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMSTRING_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamString(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamString(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMSTRING_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamString(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMSTRING_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(stringPrm.args.val, data.args.val);
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMENUM_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMENUM_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMENUM_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMENUM_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamEnum(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamEnum(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMENUM_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamEnum(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMENUM_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(enumPrm.args.val, data.args.val);
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMARRAY_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMARRAY_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMARRAY_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMARRAY_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamArray(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamArray(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMARRAY_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamArray(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMARRAY_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(arrayPrm.args.val, data.args.val);
}

void Tester ::testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::StructParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMSTRUCT_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMSTRUCT_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMSTRUCT_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMSTRUCT_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamStruct(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamStruct(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMSTRUCT_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamStruct(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMSTRUCT_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(structPrm.args.val, data.args.val);
}
