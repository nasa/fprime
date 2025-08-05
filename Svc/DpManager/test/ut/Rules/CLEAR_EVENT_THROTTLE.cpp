// ======================================================================
// \title  CLEAR_EVENT_THROTTLE.cpp
// \author Rob Bocchino
// \brief  CLEAR_EVENT_THROTTLE class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include "Svc/DpManager/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState::precondition__CLEAR_EVENT_THROTTLE__OK() const {
    return true;
}

void TestState::action__CLEAR_EVENT_THROTTLE__OK() {
    // Clear history
    this->clearHistory();
    // Send the command
    const FwEnumStoreType instance = static_cast<FwEnumStoreType>(STest::Pick::any());
    const U32 cmdSeq = STest::Pick::any();
    this->sendCmd_CLEAR_EVENT_THROTTLE(instance, cmdSeq);
    this->doDispatch();
    // Check the command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::DpManagerTester::getClearEventThrottleOpcode(), cmdSeq, Fw::CmdResponse::OK);
    // Check the state
    ASSERT_EQ(Svc::DpManagerTester::getBufferAllocationFailedThrottleCount(), 0);
    this->abstractState.bufferAllocationFailedEventCount = 0;
}

namespace CLEAR_EVENT_THROTTLE {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::OK() {
    Testers::bufferGetStatus.ruleInvalid.apply(this->testState);
    for (FwSizeType i = 0; i <= Svc::DpManagerTester::getBufferAllocationFailedThrottle(); ++i) {
        Testers::productRequestIn.ruleBufferInvalid.apply(this->testState);
    }
    this->ruleOK.apply(this->testState);
    Testers::productRequestIn.ruleBufferInvalid.apply(this->testState);
}

}  // namespace CLEAR_EVENT_THROTTLE

}  // namespace Svc
