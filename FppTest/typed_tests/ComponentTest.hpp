// ======================================================================
// \title  ComponentTest.hpp
// \author T. Chieu
// \brief  hpp file for component test classes
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_COMPONENT_TEST_HPP
#define FPP_TEST_COMPONENT_TEST_HPP

#include "test/ut/Tester.hpp"

#include "gtest/gtest.h"

template <typename FormalParamType>
class ComponentCommandTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentCommandTest);

TYPED_TEST_P(ComponentCommandTest, CommandTest) {
    this->tester.testCommand(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentCommandTest,
    CommandTest
);

template <typename FormalParamType>
class ComponentAsyncCommandTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentAsyncCommandTest);

TYPED_TEST_P(ComponentAsyncCommandTest, AsyncCommandTest) {
    this->tester.testAsyncCommand(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentAsyncCommandTest,
    AsyncCommandTest
);

template <typename FormalParamType>
class ComponentEventTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentEventTest);

TYPED_TEST_P(ComponentEventTest, EventTest) {
    this->tester.connectTimeGetOut();
    this->tester.testEvent(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentEventTest,
    EventTest
);

template <typename FormalParamType>
class ComponentTelemetryTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentTelemetryTest);

TYPED_TEST_P(ComponentTelemetryTest, TelemetryTest) {
    this->tester.connectTimeGetOut();
    this->tester.testTelemetry(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentTelemetryTest,
    TelemetryTest
);

template <typename FormalParamType>
class ComponentParamCommandTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentParamCommandTest);

TYPED_TEST_P(ComponentParamCommandTest, ParamTest) {
    this->tester.testParamCommand(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentParamCommandTest,
    ParamTest
);

template <typename FormalParamType>
class ComponentInternalInterfaceTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentInternalInterfaceTest);

TYPED_TEST_P(ComponentInternalInterfaceTest, InternalInterfaceTest) {
    this->tester.testInternalInterface(this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentInternalInterfaceTest,
    InternalInterfaceTest);

#endif
