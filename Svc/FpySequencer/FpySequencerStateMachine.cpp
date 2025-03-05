#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

// ----------------------------------------------------------------------
// Functions to implement for internal state machine actions
// ----------------------------------------------------------------------

//! Implementation for action signalEntered of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! simply raises the "entered" signal
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_signalEntered(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->sequencer_sendSignal_entered();
}

//! Implementation for action setSequenceFilePath of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the current sequence file path member var
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setSequenceFilePath(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
) {
    m_sequenceFilePath = value.getfilePath();
}

//! Implementation for action setSequenceBlockState of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the block state of the sequence to be run
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setSequenceBlockState(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
) {
    m_sequenceBlockState = value.getblock();
}

//! Implementation for action report_seqSucceeded of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was completed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_ACTIVITY_HI_SequenceDone(m_sequenceFilePath);
}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_ACTIVITY_HI_SequenceCancelled(m_sequenceFilePath);
}

//! Implementation for action report_invalidCmd of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the user cmd was invalid
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_invalidCmd(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    FwEnumStoreType stateIdx = static_cast<FwEnumStoreType>(this->sequencer_getState());
    this->log_WARNING_LO_InvalidCommand(stateIdx);
}

//! Implementation for action report_invalidSeq of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that the sequence failed validation
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_invalidSeq(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_WARNING_HI_InvalidSequence(m_sequenceFilePath);
}

//! Implementation for action report_unexpectedStatementResponse of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! warns that a statement response came in unexpectedly
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_unexpectedStatementResponse(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_StatementResponse& value        //!< The value
) {
    this->log_WARNING_LO_UnexpectedStatementResponseForState(static_cast<I32>(sequencer_getState()), value.getopcode(),
                                                             value.getresponse());
}

//! Implementation for action report_seqFailed of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence failed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_WARNING_LO_SequenceFailed(m_sequenceFilePath);
}

//! Implementation for action stepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! iterates to the next statement and dispatches it
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_stepStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->stepStatement();
}

//! Implementation for action cancelNextStepStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! indicates to the component that the next call to stepStatement should
//! cancel
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cancelNextStepStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_runtime.cancelNextStatement = true;
    this->log_DIAGNOSTIC_CancellingOnNextStatement();
}

//! Implementation for action setGoalState_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to RUNNING
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::RUNNING;
}

//! Implementation for action setGoalState_VALID of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to VALID
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::VALID;
}

//! Implementation for action setGoalState_IDLE of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to IDLE
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_goalState = FpySequencer_GoalState::IDLE;
}

//! Implementation for action cmdResponseOut_OK of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with OK
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cmdResponseOut_OK(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
        // respond if we were waiting on a response
        this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq, Fw::CmdResponse::OK);
    }
}

//! Implementation for action cmdResponseOut_EXECUTION_ERROR of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with EXECUTION_ERROR
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_cmdResponseOut_EXECUTION_ERROR(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
        // respond if we were waiting on a response
        this->cmdResponse_out(m_savedOpCode, m_savedCmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

//! Implementation for action resetRuntime of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! resets the sequence runtime
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_resetRuntime(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    m_runtime = Runtime();
}

//! Implementation for action validate of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! performs all steps necessary for sequence validation, and raises a signal
//! result_success or result_failure
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_validate(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    bool result = this->validate();
    if (!result) {
        this->sequencer_sendSignal_result_failure();
        return;
    }
    this->sequencer_sendSignal_result_success();
}

// ----------------------------------------------------------------------
// Functions to implement for internal state machine guards
// ----------------------------------------------------------------------

//! Implementation for guard statementSuccessful of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! given a statement response, return true if the
//! statement successfully executed
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_statementSuccessful(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_StatementResponse& value        //!< The value
) const {
    return value.getresponse() == Fw::CmdResponse::OK;
}

//! Implementation for guard goalStateIs_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! return true if the goal state is RUNNING
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) const {
    return m_goalState == FpySequencer_GoalState::RUNNING;
}

}  // namespace Svc