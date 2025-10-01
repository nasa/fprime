// ======================================================================
// \title  ExternalParamTests.cpp
// \author B. Campuzano
// \brief  cpp file for external parameter tests
//
// \copyright
// Copyright (C) 2009-2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

// ----------------------------------------------------------------------
// External Parameter tests
// ----------------------------------------------------------------------

void Tester ::testExternalParam() {
    ASSERT_TRUE(component.isConnected_prmGetOut_OutputPort(0));
    component.loadParameters();

    Fw::ParamValid valid;

    bool extBoolVal = component.paramGet_ParamBoolExternal(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(extBoolVal, this->paramTesterDelegate.m_param_ParamBoolExternal);
    }

    I32 i32Val = component.paramGet_ParamI32External(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(i32Val, this->paramTesterDelegate.m_param_ParamI32External);
    }

    Fw::ParamString stringVal = component.paramGet_ParamStringExternal(valid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(stringVal, this->paramTesterDelegate.m_param_ParamStringExternal);
    } else {
        ASSERT_EQ(valid, Fw::ParamValid::DEFAULT);
    }

    FormalParamEnum enumVal = component.paramGet_ParamEnumExternal(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(enumVal, this->paramTesterDelegate.m_param_ParamEnumExternal);
    }

    FormalParamArray arrayVal = component.paramGet_ParamArrayExternal(valid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(arrayVal, this->paramTesterDelegate.m_param_ParamArrayExternal);
    } else {
        ASSERT_EQ(valid, Fw::ParamValid::DEFAULT);
    }

    FormalParamStruct structVal = component.paramGet_ParamStructExternal(valid);
    ASSERT_EQ(valid, prmValid);
    if (valid == Fw::ParamValid::VALID) {
        ASSERT_EQ(structVal, this->paramTesterDelegate.m_param_ParamStructExternal);
    }
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::BoolParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMBOOLEXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMBOOLEXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMBOOLEXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMBOOLEXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamBoolExternal(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamBoolExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMBOOLEXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamBoolExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMBOOLEXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamBoolExternal, data.args.val);
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::I32Param& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMI32EXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMI32EXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMI32EXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMI32EXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamI32External(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamI32External(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMI32EXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamI32External(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMI32EXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamI32External, data.args.val);
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::PrmStringParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMSTRINGEXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMSTRINGEXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMSTRINGEXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMSTRINGEXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamStringExternal(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamStringExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMSTRINGEXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamStringExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMSTRINGEXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamStringExternal, data.args.val);
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::EnumParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMENUMEXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMENUMEXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMENUMEXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMENUMEXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamEnumExternal(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamEnumExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMENUMEXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamEnumExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMENUMEXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamEnumExternal, data.args.val);
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::ArrayParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMARRAYEXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMARRAYEXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMARRAYEXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMARRAYEXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamArrayExternal(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamArrayExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMARRAYEXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamArrayExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMARRAYEXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamArrayExternal, data.args.val);
}

void Tester ::testExternalParamCommand(FwIndexType portNum, FppTest::Types::StructParam& data) {
    Fw::CmdArgBuffer buf;

    // Test unsuccessful saving of param
    this->sendRawCmd(component.OPCODE_PARAMSTRUCTEXTERNAL_SAVE, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, component.OPCODE_PARAMSTRUCTEXTERNAL_SAVE, 1, Fw::CmdResponse::EXECUTION_ERROR);

    this->connectPrmSetIn();
    ASSERT_TRUE(component.isConnected_prmSetOut_OutputPort(portNum));

    // Test incorrect deserialization when setting param
    this->sendRawCmd(component.OPCODE_PARAMSTRUCTEXTERNAL_SET, 1, buf);

    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, component.OPCODE_PARAMSTRUCTEXTERNAL_SET, 1, Fw::CmdResponse::VALIDATION_ERROR);

    // Test successful setting of param
    this->paramSet_ParamStructExternal(data.args.val, Fw::ParamValid::VALID);
    this->paramSend_ParamStructExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, component.OPCODE_PARAMSTRUCTEXTERNAL_SET, 1, Fw::CmdResponse::OK);

    // Test successful saving of param
    this->paramSave_ParamStructExternal(0, 1);

    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, component.OPCODE_PARAMSTRUCTEXTERNAL_SAVE, 1, Fw::CmdResponse::OK);
    ASSERT_EQ(this->paramTesterDelegate.m_param_ParamStructExternal, data.args.val);
}
