// ======================================================================
// \title  ProductSendIn.cpp
// \author Rob Bocchino
// \brief  ProductSendIn class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include "Svc/DpManager/test/ut/Rules/ProductSendIn.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/Testers.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__ProductSendIn__OK() const {
    return true;
}

void TestState ::action__ProductSendIn__OK() {
    // Clear history
    this->clearHistory();
    // Send the invocation
    const auto portNum = static_cast<FwIndexType>(STest::Pick::startLength(0, DpManagerNumPorts));
    const auto id =
        static_cast<FwDpIdType>(STest::Pick::lowerUpper(0, static_cast<U32>(std::numeric_limits<FwDpIdType>::max())));
    const FwSizeType size = this->abstractState.getBufferSize();
    const Fw::Buffer buffer(this->abstractState.bufferData, static_cast<Fw::Buffer::SizeType>(size));
    this->invoke_to_productSendIn(portNum, id, buffer);
    this->doDispatch();
    // Check events
    ASSERT_EVENTS_SIZE(0);
    // Update test state
    ++this->abstractState.NumDataProducts.value;
    this->abstractState.NumBytes.value += size;
    // Check port history
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // Check product send out
    ASSERT_from_productSendOut_SIZE(1);
    ASSERT_from_productSendOut(0, buffer);
    ASSERT_EQ(this->abstractState.productSendOutPortNumOpt.value(), portNum);
}

// ----------------------------------------------------------------------
// Non-rule tests
// ----------------------------------------------------------------------

void TestState ::testProductSendInOverflowHook() {
    this->clearHistory();
    this->abstractState.setBufferSize(Svc::AbstractState::MAX_BUFFER_SIZE);
    const FwSizeType size = this->abstractState.getBufferSize();
    const Fw::Buffer buffer(this->abstractState.bufferData, static_cast<Fw::Buffer::SizeType>(size));
    // Fill the message queue without dispatching so the next invoke overflows
    for (FwSizeType i = 0; i < DpManagerTester::TEST_INSTANCE_QUEUE_DEPTH; i++) {
        this->invoke_to_productSendIn(0, 0, buffer);
    }
    // Queue full: this invoke triggers the overflow hook, which must not assert
    const FwDpIdType overflowId = 42;
    this->invoke_to_productSendIn(0, overflowId, buffer);
    // The DP is dropped, a throttled event records it, and its buffer is returned to the pool
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferDropped_SIZE(1);
    ASSERT_EVENTS_BufferDropped(0, overflowId, size);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer);
    ASSERT_from_productSendOut_SIZE(0);
}

namespace ProductSendIn {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::OK() {
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MIN_BUFFER_SIZE);
    this->ruleOK.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MAX_BUFFER_SIZE);
    this->ruleOK.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
}

void Tester ::OverflowHook() {
    this->testState.testProductSendInOverflowHook();
}

}  // namespace ProductSendIn

}  // namespace Svc
