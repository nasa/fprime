// ======================================================================
// \title  RequestModePort.cpp
// \brief  Rule implementations for the requestMode port array
//
// Exercises the component request path: provenance is stamped COMPONENT,
// and the requester identity is derived from which port index the call
// arrived on -- never supplied by the caller. Port index 0 is skipped:
// Requester::NONE (value 0) is reserved for the ground path, so a real
// requester must occupy index 1 or above (see ModeManagerCfg.fpp).
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// RequestModePort.Transition
// ----------------------------------------------------------------------

bool ModeManagerTester ::RequestModePort__Transition__precondition() const {
    return true;
}

void ModeManagerTester ::RequestModePort__Transition__action() {
    this->clearHistory();

    const Mode::T fromMode = this->shadow.shadow_currentMode.e;
    const Mode::T target = ModeManagerTester::pickReachableMode();

    const FwIndexType portNum = static_cast<FwIndexType>(
        STest::Pick::lowerUpper(ModeManagerTester::FIRST_REAL_REQUESTER, ModeManagerTester::NUM_REQUESTERS - 1));
    // Requester enum values equal their requestMode port index by contract
    // (ModeManagerCfg.fpp, section 4.1 of the SDD).
    const Requester::T requester = static_cast<Requester::T>(portNum);

    this->invoke_to_requestMode(portNum, target);
    this->dispatchExactly(2);  // command, then the signal it sends

    this->assertPermittedTransition(fromMode, target, ModeRequestSource::COMPONENT, requester);
}

// ----------------------------------------------------------------------
// RequestModePort.ReservedPort
//
// Port index 0 is reserved: Requester::NONE (value 0) means "this came
// from ground", so a request arriving on index 0 must never be turned
// into a ModeRequest at all. It is rejected via the same TransitionRejected
// path any other rejection uses, deliberately not relabeled as GROUND --
// that would let a component wired here bypass a ground-only policy rule.
// The event label is imperfect (COMPONENT/NONE); what matters is that no
// signal is ever sent, so only one message is dispatched.
// ----------------------------------------------------------------------

bool ModeManagerTester ::RequestModePort__ReservedPort__precondition() const {
    return true;
}

void ModeManagerTester ::RequestModePort__ReservedPort__action() {
    this->clearHistory();

    const Mode::T target = ModeManagerTester::pickReachableMode();

    this->invoke_to_requestMode(0, target);
    this->dispatchExactly(1);  // rejected before any signal is ever sent

    this->assertDeniedTransition(target, ModeRequestSource::COMPONENT, Requester::NONE, ModePolicySource::DEFAULT);
}

}  // namespace Svc
