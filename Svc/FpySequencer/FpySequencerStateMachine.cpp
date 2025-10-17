#include <new>
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
    this->m_sequenceFilePath = value.get_filePath();
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
    this->m_sequenceBlockState = value.get_block();
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
    if (this->isConnected_seqDoneOut_OutputPort(0)) {
        // report that the sequence succeeded to internal callers
        this->seqDoneOut_out(0, 0, 0, Fw::CmdResponse::OK);
    }
}

//! Implementation for action report_seqCancelled of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was cancelled
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_tlm.sequencesCancelled++;
    this->log_ACTIVITY_HI_SequenceCancelled(this->m_sequenceFilePath);
    if (this->isConnected_seqDoneOut_OutputPort(0)) {
        // report that the sequence failed to internal callers
        this->seqDoneOut_out(0, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

//! Implementation for action dispatchStatement of state machine
//! Svc_FpySequencer_SequencerStateMachine
//!
//! iterates to the next statement and dispatches it
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_dispatchStatement(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    Signal result = this->dispatchStatement();
    switch (result) {
        case Signal::result_dispatchStatement_noMoreStatements: {
            this->sequencer_sendSignal_result_dispatchStatement_noMoreStatements();
            break;
        }
        case Signal::result_dispatchStatement_success: {
            this->sequencer_sendSignal_result_dispatchStatement_success();
            break;
        }
        case Signal::result_dispatchStatement_failure: {
            this->sequencer_sendSignal_result_dispatchStatement_failure();
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(result));
        }
    }
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
    // explicitly call dtor
    this->m_runtime.~Runtime();
    new (&this->m_runtime) Runtime();
    Fw::ParamValid valid;
    this->m_runtime.flags[Fpy::FlagId::EXIT_ON_CMD_FAIL] = this->paramGet_FLAG_DEFAULT_EXIT_ON_CMD_FAIL(valid);
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
    Fw::Success result = this->validate();
    if (result == Fw::Success::FAILURE) {
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
    Signal result = this->checkShouldWake();
    switch (result) {
        case Signal::result_checkShouldWake_keepSleeping: {
            this->sequencer_sendSignal_result_checkShouldWake_keepSleeping();
            break;
        }
        case Signal::result_checkShouldWake_wakeup: {
            this->sequencer_sendSignal_result_checkShouldWake_wakeup();
            break;
        }
        case Signal::result_timeOpFailed: {
            this->sequencer_sendSignal_result_timeOpFailed();
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(result));
        }
    }
}

//! Implementation for action checkStatementTimeout of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! checks if the current statement has timed out
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    Signal result = this->checkStatementTimeout();
    switch (result) {
        case Signal::result_checkStatementTimeout_noTimeout: {
            this->sequencer_sendSignal_result_checkStatementTimeout_noTimeout();
            break;
        }
        case Signal::result_checkStatementTimeout_statementTimeout: {
            this->sequencer_sendSignal_result_checkStatementTimeout_statementTimeout();
            break;
        }
        case Signal::result_timeOpFailed: {
            this->sequencer_sendSignal_result_timeOpFailed();
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(result));
        }
    }
}

//! Implementation for action incrementSequenceCounter of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! increments the m_sequencesStarted counter
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_incrementSequenceCounter(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_sequencesStarted++;
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

//! Implementation for action clearBreakpoint of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! clears the breakpoint, allowing execution of the sequence to continue
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_clearBreakpoint(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_breakpoint.breakpointInUse = false;
    this->m_breakpoint.breakpointIndex = 0;
    this->m_breakpoint.breakOnlyOnceOnBreakpoint = false;
    this->m_breakpoint.breakBeforeNextLine = false;
}

//! Implementation for action report_seqBroken of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a breakpoint was hit
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqBroken(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->log_ACTIVITY_HI_SequencePaused(this->m_runtime.nextStatementIndex);
}

//! Implementation for action setBreakpoint of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! sets the breakpoint to the provided args
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setBreakpoint(
    SmId smId,                                              //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
    const Svc::FpySequencer_BreakpointArgs& value           //!< The value
) {
    this->m_breakpoint.breakpointInUse = value.get_breakOnBreakpoint();
    this->m_breakpoint.breakOnlyOnceOnBreakpoint = value.get_breakOnlyOnceOnBreakpoint();
    this->m_breakpoint.breakpointIndex = value.get_breakpointIndex();
    this->log_ACTIVITY_HI_BreakpointSet(value.get_breakpointIndex(), value.get_breakOnlyOnceOnBreakpoint());
}

//! Implementation for action setBreakBeforeNextLine of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! sets the "break on next line" flag to true
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_breakpoint.breakBeforeNextLine = true;
}

//! Implementation for action clearBreakBeforeNextLine of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! sets the "break on next line" flag to false
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    this->m_breakpoint.breakBeforeNextLine = false;
}

//! Implementation for action report_seqFailed of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! called when a sequence failed to execute successfully
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->isConnected_seqDoneOut_OutputPort(0)) {
        // report that the sequence failed to internal callers
        this->seqDoneOut_out(0, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

//! Implementation for action report_seqStarted of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! reports that a sequence was started
void FpySequencer::Svc_FpySequencer_SequencerStateMachine_action_report_seqStarted(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) {
    if (this->isConnected_seqDoneOut_OutputPort(0)) {
        // report that the sequence started to internal callers
        this->seqStartOut_out(0, this->m_sequenceFilePath);
    }
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

//! Implementation for guard shouldBreak of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! return true if should break at this point in execution, before dispatching
//! next stmt
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_shouldBreak(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) const {
    // there are really two mechanisms for pausing the execution of the seq
    // one is the "break on next line flag", and the other is the breakpoint
    return this->m_breakpoint.breakBeforeNextLine ||
           (this->m_breakpoint.breakpointInUse &&
            this->m_breakpoint.breakpointIndex == this->m_runtime.nextStatementIndex);
}

//! Implementation for guard breakOnce of state machine Svc_FpySequencer_SequencerStateMachine
//!
//! return true if this breakpoint should only happen once
bool FpySequencer::Svc_FpySequencer_SequencerStateMachine_guard_breakOnce(
    SmId smId,                                             //!< The state machine id
    Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
) const {
    return this->m_breakpoint.breakOnlyOnceOnBreakpoint;
}
}  // namespace Svc
