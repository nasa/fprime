// ======================================================================
// \title  BufferGetStatus.cpp
// \author Rob Bocchino
// \brief  BufferGetStatus class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "Svc/DpManager/test/ut/Rules/BufferGetStatus.hpp"
#include "Svc/DpManager/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__BufferGetStatus__Valid() const {
    bool result = (this->abstractState.bufferGetStatus != AbstractState::BufferGetStatus::VALID);
    return result;
}

void TestState ::action__BufferGetStatus__Valid() {
    this->abstractState.bufferGetStatus = AbstractState::BufferGetStatus::VALID;
}

bool TestState ::precondition__BufferGetStatus__Invalid() const {
    bool result = (this->abstractState.bufferGetStatus != AbstractState::BufferGetStatus::INVALID);
    return result;
}

void TestState ::action__BufferGetStatus__Invalid() {
    this->abstractState.bufferGetStatus = AbstractState::BufferGetStatus::INVALID;
}

namespace BufferGetStatus {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::Valid() {
    this->ruleInvalid.apply(this->testState);
    this->ruleValid.apply(this->testState);
}

void Tester ::Invalid() {
    this->ruleInvalid.apply(this->testState);
}

}  // namespace BufferGetStatus

}  // namespace Svc
