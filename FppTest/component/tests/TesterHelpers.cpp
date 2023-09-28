// ======================================================================
// \title  TesterHelpers.cpp
// \author T. Chieu
// \brief  cpp file for tester helper functions
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // arrayArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_arrayArgsGuarded(i, this->component.get_arrayArgsGuarded_InputPort(i));
    }

    // arrayArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_arrayArgsSync(i, this->component.get_arrayArgsSync_InputPort(i));
    }

    // arrayReturnGuarded
    this->connect_to_arrayReturnGuarded(0, this->component.get_arrayReturnGuarded_InputPort(0));

    // arrayReturnSync
    this->connect_to_arrayReturnSync(0, this->component.get_arrayReturnSync_InputPort(0));

    // cmdIn
    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    // enumArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_enumArgsGuarded(i, this->component.get_enumArgsGuarded_InputPort(i));
    }

    // enumArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_enumArgsSync(i, this->component.get_enumArgsSync_InputPort(i));
    }

    // enumReturnGuarded
    this->connect_to_enumReturnGuarded(0, this->component.get_enumReturnGuarded_InputPort(0));

    // enumReturnSync
    this->connect_to_enumReturnSync(0, this->component.get_enumReturnSync_InputPort(0));

    // noArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_noArgsGuarded(i, this->component.get_noArgsGuarded_InputPort(i));
    }

    // noArgsReturnGuarded
    this->connect_to_noArgsReturnGuarded(0, this->component.get_noArgsReturnGuarded_InputPort(0));

    // noArgsReturnSync
    this->connect_to_noArgsReturnSync(0, this->component.get_noArgsReturnSync_InputPort(0));

    // noArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_noArgsSync(i, this->component.get_noArgsSync_InputPort(i));
    }

    // primitiveArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_primitiveArgsGuarded(i, this->component.get_primitiveArgsGuarded_InputPort(i));
    }

    // primitiveArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_primitiveArgsSync(i, this->component.get_primitiveArgsSync_InputPort(i));
    }

    // primitiveReturnGuarded
    this->connect_to_primitiveReturnGuarded(0, this->component.get_primitiveReturnGuarded_InputPort(0));

    // primitiveReturnSync
    this->connect_to_primitiveReturnSync(0, this->component.get_primitiveReturnSync_InputPort(0));

    // stringArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_stringArgsGuarded(i, this->component.get_stringArgsGuarded_InputPort(i));
    }

    // stringArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_stringArgsSync(i, this->component.get_stringArgsSync_InputPort(i));
    }

    // structArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_structArgsGuarded(i, this->component.get_structArgsGuarded_InputPort(i));
    }

    // structArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_structArgsSync(i, this->component.get_structArgsSync_InputPort(i));
    }

    // structReturnGuarded
    this->connect_to_structReturnGuarded(0, this->component.get_structReturnGuarded_InputPort(0));

    // structReturnSync
    this->connect_to_structReturnSync(0, this->component.get_structReturnSync_InputPort(0));

    // arrayArgsOut
    this->component.set_arrayArgsOut_OutputPort(TypedPortIndex::TYPED,
                                                this->get_from_arrayArgsOut(TypedPortIndex::TYPED));

    // arrayReturnOut
    this->component.set_arrayReturnOut_OutputPort(0, this->get_from_arrayReturnOut(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // enumArgsOut
    this->component.set_enumArgsOut_OutputPort(TypedPortIndex::TYPED,
                                               this->get_from_enumArgsOut(TypedPortIndex::TYPED));

    // enumReturnOut
    this->component.set_enumReturnOut_OutputPort(0, this->get_from_enumReturnOut(0));

    // eventOut
    this->component.set_eventOut_OutputPort(0, this->get_from_eventOut(0));

    // noArgsOut
    this->component.set_noArgsOut_OutputPort(TypedPortIndex::TYPED, this->get_from_noArgsOut(TypedPortIndex::TYPED));

    // noArgsReturnOut
    this->component.set_noArgsReturnOut_OutputPort(0, this->get_from_noArgsReturnOut(0));

    // primitiveArgsOut
    this->component.set_primitiveArgsOut_OutputPort(TypedPortIndex::TYPED,
                                                    this->get_from_primitiveArgsOut(TypedPortIndex::TYPED));

    // primitiveReturnOut
    this->component.set_primitiveReturnOut_OutputPort(0, this->get_from_primitiveReturnOut(0));

    // stringArgsOut
    this->component.set_stringArgsOut_OutputPort(TypedPortIndex::TYPED,
                                                 this->get_from_stringArgsOut(TypedPortIndex::TYPED));

    // structArgsOut
    this->component.set_structArgsOut_OutputPort(TypedPortIndex::TYPED,
                                                 this->get_from_structArgsOut(TypedPortIndex::TYPED));

    // structReturnOut
    this->component.set_structReturnOut_OutputPort(0, this->get_from_structReturnOut(0));

    // textEventOut
    this->component.set_textEventOut_OutputPort(0, this->get_from_textEventOut(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // ----------------------------------------------------------------------
    // Connect special ports
    // ----------------------------------------------------------------------

    // prmGetOut
    this->component.set_prmGetOut_OutputPort(0, this->get_from_prmGetIn(0));

    // ----------------------------------------------------------------------
    // Connect serial output ports
    // ----------------------------------------------------------------------

    this->component.set_noArgsOut_OutputPort(TypedPortIndex::SERIAL,
                                             this->get_from_serialOut(SerialPortIndex::NO_ARGS));

    this->component.set_primitiveArgsOut_OutputPort(TypedPortIndex::SERIAL,
                                                    this->get_from_serialOut(SerialPortIndex::PRIMITIVE));

    this->component.set_stringArgsOut_OutputPort(TypedPortIndex::SERIAL,
                                                 this->get_from_serialOut(SerialPortIndex::STRING));

    this->component.set_enumArgsOut_OutputPort(TypedPortIndex::SERIAL, this->get_from_serialOut(SerialPortIndex::ENUM));

    this->component.set_arrayArgsOut_OutputPort(TypedPortIndex::SERIAL,
                                                this->get_from_serialOut(SerialPortIndex::ARRAY));

    this->component.set_structArgsOut_OutputPort(TypedPortIndex::SERIAL,
                                                 this->get_from_serialOut(SerialPortIndex::STRUCT));

    this->component.set_serialOut_OutputPort(SerialPortIndex::NO_ARGS,
                                             this->get_from_noArgsOut(TypedPortIndex::SERIAL));

    this->component.set_serialOut_OutputPort(SerialPortIndex::PRIMITIVE,
                                             this->get_from_primitiveArgsOut(TypedPortIndex::SERIAL));

    this->component.set_serialOut_OutputPort(SerialPortIndex::STRING,
                                             this->get_from_stringArgsOut(TypedPortIndex::SERIAL));

    this->component.set_serialOut_OutputPort(SerialPortIndex::ENUM, this->get_from_enumArgsOut(TypedPortIndex::SERIAL));

    this->component.set_serialOut_OutputPort(SerialPortIndex::ARRAY,
                                             this->get_from_arrayArgsOut(TypedPortIndex::SERIAL));

    this->component.set_serialOut_OutputPort(SerialPortIndex::STRUCT,
                                             this->get_from_structArgsOut(TypedPortIndex::SERIAL));

    // ----------------------------------------------------------------------
    // Connect serial input ports
    // ----------------------------------------------------------------------

    // serialGuarded
    for (NATIVE_INT_TYPE i = 0; i < 6; ++i) {
        this->connect_to_serialGuarded(i, this->component.get_serialGuarded_InputPort(i));
    }

    // serialSync
    for (NATIVE_INT_TYPE i = 0; i < 6; ++i) {
        this->connect_to_serialSync(i, this->component.get_serialSync_InputPort(i));
    }
}

void Tester ::connectPrmSetIn() {
    // prmSetOut
    this->component.set_prmSetOut_OutputPort(0, this->get_from_prmSetIn(0));
}

void Tester ::connectTimeGetOut() {
    // timeGetOut
    this->component.set_timeGetOut_OutputPort(0, this->get_from_timeGetOut(0));
}

void Tester ::connectSpecialPortsSerial() {
    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_serialOut(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_serialOut(0));

    // eventOut
    this->component.set_eventOut_OutputPort(0, this->get_from_serialOut(0));

    // textEventOut
    this->component.set_textEventOut_OutputPort(0, this->get_from_serialOut(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_serialOut(0));

    // prmSetOut
    this->component.set_prmSetOut_OutputPort(0, this->get_from_serialOut(0));

    // timeGetOut
    this->component.set_timeGetOut_OutputPort(0, this->get_from_serialOut(0));
}

void Tester ::setPrmValid(Fw::ParamValid valid) {
    this->prmValid = valid;
}

void Tester ::checkSerializeStatusSuccess() {
    ASSERT_EQ(component.serializeStatus, Fw::FW_SERIALIZE_OK);
}

void Tester ::checkSerializeStatusBufferEmpty() {
    ASSERT_EQ(component.serializeStatus, Fw::FW_DESERIALIZE_BUFFER_EMPTY);
}
