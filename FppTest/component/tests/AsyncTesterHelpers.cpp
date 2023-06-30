// ======================================================================
// \title  AsyncTesterHelpers.cpp
// \author T. Chieu
// \brief  cpp file for async tester helper functions
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "test/ut/Tester.hpp"

void Tester ::connectAsyncPorts() {
    // arrayArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_arrayArgsAsync(i, this->component.get_arrayArgsAsync_InputPort(i));
    }

    // enumArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_enumArgsAsync(i, this->component.get_enumArgsAsync_InputPort(i));
    }

    // noArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_noArgsAsync(i, this->component.get_noArgsAsync_InputPort(i));
    }

    // primitiveArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_primitiveArgsAsync(i, this->component.get_primitiveArgsAsync_InputPort(i));
    }

    // stringArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_stringArgsAsync(i, this->component.get_stringArgsAsync_InputPort(i));
    }

    // structArgsAsync
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->connect_to_structArgsAsync(i, this->component.get_structArgsAsync_InputPort(i));
    }

    // serialAsync
    for (NATIVE_INT_TYPE i = 0; i < 3; ++i) {
        this->connect_to_serialAsync(i, this->component.get_serialAsync_InputPort(i));
    }

    // serialAsyncAssert
    this->connect_to_serialAsyncAssert(0, this->component.get_serialAsyncAssert_InputPort(0));

    // serialAsyncBlockPriority
    this->connect_to_serialAsyncBlockPriority(0, this->component.get_serialAsyncBlockPriority_InputPort(0));

    // serialAsyncDropPriority
    this->connect_to_serialAsyncDropPriority(0, this->component.get_serialAsyncDropPriority_InputPort(0));
}

Fw::QueuedComponentBase::MsgDispatchStatus Tester ::doDispatch() {
    return component.doDispatch();
}
