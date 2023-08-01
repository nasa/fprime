// ======================================================================
// \title  QueuedTest/test/ut/TesterHelpers.cpp
// \author Auto-generated
// \brief  cpp file for QueuedTest component test harness base class
//
// NOTE: this file was automatically generated
//
// ======================================================================
#include "Tester.hpp"

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // arrayArgsAsyncBlockPriority
    this->connect_to_arrayArgsAsyncBlockPriority(0, this->component.get_arrayArgsAsyncBlockPriority_InputPort(0));

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

    // enumArgsAsyncAssert
    this->connect_to_enumArgsAsyncAssert(0, this->component.get_enumArgsAsyncAssert_InputPort(0));

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

    // noArgsAsync
    this->connect_to_noArgsAsync(0, this->component.get_noArgsAsync_InputPort(0));

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

    // primitiveArgsAsync
    this->connect_to_primitiveArgsAsync(0, this->component.get_primitiveArgsAsync_InputPort(0));

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

    // stringArgsAsync
    this->connect_to_stringArgsAsync(0, this->component.get_stringArgsAsync_InputPort(0));

    // stringArgsGuarded
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_stringArgsGuarded(i, this->component.get_stringArgsGuarded_InputPort(i));
    }

    // stringArgsSync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_stringArgsSync(i, this->component.get_stringArgsSync_InputPort(i));
    }

    // structArgsAsyncDropPriority
    this->connect_to_structArgsAsyncDropPriority(0, this->component.get_structArgsAsyncDropPriority_InputPort(0));

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
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_arrayArgsOut_OutputPort(i, this->get_from_arrayArgsOut(i));
    }

    // arrayReturnOut
    this->component.set_arrayReturnOut_OutputPort(0, this->get_from_arrayReturnOut(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // enumArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_enumArgsOut_OutputPort(i, this->get_from_enumArgsOut(i));
    }

    // enumReturnOut
    this->component.set_enumReturnOut_OutputPort(0, this->get_from_enumReturnOut(0));

    // eventOut
    this->component.set_eventOut_OutputPort(0, this->get_from_eventOut(0));

    // noArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_noArgsOut_OutputPort(i, this->get_from_noArgsOut(i));
    }

    // noArgsReturnOut
    this->component.set_noArgsReturnOut_OutputPort(0, this->get_from_noArgsReturnOut(0));

    // primitiveArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_primitiveArgsOut_OutputPort(i, this->get_from_primitiveArgsOut(i));
    }

    // primitiveReturnOut
    this->component.set_primitiveReturnOut_OutputPort(0, this->get_from_primitiveReturnOut(0));

    // prmGetOut
    this->component.set_prmGetOut_OutputPort(0, this->get_from_prmGetOut(0));

    // prmSetOut
    this->component.set_prmSetOut_OutputPort(0, this->get_from_prmSetOut(0));

    // stringArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_stringArgsOut_OutputPort(i, this->get_from_stringArgsOut(i));
    }

    // structArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_structArgsOut_OutputPort(i, this->get_from_structArgsOut(i));
    }

    // structReturnOut
    this->component.set_structReturnOut_OutputPort(0, this->get_from_structReturnOut(0));

    // textEventOut
    this->component.set_textEventOut_OutputPort(0, this->get_from_textEventOut(0));

    // timeGetOut
    this->component.set_timeGetOut_OutputPort(0, this->get_from_timeGetOut(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // ----------------------------------------------------------------------
    // Connect serial output ports
    // ----------------------------------------------------------------------
    for (NATIVE_INT_TYPE i = 0; i < 5; ++i) {
        this->component.set_serialOut_OutputPort(i, this->get_from_serialOut(i));
    }

    // ----------------------------------------------------------------------
    // Connect serial input ports
    // ----------------------------------------------------------------------
    // serialAsync
    this->connect_to_serialAsync(0, this->component.get_serialAsync_InputPort(0));

    // serialAsyncAssert
    this->connect_to_serialAsyncAssert(0, this->component.get_serialAsyncAssert_InputPort(0));

    // serialAsyncBlockPriority
    this->connect_to_serialAsyncBlockPriority(0, this->component.get_serialAsyncBlockPriority_InputPort(0));

    // serialAsyncDropPriority
    this->connect_to_serialAsyncDropPriority(0, this->component.get_serialAsyncDropPriority_InputPort(0));

    // serialGuarded
    this->connect_to_serialGuarded(0, this->component.get_serialGuarded_InputPort(0));

    // serialSync
    this->connect_to_serialSync(0, this->component.get_serialSync_InputPort(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(Tester::TEST_INSTANCE_QUEUE_DEPTH, Tester::TEST_INSTANCE_ID);
}
