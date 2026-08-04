// ======================================================================
// \title  ModeManagerTester.cpp
// \brief  cpp file for ModeManager component rule-based test harness
// ======================================================================

#include "Svc/ModeManager/test/ut/ModeManagerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ModeManagerTester ::ModeManagerTester(bool connectExternalPolicy)
    : ModeManagerGTestBase("ModeManagerTester", ModeManagerTester::MAX_HISTORY_SIZE),
      component("ModeManager"),
      m_connectExternalPolicy(connectExternalPolicy) {
    this->initComponents();
    this->connectPorts();
}

ModeManagerTester ::~ModeManagerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void ModeManagerTester ::connectPorts() {
    // Connect special input ports

    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    // Connect special output ports

    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));

#if FW_ENABLE_TEXT_LOGGING == 1
    this->component.set_logTextOut_OutputPort(0, this->get_from_logTextOut(0));
#endif

    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // Connect typed input ports

    this->connect_to_getMode(0, this->component.get_getMode_InputPort(0));
    this->connect_to_pingIn(0, this->component.get_pingIn_InputPort(0));

    for (FwIndexType i = 0; i < ModeManagerTester::NUM_REQUESTERS; i++) {
        this->connect_to_requestMode(i, this->component.get_requestMode_InputPort(i));
    }

    // Connect typed output ports

    // checkTransition is deliberately left unconnected unless the test
    // explicitly asks for an external policy. See the header comment.
    if (this->m_connectExternalPolicy) {
        this->component.set_checkTransition_OutputPort(0, this->get_from_checkTransition(0));
    }

    for (FwIndexType i = 0; i < ModeManagerTester::NUM_SUBSCRIBERS; i++) {
        this->component.set_modeChanged_OutputPort(i, this->get_from_modeChanged(i));
    }

    this->component.set_pingOut_OutputPort(0, this->get_from_pingOut(0));
}

void ModeManagerTester ::initComponents() {
    this->init();
    // component.init() already creates the component's message queue;
    // a separate createQueue() call here would try to create it twice.
    this->component.init(ModeManagerTester::TEST_INSTANCE_QUEUE_DEPTH, ModeManagerTester::TEST_INSTANCE_ID);
}

// ----------------------------------------------------------------------
// Shared assertion helpers
// ----------------------------------------------------------------------

void ModeManagerTester ::dispatchExactly(FwSizeType numMessages) {
    for (FwSizeType i = 0; i < numMessages; i++) {
        const Fw::QueuedComponentBase::MsgDispatchStatus status = this->component.doDispatch();
        // A wrong count here is a test bug, not a runtime condition: fail
        // loudly rather than let the next call block forever on an empty
        // queue.
        FW_ASSERT(status == Fw::QueuedComponentBase::MSG_DISPATCH_OK, static_cast<FwAssertArgType>(status));
    }
}

void ModeManagerTester ::assertModeChangedBroadcast(Mode::T fromMode, Mode::T toMode) {
    ASSERT_from_modeChanged_SIZE(static_cast<U32>(ModeManagerTester::NUM_SUBSCRIBERS));
    for (FwIndexType i = 0; i < ModeManagerTester::NUM_SUBSCRIBERS; i++) {
        ASSERT_from_modeChanged(static_cast<U32>(i), fromMode, toMode);
    }
}

void ModeManagerTester ::assertPermittedTransition(Mode::T fromMode,
                                                     Mode::T toMode,
                                                     ModeRequestSource::T source,
                                                     Requester::T requester) {
    ASSERT_EVENTS_TransitionRejected_SIZE(0);
    ASSERT_EVENTS_ModeTransitioned_SIZE(1);
    ASSERT_EVENTS_ModeTransitioned(0, fromMode, toMode, source, requester);
    ASSERT_TLM_CurrentMode_SIZE(1);
    ASSERT_TLM_CurrentMode(0, toMode);
    this->assertModeChangedBroadcast(fromMode, toMode);
    this->shadow.shadow_currentMode = toMode;
}

void ModeManagerTester ::assertDeniedTransition(Mode::T requested,
                                                  ModeRequestSource::T source,
                                                  Requester::T requester,
                                                  ModePolicySource::T decidedBy) {
    ASSERT_EVENTS_ModeTransitioned_SIZE(0);
    ASSERT_EVENTS_TransitionRejected_SIZE(1);
    ASSERT_EVENTS_TransitionRejected(0, this->shadow.shadow_currentMode.e, requested, source, requester, decidedBy);
    ASSERT_TLM_CurrentMode_SIZE(0);
    ASSERT_from_modeChanged_SIZE(0);
    // Shadow is intentionally left unchanged: the mode did not move.
}

Mode::T ModeManagerTester ::pickReachableMode() {
    // Reachable targets are IDLE(1), SCIENCE(2), SAFE(3); STARTUP(0) has no
    // inbound signal in ModeMachine.fpp and is never a legal request target.
    return static_cast<Mode::T>(STest::Pick::lowerUpper(1, 3));
}

// ----------------------------------------------------------------------
// Handler overrides
// ----------------------------------------------------------------------

Fw::Success ModeManagerTester ::from_checkTransition_handler(FwIndexType portNum,
                                                                const Mode& current,
                                                                const Mode& target,
                                                                const ModeRequest& req) {
    this->pushFromPortEntry_checkTransition(current, target, req);
    return this->m_policyResponse;
}

}  // namespace Svc
