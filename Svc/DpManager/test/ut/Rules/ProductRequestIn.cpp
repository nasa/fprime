// ======================================================================
// \title  ProductRequestIn.cpp
// \author Rob Bocchino
// \brief  ProductRequestIn class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include <limits>

#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductRequestIn.hpp"
#include "Svc/DpManager/test/ut/Rules/Testers.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState::precondition__ProductRequestIn__BufferValid() const {
    return this->abstractState.bufferGetStatus == AbstractState::BufferGetStatus::VALID;
}

void TestState::action__ProductRequestIn__BufferValid() {
    // Clear history
    this->clearHistory();
    // Send the invocation
    const FwIndexType portNum = STest::Pick::startLength(0, DpManagerNumPorts);
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    const FwSizeType size = this->abstractState.getBufferSize();
    this->invoke_to_productRequestIn(portNum, id, size);
    this->component.doDispatch();
    // Check events
    ASSERT_EVENTS_SIZE(0);
    // Update test state
    ++this->abstractState.NumSuccessfulAllocations.value;
    // Check port history
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    // Check buffer get out
    ASSERT_from_bufferGetOut_SIZE(1);
    ASSERT_from_bufferGetOut(0, size);
    ASSERT_EQ(this->abstractState.bufferGetOutPortNumOpt.get(), portNum);
    // Check product response out
    ASSERT_from_productResponseOut_SIZE(1);
    const Fw::Success failure(Fw::Success::SUCCESS);
    const Fw::Buffer buffer(this->abstractState.bufferData, size);
    ASSERT_from_productResponseOut(0, id, buffer, failure);
    ASSERT_EQ(this->abstractState.productResponseOutPortNumOpt.get(), portNum);
}

bool TestState::precondition__ProductRequestIn__BufferInvalid() const {
    return this->abstractState.bufferGetStatus == AbstractState::BufferGetStatus::INVALID;
}

void TestState ::action__ProductRequestIn__BufferInvalid() {
    // Clear history
    this->clearHistory();
    // Send the invocation
    const FwIndexType portNum = STest::Pick::startLength(0, DpManagerNumPorts);
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    const FwSizeType size = this->abstractState.getBufferSize();
    this->invoke_to_productRequestIn(portNum, id, size);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.bufferAllocationFailedEventCount <
        DpManagerComponentBase::EVENTID_BUFFERALLOCATIONFAILED_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_BufferAllocationFailed(0, id);
        ++this->abstractState.bufferAllocationFailedEventCount;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Update test state
    ++this->abstractState.NumFailedAllocations.value;
    // Check port history
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    // Check buffer get out
    ASSERT_from_bufferGetOut_SIZE(1);
    ASSERT_from_bufferGetOut(0, size);
    ASSERT_EQ(this->abstractState.bufferGetOutPortNumOpt.get(), portNum);
    // Check product response out
    ASSERT_from_productResponseOut_SIZE(1);
    const Fw::Buffer buffer;
    const Fw::Success status(Fw::Success::FAILURE);
    ASSERT_from_productResponseOut(0, id, buffer, status);
    ASSERT_EQ(this->abstractState.productResponseOutPortNumOpt.get(), portNum);
}

namespace ProductRequestIn {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::BufferValid() {
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MIN_BUFFER_SIZE);
    this->ruleBufferValid.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MAX_BUFFER_SIZE);
    this->ruleBufferValid.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
}

void Tester ::BufferInvalid() {
    Testers::bufferGetStatus.ruleInvalid.apply(this->testState);
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MIN_BUFFER_SIZE);
    this->ruleBufferInvalid.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
    this->testState.abstractState.setBufferSize(Svc::AbstractState::MAX_BUFFER_SIZE);
    this->ruleBufferInvalid.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
}

}  // namespace ProductRequestIn

}  // namespace Svc
