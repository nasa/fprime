// ======================================================================
// \title  RequestModeCmd.cpp
// \brief  Rule implementations for the REQUEST_MODE ground command
//
// Under the shipped default policy (checkTransition unconnected in this
// tester instance), every transition is permitted, so this rule always
// expects success. It exercises the ground request path: provenance is
// stamped GROUND / NONE by the component itself, not supplied by the
// caller.
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// RequestModeCmd.Transition
// ----------------------------------------------------------------------

bool ModeManagerTester ::RequestModeCmd__Transition__precondition() const {
    return true;
}

void ModeManagerTester ::RequestModeCmd__Transition__action() {
    this->clearHistory();

    const Mode::T fromMode = this->shadow.shadow_currentMode.e;
    const Mode::T target = ModeManagerTester::pickReachableMode();

    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, target);
    this->dispatchExactly(2);  // command, then the signal it sends

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ModeManager::OPCODE_REQUEST_MODE, this->m_cmdSeq, Fw::CmdResponse::OK);
    this->m_cmdSeq++;

    this->assertPermittedTransition(fromMode, target, ModeRequestSource::GROUND, Requester::NONE);
}

// ----------------------------------------------------------------------
// RequestModeCmd.UnsupportedTarget
//
// STARTUP is a valid Mode enum member -- ground can legally send it -- but
// has no entry in ModeList.inc, since nothing transitions into it. This
// is rejected by isRequestableMode() before any policy (default or
// external) is ever consulted. Without this check, sendTransitionSignal()
// would hit its FW_ASSERT default case on a ground-reachable value: the
// exact CPP-4 hazard this rejection path exists to prevent.
// ----------------------------------------------------------------------

bool ModeManagerTester ::RequestModeCmd__UnsupportedTarget__precondition() const {
    return true;
}

void ModeManagerTester ::RequestModeCmd__UnsupportedTarget__action() {
    this->clearHistory();

    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, Mode::STARTUP);
    this->dispatchExactly(1);  // rejected before any signal is ever sent

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ModeManager::OPCODE_REQUEST_MODE, this->m_cmdSeq, Fw::CmdResponse::OK);
    this->m_cmdSeq++;

    this->assertDeniedTransition(Mode::STARTUP, ModeRequestSource::GROUND, Requester::NONE,
                                  ModePolicySource::DEFAULT);
}

}  // namespace Svc
