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
    this->m_sequenceFilePath = value.getfilePath();
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
    this->m_sequenceBlockState = value.getblock();
}

//! Implementation for action report_seqSucceeded of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was completed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_tlm.sequencesSucceeded++;
    this->log_ACTIVITY_HI_SequenceDone(this->m_sequenceFilePath);
}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_tlm.sequencesFailed++;
    this->log_ACTIVITY_HI_SequenceCancelled(this->m_sequenceFilePath);
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
    this->log_WARNING_HI_InvalidSequence(this->m_sequenceFilePath);
}

//! Implementation for action report_seqFailed of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence failed
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_tlm.sequencesFailed++;
    this->log_WARNING_LO_SequenceFailed(this->m_sequenceFilePath);
}

//! Implementation for action dispatchStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! iterates to the next statement and dispatches it
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_dispatchStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->dispatchStatement();
}

//! Implementation for action setGoalState_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to RUNNING
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_goalState = FpySequencer_GoalState::RUNNING;
}

//! Implementation for action setGoalState_VALID of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to VALID
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_goalState = FpySequencer_GoalState::VALID;
}

//! Implementation for action setGoalState_IDLE of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! sets the goal state to IDLE
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_goalState = FpySequencer_GoalState::IDLE;
}

//! Implementation for action sendCmdResponse_OK of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with OK
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_sendCmdResponse_OK(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
        // respond if we were waiting on a response
        this->cmdResponse_out(this->m_savedOpCode, this->m_savedCmdSeq, Fw::CmdResponse::OK);
    }
}

//! Implementation for action sendCmdResponse_EXECUTION_ERROR of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! responds to the calling command with EXECUTION_ERROR
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->m_sequenceBlockState == FpySequencer_BlockState::BLOCK) {
        // respond if we were waiting on a response
        this->cmdResponse_out(this->m_savedOpCode, this->m_savedCmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
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
    this->m_runtime = Runtime();
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

//! Implementation for action checkShouldWake of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! checks if sequencer should wake from sleep
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    Fw::Time currentTime = this->getTime();

    if (currentTime.getTimeBase() != this->m_runtime.wakeupTime.getTimeBase()) {
        // cannot compare these times.
        this->log_WARNING_LO_MismatchedTimeBase(currentTime.getTimeBase(), this->m_runtime.wakeupTime.getTimeBase());

        this->sequencer_sendSignal_result_timeOpFailed();
        return;
    }

    if (currentTime.getContext() != this->m_runtime.wakeupTime.getContext()) {
        // cannot compare these times.
        this->log_WARNING_LO_MismatchedTimeContext(currentTime.getContext(), this->m_runtime.wakeupTime.getContext());

        this->sequencer_sendSignal_result_timeOpFailed();
        return;
    }

    if (currentTime < this->m_runtime.wakeupTime) {
        // not time to wake up!
        return;
    }

    // say we've finished our sleep
    this->sequencer_sendSignal_result_checkShouldWake_wakeup();
}

//! Implementation for action checkStatementTimeout of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! checks if the current statement has timed out
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    Fw::ParamValid valid;
    F32 timeout = this->paramGet_STATEMENT_TIMEOUT_SECS(valid);
    if (timeout <= 0) {
        // no timeout
        return;
    }

    Fw::Time currentTime = getTime();

    if (currentTime.getTimeBase() != this->m_runtime.currentStatementDispatchTime.getTimeBase()) {
        // can't compare time base. must have changed
        this->log_WARNING_LO_MismatchedTimeBase(currentTime.getTimeBase(),
                                                this->m_runtime.currentStatementDispatchTime.getTimeBase());
        this->sequencer_sendSignal_result_timeOpFailed();
        return;
    }
    if (currentTime.getContext() != this->m_runtime.currentStatementDispatchTime.getContext()) {
        // can't compare time ctx. must have changed
        this->log_WARNING_LO_MismatchedTimeContext(currentTime.getContext(),
                                                   this->m_runtime.currentStatementDispatchTime.getContext());
        this->sequencer_sendSignal_result_timeOpFailed();
        return;
    }

    F64 currentTimeSecs =
        static_cast<F64>(currentTime.getSeconds()) + static_cast<F64>(currentTime.getUSeconds()) / 1000000;
    F64 dispatchTimeSecs = static_cast<F64>(this->m_runtime.currentStatementDispatchTime.getSeconds()) +
                           static_cast<F64>(this->m_runtime.currentStatementDispatchTime.getUSeconds()) / 1000000;

    if (currentTimeSecs - dispatchTimeSecs >= timeout) {
        // timed out
        this->sequencer_sendSignal_statementTimeout();
    }
}

//! Implementation for action report_seqTimedOut of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! reports that the sequence timed out
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqTimedOut(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_tlm.sequencesFailed++;
    this->log_WARNING_LO_SequenceTimedOut(this->m_sequenceFilePath);
}

//! Implementation for action clearSequenceFile of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! clears all variables related to the loading/validating of the sequence file
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_clearSequenceFile(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_sequenceFilePath = "";
}

// ----------------------------------------------------------------------
// Functions to implement for internal state machine guards
// ----------------------------------------------------------------------

//! Implementation for guard goalStateIs_RUNNING of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! return true if the goal state is RUNNING
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) const {
    return this->m_goalState == FpySequencer_GoalState::RUNNING;
}

}  // namespace Svc