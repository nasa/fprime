// ======================================================================
// \title  Random.hpp
// \author Rob Bocchino
// \brief  Random scenario
//
// \copyright
// Copyright (C) 2021 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/Scenarios/Random.hpp"

namespace Svc {

namespace Scenarios {

namespace Random {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

Rules::BufferGetStatus::Invalid bufferGetStatusInvalid;
Rules::BufferGetStatus::Valid bufferGetStatusValid;
Rules::CLEAR_EVENT_THROTTLE::OK clearEventThrottleOK;
Rules::ProductRequestIn::BufferInvalid productRequestInBufferInvalid;
Rules::ProductRequestIn::BufferValid productRequestInBufferValid;
Rules::ProductSendIn::OK productSendInOK;
Rules::SchedIn::OK schedInOK;

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::run(FwSizeType maxNumSteps) {
    STest::Rule<TestState>* rules[] = {
        &bufferGetStatusInvalid,
        &bufferGetStatusValid,
        &clearEventThrottleOK,
        &productRequestInBufferInvalid,
        &productRequestInBufferValid,
        &productSendInOK,
        &schedInOK
    };
    STest::RandomScenario<TestState> scenario("RandomScenario", rules,
                                              sizeof(rules) / sizeof(STest::RandomScenario<TestState>*));
    STest::BoundedScenario<TestState> boundedScenario("BoundedRandomScenario", scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(this->testState);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Random

}  // namespace Scenarios

}  // namespace Svc
