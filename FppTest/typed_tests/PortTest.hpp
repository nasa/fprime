// ======================================================================
// \title  PortTest.hpp
// \author T. Chieu
// \brief  hpp file for PortTest class
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_PORT_TEST_HPP
#define FPP_TEST_PORT_TEST_HPP

#include "test/ut/Tester.hpp"
#include "FppTest/component/active/TypedPortIndexEnumAc.hpp"

#include "gtest/gtest.h"

// Typed port tests (sync and guarded)
template <typename PortType>
class TypedPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(TypedPortTest);

TYPED_TEST_P(TypedPortTest, SyncPort) {
    this->tester.testSyncPortInvoke(TypedPortIndex::TYPED, this->port);
    this->tester.testSyncPortCheck(this->port);
}

TYPED_TEST_P(TypedPortTest, GuardedPort) {
    this->tester.testGuardedPortInvoke(TypedPortIndex::TYPED, this->port);
    this->tester.testGuardedPortCheck(this->port);
}

REGISTER_TYPED_TEST_SUITE_P(TypedPortTest,
    SyncPort,
    GuardedPort
);

// Typed async port tests
template <typename PortType>
class TypedAsyncPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(TypedAsyncPortTest);

TYPED_TEST_P(TypedAsyncPortTest, AsyncPort) {
    this->tester.testAsyncPortInvoke(TypedPortIndex::TYPED, this->port);
    this->tester.doDispatch();
    this->tester.testAsyncPortCheck(this->port);
}

REGISTER_TYPED_TEST_SUITE_P(TypedAsyncPortTest,
    AsyncPort
);

// Serial port tests (sync and guarded)
template <typename PortType>
class SerialPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(SerialPortTest);

TYPED_TEST_P(SerialPortTest, ToSerialSync) {
    this->tester.testSyncPortInvoke(TypedPortIndex::SERIAL, this->port);
    this->tester.testSyncPortCheckSerial(this->port);
}

TYPED_TEST_P(SerialPortTest, FromSerialSync) {
    this->tester.testSyncPortInvokeSerial(TypedPortIndex::SERIAL, this->port);
    this->tester.testSyncPortCheck(this->port);
}

TYPED_TEST_P(SerialPortTest, ToSerialGuarded) {
    this->tester.testGuardedPortInvoke(TypedPortIndex::SERIAL, this->port);
    this->tester.testGuardedPortCheckSerial(this->port);
}

TYPED_TEST_P(SerialPortTest, FromSerialGuarded) {
    this->tester.testGuardedPortInvokeSerial(TypedPortIndex::SERIAL, this->port);
    this->tester.testGuardedPortCheck(this->port);
}

REGISTER_TYPED_TEST_SUITE_P(SerialPortTest,
    ToSerialSync,
    FromSerialSync,
    ToSerialGuarded,
    FromSerialGuarded
);

// Serial async port tests
template <typename PortType>
class SerialAsyncPortTest : public ::testing::Test {
protected:
    Tester tester;
    PortType port;
};

TYPED_TEST_SUITE_P(SerialAsyncPortTest);

TYPED_TEST_P(SerialAsyncPortTest, ToSerialAsync) {
    this->tester.testAsyncPortInvoke(TypedPortIndex::SERIAL, this->port);
    this->tester.doDispatch();
    this->tester.testAsyncPortCheckSerial(this->port);
}

TYPED_TEST_P(SerialAsyncPortTest, FromSerialAsync) {
    this->tester.testAsyncPortInvokeSerial(TypedPortIndex::SERIAL, this->port);
    this->tester.testAsyncPortCheck(this->port);
}

REGISTER_TYPED_TEST_SUITE_P(SerialAsyncPortTest,
    ToSerialAsync,
    FromSerialAsync
);

#endif
