// ======================================================================
// \title  SchedIn.cpp
// \author Rob Bocchino
// \brief  SchedIn class implementation
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/DpWriter/test/ut/Rules/SchedIn.hpp"
#include "Svc/DpWriter/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__SchedIn__OK() const {
    return true;
}

void TestState ::action__SchedIn__OK() {
    // Clear history
    this->clearHistory();
    // Invoke schedIn port
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);
    this->component.doDispatch();
    // Check telemetry
    this->checkTelemetry();
}

namespace SchedIn {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::OK() {
    this->ruleOK.apply(this->testState);
}

}  // namespace SchedIn

}  // namespace Svc
