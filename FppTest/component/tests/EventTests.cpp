// ======================================================================
// \title  EventTests.cpp
// \author T. Chieu
// \brief  cpp file for event tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

// ----------------------------------------------------------------------
// Event tests
// ----------------------------------------------------------------------

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::NoParams& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    component.log_ACTIVITY_HI_EventNoArgs();

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EventNoArgs_SIZE(1);

    this->printTextLogHistory(stdout);
}

void Tester ::testEventHelper(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& data, NATIVE_UINT_TYPE size) {
    component.log_ACTIVITY_LO_EventPrimitive(data.args.val1, data.args.val2, data.args.val3, data.args.val4,
                                             data.args.val5, data.args.val6);

    ASSERT_EVENTS_SIZE(size);
    ASSERT_EVENTS_EventPrimitive_SIZE(size);
    ASSERT_EVENTS_EventPrimitive(portNum, data.args.val1, data.args.val2, data.args.val3, data.args.val4,
                                 data.args.val5, data.args.val6);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    for (U32 i = 0; i < component.EVENTID_EVENTPRIMITIVE_THROTTLE; i++) {
        testEventHelper(portNum, data, i + 1);
    }

    // Test that throttling works
    testEventHelper(portNum, data, component.EVENTID_EVENTPRIMITIVE_THROTTLE);

    // Test throttle reset
    component.log_ACTIVITY_LO_EventPrimitive_ThrottleClear();
    testEventHelper(portNum, data, component.EVENTID_EVENTPRIMITIVE_THROTTLE + 1);

    this->printTextLogHistory(stdout);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::LogStringParams& data) {
    component.log_COMMAND_EventString(data.args.val1, data.args.val2);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EventString_SIZE(1);
    ASSERT_EVENTS_EventString(portNum, data.args.val1.toChar(), data.args.val2.toChar());

    this->printTextLogHistory(stdout);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParam& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    component.log_DIAGNOSTIC_EventEnum(data.args.val);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EventEnum_SIZE(1);
    ASSERT_EVENTS_EventEnum(portNum, data.args.val);

    this->printTextLogHistory(stdout);
}

void Tester ::testEventHelper(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParam& data, NATIVE_UINT_TYPE size) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    component.log_FATAL_EventArray(data.args.val);

    ASSERT_EVENTS_SIZE(size);
    ASSERT_EVENTS_EventArray_SIZE(size);
    ASSERT_EVENTS_EventArray(portNum, data.args.val);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParam& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    for (U32 i = 0; i < component.EVENTID_EVENTARRAY_THROTTLE; i++) {
        testEventHelper(portNum, data, i + 1);
    }

    // Test that throttling works
    testEventHelper(portNum, data, component.EVENTID_EVENTARRAY_THROTTLE);

    // Test throttle reset
    component.log_FATAL_EventArray_ThrottleClear();
    testEventHelper(portNum, data, component.EVENTID_EVENTARRAY_THROTTLE + 1);

    this->printTextLogHistory(stdout);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::StructParam& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    component.log_WARNING_HI_EventStruct(data.args.val);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EventStruct_SIZE(1);
    ASSERT_EVENTS_EventStruct(portNum, data.args.val);

    this->printTextLogHistory(stdout);
}

void Tester ::testEventHelper(NATIVE_INT_TYPE portNum, FppTest::Types::BoolParam& data, NATIVE_UINT_TYPE size) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    component.log_WARNING_LO_EventBool(data.args.val);

    ASSERT_EVENTS_SIZE(size);
    ASSERT_EVENTS_EventBool_SIZE(size);
    ASSERT_EVENTS_EventBool(portNum, data.args.val);
}

void Tester ::testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::BoolParam& data) {
    ASSERT_TRUE(component.isConnected_eventOut_OutputPort(portNum));
    ASSERT_TRUE(component.isConnected_textEventOut_OutputPort(portNum));

    for (U32 i = 0; i < component.EVENTID_EVENTBOOL_THROTTLE; i++) {
        testEventHelper(portNum, data, i + 1);
    }

    // Test that throttling works
    testEventHelper(portNum, data, component.EVENTID_EVENTBOOL_THROTTLE);

    // Test throttle reset
    component.log_WARNING_LO_EventBool_ThrottleClear();
    testEventHelper(portNum, data, component.EVENTID_EVENTBOOL_THROTTLE + 1);

    this->printTextLogHistory(stdout);
}
