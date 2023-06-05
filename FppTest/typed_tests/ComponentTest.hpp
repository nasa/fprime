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
class ComponentSpecialPortTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentSpecialPortTest);

TYPED_TEST_P(ComponentSpecialPortTest, SpecialPortTest) {
//    this->tester.testSpecialPort(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentSpecialPortTest,
    SpecialPortTest
);

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
class ComponentEventTest : public ::testing::Test {
protected:
    Tester tester;
    FormalParamType data;
};

TYPED_TEST_SUITE_P(ComponentEventTest);

TYPED_TEST_P(ComponentEventTest, EventTest) {
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
    this->tester.testTelemetry(0, this->data);
}

REGISTER_TYPED_TEST_SUITE_P(ComponentTelemetryTest,
    TelemetryTest
);

#endif
