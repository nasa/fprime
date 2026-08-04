// ======================================================================
// \title  Start.cpp
// \brief  Rule implementations for the Start command
//
// START is a convenience command: leaving the initial state always means
// requesting IDLE from ground. It goes through the same
// transitionAllowed() policy check as REQUEST_MODE; it is not a separate
// mechanism.
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Start.ToIdle
// ----------------------------------------------------------------------

bool ModeManagerTester ::Start__ToIdle__precondition() const {
    // Always legal to attempt; harmless to call again after leaving
    // STARTUP under the shipped (permit-everything) default policy.
    return true;
}

void ModeManagerTester ::Start__ToIdle__action() {
    this->clearHistory();

    const Mode::T fromMode = this->shadow.shadow_currentMode.e;
    this->sendCmd_START(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq);

    this->dispatchExactly(2);  // command, then the signal it sends

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ModeManager::OPCODE_START, this->m_cmdSeq, Fw::CmdResponse::OK);
    this->m_cmdSeq++;

    this->assertPermittedTransition(fromMode, Mode::IDLE, ModeRequestSource::GROUND, Requester::NONE);
}

}  // namespace Svc
