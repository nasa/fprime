// ======================================================================
// \title  ModeManager.cpp
// \brief  cpp file for ModeManager component implementation class
// ======================================================================

#include "Svc/ModeManager/ModeManager.hpp"
#include "Svc/ModeManager/config/ModeManagerConfig/ModeManagerCfg.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ModeManager ::ModeManager(const char* const compName)
    : ModeManagerComponentBase(compName),
      m_currentMode(Mode::STARTUP),
      m_pendingFrom(Mode::STARTUP),
      m_pendingTarget(Mode::STARTUP) {}

ModeManager ::~ModeManager() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

Svc::Mode ModeManager ::getMode_handler(FwIndexType portNum) {
    return this->m_currentMode;
}

void ModeManager ::pingIn_handler(FwIndexType portNum, U32 key) {
    this->pingOut_out(portNum, key);
}

void ModeManager ::requestMode_handler(FwIndexType portNum, const Svc::Mode& target) {
    // Port index 0 is reserved: Requester::NONE (value 0) means "this came
    // from ground", so stamping a component request with it would produce
    // an event that looks like a ground request. Reject via the same
    // TransitionRejected path any other rejection uses -- deliberately not
    // relabeled as GROUND, which would let a component wired here bypass
    // any ground-only policy rule (defeats MM-011's whole purpose). The
    // label is imperfect (COMPONENT/NONE) but the request is never granted
    // trust, which is what actually matters.
    if (portNum == 0) {
        this->log_WARNING_LO_TransitionRejected(this->m_currentMode, target, ModeRequestSource::COMPONENT,
                                                 Requester::NONE, ModePolicySource::DEFAULT);
        return;
    }

    ModeRequest req;
    req.set_source(ModeRequestSource::COMPONENT);
    // Requester values equal their requestMode port index by contract
    // (ModeManagerCfg.fpp). portNum is bounded to [1, NUM_REQUESTERS) here
    // (0 was rejected above) and to [0, NUM_REQUESTERS) by the generated
    // port dispatch, so this cast always lands on a real requester.
    req.set_requester(static_cast<Requester::T>(portNum));
    this->handleRequest(target.e, req);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void ModeManager ::START_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    ModeRequest req;
    req.set_source(ModeRequestSource::GROUND);
    req.set_requester(Requester::NONE);
    this->handleRequest(Mode::IDLE, req);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void ModeManager ::REQUEST_MODE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Svc::Mode& target) {
    ModeRequest req;
    req.set_source(ModeRequestSource::GROUND);
    req.set_requester(Requester::NONE);
    this->handleRequest(target.e, req);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void ModeManager ::Svc_ModeMachine_action_announce(SmId smId,
                                                   Svc_ModeMachine::Signal signal,
                                                   const Svc::ModeRequest& value) {
    FW_ASSERT(smId == SmId::modeSm, static_cast<FwAssertArgType>(smId));

    const Mode::T fromMode = this->m_pendingFrom;
    const Mode::T toMode = this->m_pendingTarget;

    this->m_currentMode = toMode;

    this->tlmWrite_CurrentMode(toMode);
    this->log_ACTIVITY_HI_ModeTransitioned(fromMode, toMode, value.get_source(), value.get_requester());

    for (FwIndexType i = 0; i < this->getNum_modeChanged_OutputPorts(); i++) {
        if (this->isConnected_modeChanged_OutputPort(i)) {
            this->modeChanged_out(i, fromMode, toMode);
        }
    }
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

void ModeManager ::handleRequest(Mode::T target, const ModeRequest& req) {
    const Mode::T current = this->m_currentMode;

    if (!this->isRequestableMode(target)) {
        // Not a policy decision: target has no inbound signal in
        // ModeMachine.fpp at all (e.g. STARTUP, or a mode a project's
        // override removed). Reported as DEFAULT since no policy -- ours
        // or an external one -- was ever consulted.
        this->log_WARNING_LO_TransitionRejected(current, target, req.get_source(), req.get_requester(),
                                                 ModePolicySource::DEFAULT);
        return;
    }

    ModePolicySource::T decidedBy;
    const bool allowed = this->transitionAllowed(current, target, req, decidedBy);

    if (!allowed) {
        this->log_WARNING_LO_TransitionRejected(current, target, req.get_source(), req.get_requester(), decidedBy);
        return;
    }

    // Capture both ends of the transition before signalling: announce()
    // receives only the ModeRequest, not the state machine's state, so
    // this is the only place either mode is known.
    this->m_pendingFrom = current;
    this->m_pendingTarget = target;
    this->sendTransitionSignal(target, req);
}

bool ModeManager ::isRequestableMode(Mode::T target) const {
    switch (target) {
        // X-macro over the project-supplied ModeList.inc: one case per
        // requestable mode, generated rather than hand-written so this
        // switch can never drift out of sync with sendTransitionSignal()
        // below, which is driven by the same list.
#define MODE_ENTRY(mode, signal) \
    case Mode::mode:             \
        return true;
#include "Svc/ModeManager/config/ModeManagerConfig/ModeList.inc"
#undef MODE_ENTRY
        default:
            return false;
    }
}

bool ModeManager ::transitionAllowed(Mode::T current,
                                      Mode::T target,
                                      const ModeRequest& req,
                                      ModePolicySource::T& decidedBy) const {
    if (this->isConnected_checkTransition_OutputPort(0)) {
        decidedBy = ModePolicySource::EXTERNAL;
        return this->checkTransition_out(0, current, target, req) == Fw::Success::SUCCESS;
    }
    decidedBy = ModePolicySource::DEFAULT;
    return ModeManagerCfg::defaultPolicy(current, target, req);
}

void ModeManager ::sendTransitionSignal(Mode::T target, const ModeRequest& req) {
    switch (target) {
        // Token-pasting (##signal) is required here, not a stylistic choice:
        // modeSm_sendSignal_<signal> is a protected member of the generated
        // state machine base class, callable only from ModeManager's own
        // member functions. A project-supplied free function or lookup
        // table could not call it; only text substituted directly into this
        // method body can. See ModeList.inc for the full rationale.
#define MODE_ENTRY(mode, signal)             \
    case Mode::mode:                         \
        this->modeSm_sendSignal_##signal(req); \
        return;
#include "Svc/ModeManager/config/ModeManagerConfig/ModeList.inc"
#undef MODE_ENTRY
        default:
            // Precondition violated: callers must check isRequestableMode()
            // first. handleRequest() is the only caller and does so, so
            // reaching here is a programming error, not untrusted input.
            FW_ASSERT(0, static_cast<FwAssertArgType>(target));
    }
}

}  // namespace Svc
