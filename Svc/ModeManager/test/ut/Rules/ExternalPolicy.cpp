// ======================================================================
// \title  ExternalPolicy.cpp
// \brief  Rule implementations exercising an external policy on
//         checkTransition
//
// These rules only make sense with a tester constructed with
// connectExternalPolicy = true. They are not part of the large random
// scenario in ModeManagerTestMain.cpp -- they run as small directed
// tests against a separately constructed tester.
//
// The policy's specific logic is irrelevant here; it is a controllable
// stub (m_policyResponse). What is under test is whether the component
// obeys whatever answer it receives, not whether the answer itself was
// wise -- that is a project's responsibility when it supplies a real
// policy.
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// ExternalPolicy.Permits
// ----------------------------------------------------------------------

bool ModeManagerTester ::ExternalPolicy__Permits__precondition() const {
    return true;
}

void ModeManagerTester ::ExternalPolicy__Permits__action() {
    this->clearHistory();

    const Mode::T fromMode = this->shadow.shadow_currentMode.e;
    const Mode::T target = ModeManagerTester::pickReachableMode();

    this->m_policyResponse = Fw::Success::SUCCESS;
    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, target);
    this->dispatchExactly(2);  // approved: command, then the signal it sends
    this->m_cmdSeq++;

    ASSERT_from_checkTransition_SIZE(1);
    ModeRequest expectedReq;
    expectedReq.set_source(ModeRequestSource::GROUND);
    expectedReq.set_requester(Requester::NONE);
    ASSERT_from_checkTransition(0, fromMode, target, expectedReq);

    this->assertPermittedTransition(fromMode, target, ModeRequestSource::GROUND, Requester::NONE);
}

// ----------------------------------------------------------------------
// ExternalPolicy.Denies
// ----------------------------------------------------------------------

bool ModeManagerTester ::ExternalPolicy__Denies__precondition() const {
    return true;
}

void ModeManagerTester ::ExternalPolicy__Denies__action() {
    this->clearHistory();

    const Mode::T target = ModeManagerTester::pickReachableMode();

    this->m_policyResponse = Fw::Success::FAILURE;
    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, target);
    this->dispatchExactly(1);  // denied: handleRequest() returns before sending any signal
    this->m_cmdSeq++;

    ASSERT_from_checkTransition_SIZE(1);

    this->assertDeniedTransition(target, ModeRequestSource::GROUND, Requester::NONE, ModePolicySource::EXTERNAL);
}

// ----------------------------------------------------------------------
// ExternalPolicy.DeniesEvenObviousTransition
//
// Proves the external policy takes precedence over the shipped default
// rather than merely being polled: it denies IDLE -> SCIENCE, a
// transition the (permit-everything) default would never refuse. The
// rule first drives itself to IDLE (policy set to permit) so it is
// self-contained regardless of what ran before it.
// ----------------------------------------------------------------------

bool ModeManagerTester ::ExternalPolicy__DeniesEvenObviousTransition__precondition() const {
    return true;
}

void ModeManagerTester ::ExternalPolicy__DeniesEvenObviousTransition__action() {
    // Drive to a known state first.
    this->clearHistory();
    this->m_policyResponse = Fw::Success::SUCCESS;
    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, Mode::IDLE);
    this->dispatchExactly(2);  // approved: command, then the signal it sends
    this->m_cmdSeq++;
    this->assertPermittedTransition(this->shadow.shadow_currentMode.e, Mode::IDLE, ModeRequestSource::GROUND,
                                     Requester::NONE);

    // Now deny a transition nothing else in this component would ever
    // refuse.
    this->clearHistory();
    this->m_policyResponse = Fw::Success::FAILURE;
    this->sendCmd_REQUEST_MODE(ModeManagerTester::TEST_INSTANCE_ID, this->m_cmdSeq, Mode::SCIENCE);
    this->dispatchExactly(1);  // denied: handleRequest() returns before sending any signal
    this->m_cmdSeq++;

    this->assertDeniedTransition(Mode::SCIENCE, ModeRequestSource::GROUND, Requester::NONE,
                                  ModePolicySource::EXTERNAL);

    // Confirm the mode really is still IDLE, not just that no event fired.
    this->clearHistory();
    const Mode returned = this->invoke_to_getMode(0);
    ASSERT_EQ(returned.e, Mode::IDLE) << "Denied transition must leave the mode unchanged";
}

}  // namespace Svc
