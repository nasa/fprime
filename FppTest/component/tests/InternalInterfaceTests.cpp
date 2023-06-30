// ======================================================================
// \title  InternalInterfaceTests.cpp
// \author T. Chieu
// \brief  cpp file for internal interface tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "test/ut/Tester.hpp"

#include "Fw/Comp/QueuedComponentBase.hpp"

void Tester ::testInternalInterface(FppTest::Types::NoParams& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalNoArgs_internalInterfaceInvoke();
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
}

void Tester ::testInternalInterface(FppTest::Types::PrimitiveParams& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalPrimitive_internalInterfaceInvoke(data.args.val1, data.args.val2, data.args.val3,
                                                              data.args.val4, data.args.val5, data.args.val6);
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_EQ(this->component.primitiveInterface.args.val1, data.args.val1);
    ASSERT_EQ(this->component.primitiveInterface.args.val2, data.args.val2);
    ASSERT_EQ(this->component.primitiveInterface.args.val3, data.args.val3);
    ASSERT_EQ(this->component.primitiveInterface.args.val4, data.args.val4);
    ASSERT_EQ(this->component.primitiveInterface.args.val5, data.args.val5);
    ASSERT_EQ(this->component.primitiveInterface.args.val6, data.args.val6);
}

void Tester ::testInternalInterface(FppTest::Types::InternalInterfaceStringParams& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalString_internalInterfaceInvoke(data.args.val1, data.args.val2);
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_EQ(this->component.stringInterface.args.val1, data.args.val1);
    ASSERT_EQ(this->component.stringInterface.args.val2, data.args.val2);
}

void Tester ::testInternalInterface(FppTest::Types::EnumParam& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalEnum_internalInterfaceInvoke(data.args.val);
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_EQ(this->component.enumInterface.args.val, data.args.val);
}

void Tester ::testInternalInterface(FppTest::Types::ArrayParam& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalArray_internalInterfaceInvoke(data.args.val);
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_EQ(this->component.arrayInterface.args.val, data.args.val);
}

void Tester ::testInternalInterface(FppTest::Types::StructParam& data) {
    Fw::QueuedComponentBase::MsgDispatchStatus status;

    this->component.internalStruct_internalInterfaceInvoke(data.args.val);
    status = this->doDispatch();

    ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_EQ(this->component.structInterface.args.val, data.args.val);
}
