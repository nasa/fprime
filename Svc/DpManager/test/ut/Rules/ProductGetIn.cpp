// ======================================================================
// \title  ProductGetIn.cpp
// \author Rob Bocchino
// \brief  ProductGetIn class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include <limits>

#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/ProductGetIn.hpp"
#include "Svc/DpManager/test/ut/Rules/Testers.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState::precondition__ProductGetIn__BufferValid() const {
    return this->abstractState.bufferGetStatus == AbstractState::BufferGetStatus::VALID;
}

void TestState::action__ProductGetIn__BufferValid() {
    // Clear history
    this->clearHistory();
    // Send the invocation
    const FwIndexType portNum = STest::Pick::startLength(0, DpManagerNumPorts);
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    const FwSizeType size = this->abstractState.getBufferSize();
    Fw::Buffer buffer;
    const auto status = this->invoke_to_productGetIn(portNum, id, size, buffer);
    ASSERT_EQ(status, Fw::Success::SUCCESS);
    // Check events
    ASSERT_EVENTS_SIZE(0);
    // Update test state
    ++this->abstractState.NumSuccessfulAllocations.value;
    // Check port history
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // Check buffer get out
    ASSERT_from_bufferGetOut_SIZE(1);
    ASSERT_from_bufferGetOut(0, size);
    ASSERT_EQ(this->abstractState.bufferGetOutPortNumOpt.get(), portNum);
    // Check the buffer
    const Fw::Buffer expectedBuffer(this->abstractState.bufferData, size);
    ASSERT_EQ(buffer, expectedBuffer);
}

bool TestState::precondition__ProductGetIn__BufferInvalid() const {
    return this->abstractState.bufferGetStatus == AbstractState::BufferGetStatus::INVALID;
}

void TestState ::action__ProductGetIn__BufferInvalid() {
    // Clear history
    this->clearHistory();
    // Send the invocation
    const FwIndexType portNum = STest::Pick::startLength(0, DpManagerNumPorts);
    const FwDpIdType id = STest::Pick::lowerUpper(0, std::numeric_limits<FwDpIdType>::max());
    const FwSizeType size = this->abstractState.getBufferSize();
    Fw::Buffer buffer;
    const auto status = this->invoke_to_productGetIn(portNum, id, size, buffer);
    ASSERT_EQ(status, Fw::Success::FAILURE);
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
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // Check buffer get out
    ASSERT_from_bufferGetOut_SIZE(1);
    ASSERT_from_bufferGetOut(0, size);
    ASSERT_EQ(this->abstractState.bufferGetOutPortNumOpt.get(), portNum);
}

namespace ProductGetIn {

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

}  // namespace ProductGetIn

}  // namespace Svc
