// ======================================================================
// \title  PortTest.hpp
// \author T. Chieu
// \brief  hpp file for PortTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_PORT_TEST_HPP
#define FPP_TEST_PORT_TEST_HPP

#include "Tester.hpp"
#include "FppTest/port/TypedPortIndexEnumAc.hpp"

#include "gtest/gtest.h"

// Typed port tests
template <typename PortType>
class TypedPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(TypedPortTest);

TYPED_TEST_P(TypedPortTest, TypedPort) {
    this->tester.invoke(TypedPortIndex::TYPED, this->port);
    this->tester.check_history(TypedPortIndex::TYPED, this->port);
}

REGISTER_TYPED_TEST_SUITE_P(TypedPortTest,
    TypedPort
);

// Serial port tests
template <typename PortType>
class SerialPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(SerialPortTest);

TYPED_TEST_P(SerialPortTest, SerialToTypedTest) {
//    this->tester.invoke(TypedPortIndex::SERIAL, this->port);
//    this->tester.check_history(TypedPortIndex::SERIAL, this->port);
}

TYPED_TEST_P(SerialPortTest, TypedToSerialTest) {
//    this->tester.invoke_serial_in(0, this->port);
//    this->tester.check_history(0, this->port);
}

REGISTER_TYPED_TEST_SUITE_P(SerialPortTest,
    SerialToTypedTest,
    TypedToSerialTest
);

#endif
