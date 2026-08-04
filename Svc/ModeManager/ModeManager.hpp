// ======================================================================
// \title  ModeManager.hpp
// \brief  hpp file for ModeManager component implementation class
// ======================================================================

#ifndef Svc_ModeManager_HPP
#define Svc_ModeManager_HPP

#include "Svc/ModeManager/ModeManagerComponentAc.hpp"

namespace Svc {

class ModeManager final : public ModeManagerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ModeManager object
    ModeManager(const char* const compName  //!< The component name
    );

    //! Destroy ModeManager object
    ~ModeManager();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for getMode
    //!
    //! Current mode query. Guarded because it executes in the caller's
    //! thread and reads state this component's thread may be writing.
    Svc::Mode getMode_handler(FwIndexType portNum  //!< The port number
                              ) override;

    //! Handler implementation for pingIn
    //!
    //! Ping input port
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

    //! Handler implementation for requestMode
    //!
    //! Transition requests from other components. The port index
    //! identifies the requester.
    void requestMode_handler(FwIndexType portNum,     //!< The port number
                             const Svc::Mode& target  //!< The requested mode
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command START
    //!
    //! Leave STARTUP and begin operations
    void START_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command REQUEST_MODE
    //!
    //! Request a transition to the given mode
    void REQUEST_MODE_cmdHandler(FwOpcodeType opCode,     //!< The opcode
                                 U32 cmdSeq,              //!< The command sequence number
                                 const Svc::Mode& target  //!< The requested mode
                                 ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action announce of state machine Svc_ModeMachine
    //!
    //! Record the new mode, emit telemetry and the transition event,
    //! and notify all connected subscribers
    void Svc_ModeMachine_action_announce(SmId smId,                       //!< The state machine id
                                         Svc_ModeMachine::Signal signal,  //!< The signal
                                         const Svc::ModeRequest& value    //!< The value
                                         ) override;

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Common path for every transition request, regardless of origin.
    //! Checks whether the target is a requestable mode, applies the active
    //! policy, and on approval signals the mode machine. On denial, emits
    //! TransitionRejected and leaves the mode unchanged.
    void handleRequest(Svc::Mode::T target, const Svc::ModeRequest& req);

    //! True if target has a corresponding entry in ModeList.inc, i.e. is a
    //! mode the mode machine can actually be signaled into. STARTUP is
    //! never requestable: nothing transitions into it.
    bool isRequestableMode(Svc::Mode::T target) const;

    //! Ask the active policy whether (current -> target) is permitted.
    //! Consults checkTransition when connected; falls back to the
    //! configured default policy otherwise. Reports which one decided.
    bool transitionAllowed(Svc::Mode::T current,
                            Svc::Mode::T target,
                            const Svc::ModeRequest& req,
                            Svc::ModePolicySource::T& decidedBy) const;

    //! Send the signal corresponding to target. Precondition:
    //! isRequestableMode(target) is true.
    void sendTransitionSignal(Svc::Mode::T target, const Svc::ModeRequest& req);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The current mode. This, not the generated state machine's own state
    //! variable, is the source of truth for getMode, telemetry, and events
    //! -- it is updated at a single well-defined point (announce) rather
    //! than depending on when the generated state variable changes
    //! relative to transition actions.
    Svc::Mode::T m_currentMode;

    //! The mode being left, captured in handleRequest() before the signal
    //! is sent. announce() has no other way to learn the previous mode:
    //! it receives only the ModeRequest, not the state machine's state.
    Svc::Mode::T m_pendingFrom;

    //! The mode being entered, captured alongside m_pendingFrom for the
    //! same reason.
    Svc::Mode::T m_pendingTarget;
};

}  // namespace Svc

#endif
