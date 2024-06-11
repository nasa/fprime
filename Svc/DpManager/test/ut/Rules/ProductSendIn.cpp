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

#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductSendIn.hpp"
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
    const FwIndexType portNum = STest::Pick::startLength(0, DpManagerNumPorts);
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    const FwSizeType size = this->abstractState.getBufferSize();
    const Fw::Buffer buffer(this->abstractState.bufferData, size);
    this->invoke_to_productSendIn(portNum, id, buffer);
    this->component.doDispatch();
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
    ASSERT_EQ(this->abstractState.productSendOutPortNumOpt.get(), portNum);
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

}  // namespace ProductSendIn

}  // namespace Svc
