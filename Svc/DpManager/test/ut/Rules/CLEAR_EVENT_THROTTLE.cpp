// ======================================================================
// \title  CLEAR_EVENT_THROTTLE.cpp
// \author Rob Bocchino
// \brief  CLEAR_EVENT_THROTTLE class implementation
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/DpManager/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
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
    const NATIVE_INT_TYPE instance = static_cast<NATIVE_INT_TYPE>(STest::Pick::any());
    const U32 cmdSeq = STest::Pick::any();
    this->sendCmd_CLEAR_EVENT_THROTTLE(instance, cmdSeq);
    this->component.doDispatch();
    // Check the command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpManagerComponentBase::OPCODE_CLEAR_EVENT_THROTTLE, cmdSeq, Fw::CmdResponse::OK);
    // Check the state
    ASSERT_EQ(this->component.DpManagerComponentBase::m_BufferAllocationFailedThrottle, 0);
    this->abstractState.bufferAllocationFailedEventCount = 0;
}

namespace CLEAR_EVENT_THROTTLE {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::OK() {
    Testers::bufferGetStatus.ruleInvalid.apply(this->testState);
    for (FwSizeType i = 0; i <= DpManagerComponentBase::EVENTID_BUFFERALLOCATIONFAILED_THROTTLE; ++i) {
        Testers::productRequestIn.ruleBufferInvalid.apply(this->testState);
    }
    this->ruleOK.apply(this->testState);
    Testers::productRequestIn.ruleBufferInvalid.apply(this->testState);
}

}  // namespace CLEAR_EVENT_THROTTLE

}  // namespace Svc
