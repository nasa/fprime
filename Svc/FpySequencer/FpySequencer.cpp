// ======================================================================
// \title  FpySequencer.cpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer component implementation class
// ======================================================================

#include <Svc/FpySequencer/FpySequencer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FpySequencer ::FpySequencer(const char* const compName)
    : FpySequencerComponentBase(compName),
      m_sequenceBuffer(),
      m_allocatorId(0),
      m_sequenceFilePath("<invalid_seq>"),
      m_sequenceObj(),
      m_computedCRC(0),
      m_sequenceBlockState(),
      m_savedOpCode(0),
      m_savedCmdSeq(0),
      m_goalState(),
      m_sequencesStarted(0),
      m_statementsDispatched(0),
      m_runtime(),
      m_debug(),
      m_tlm() {}

FpySequencer ::~FpySequencer() {}

//! Handler for command RUN
//!
//! Loads, validates and runs a sequence
void FpySequencer::RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                                  U32 cmdSeq,                        //!< The command sequence number
                                  const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                                  FpySequencer_BlockState block      //!< Return command status when complete or not
) {
    // can only run a seq while in idle
    if (sequencer_getState() != State::IDLE) {
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (block == FpySequencer_BlockState::BLOCK) {
        // save the opCode and cmdSeq so we can respond later
        this->m_savedOpCode = opCode;
        this->m_savedCmdSeq = cmdSeq;
    }

    this->sequencer_sendSignal_cmd_RUN(FpySequencer_SequenceExecutionArgs(fileName, block));

    // only respond if the user doesn't want us to block further execution
    if (block == FpySequencer_BlockState::NO_BLOCK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}

//! Handler for command VALIDATE
//!
//! Loads and validates a sequence
void FpySequencer::VALIDATE_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                                       U32 cmdSeq,                       //!< The command sequence number
                                       const Fw::CmdStringArg& fileName  //!< The name of the sequence file
) {
    // can only validate a seq while in idle
    if (sequencer_getState() != State::IDLE) {
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // validate always blocks until finished, so save opcode/cmdseq
    // so we can respond once done
    this->m_savedOpCode = opCode;
    this->m_savedCmdSeq = cmdSeq;

    this->sequencer_sendSignal_cmd_VALIDATE(
        FpySequencer_SequenceExecutionArgs(fileName, FpySequencer_BlockState::BLOCK));
}

//! Handler for command RUN_VALIDATED
//!
//! Runs a previously validated sequence
void FpySequencer::RUN_VALIDATED_cmdHandler(
    FwOpcodeType opCode,           //!< The opcode
    U32 cmdSeq,                    //!< The command sequence number
    FpySequencer_BlockState block  //!< Return command status when complete or not
) {
    // can only RUN_VALIDATED if we have validated and are awaiting this exact cmd
    if (sequencer_getState() != State::AWAITING_CMD_RUN_VALIDATED) {
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (block == FpySequencer_BlockState::BLOCK) {
        // save the opCode and cmdSeq so we can respond later
        this->m_savedOpCode = opCode;
        this->m_savedCmdSeq = cmdSeq;
    }

    this->sequencer_sendSignal_cmd_RUN_VALIDATED(FpySequencer_SequenceExecutionArgs(this->m_sequenceFilePath, block));

    // only respond if the user doesn't want us to block further execution
    if (block == FpySequencer_BlockState::NO_BLOCK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}

//! Handler for command CANCEL
//!
//! Cancels a running or validated sequence
void FpySequencer::CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                     U32 cmdSeq            //!< The command sequence number
) {
    // only state you can't cancel in is IDLE
    if (sequencer_getState() == State::IDLE) {
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->sequencer_sendSignal_cmd_CANCEL();

    // cancel returns immediately and always succeeds
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command DEBUG_SET_BREAKPOINT
//!
//! Sets the debugging breakpoint which will pause the execution of the sequencer
//! until unpaused by the DEBUG_CONTINUE command. Will pause just before dispatching
//! the specified statement. This command is valid in all states. Debug settings are
//! cleared after a sequence ends execution.
void FpySequencer::DEBUG_SET_BREAKPOINT_cmdHandler(
    FwOpcodeType opCode,  //!< The opcode
    U32 cmdSeq,           //!< The command sequence number
    U32 stmtIdx,          //!< The statement index to pause execution before.
    bool breakOnce        //!< Whether or not to break only once at this breakpoint
) {
    this->sequencer_sendSignal_cmd_DEBUG_SET_BREAKPOINT(FpySequencer_DebugBreakpointArgs(true, breakOnce, stmtIdx));

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command DEBUG_BREAK
//!
//! Pauses the execution of the sequencer once, just before it is about to dispatch the next statement,
//! until unpaused by the DEBUG_CONTINUE command. This command is only valid in the RUNNING state.
//! Debug settings are cleared after a sequence ends execution.
void FpySequencer::DEBUG_BREAK_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                          U32 cmdSeq,           //!< The command sequence number
                                          bool breakOnce  //!< Whether or not to break only once at this breakpoint
) {
    if (!this->isRunningState(this->sequencer_getState())) {
        // can only break while running
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }
    this->sequencer_sendSignal_cmd_DEBUG_BREAK(
        FpySequencer_DebugBreakpointArgs(true, breakOnce, this->m_runtime.nextStatementIndex));

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command DEBUG_CONTINUE
//!
//! Continues the execution of the sequence after it has been paused by a debug break. This command
//! is only valid in the RUNNING.DEBUG_BROKEN state.
void FpySequencer::DEBUG_CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                             U32 cmdSeq            //!< The command sequence number
) {
    if (this->sequencer_getState() != State::RUNNING_DEBUG_BROKEN) {
        this->log_WARNING_HI_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->sequencer_sendSignal_cmd_DEBUG_CONTINUE();

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for command DEBUG_CLEAR_BREAKPOINT
//!
//! Clears the debugging breakpoint, but does not continue executing the sequence. This command
//! is valid in all states. This happens automatically when a sequence ends execution.
void FpySequencer::DEBUG_CLEAR_BREAKPOINT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                                     U32 cmdSeq            //!< The command sequence number
) {
    this->sequencer_sendSignal_cmd_DEBUG_CLEAR_BREAKPOINT();
    this->log_ACTIVITY_HI_DebugBreakpointCleared();

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! Handler for input port checkTimers
void FpySequencer::checkTimers_handler(FwIndexType portNum,  //!< The port number
                                       U32 context           //!< The call order
) {
    this->sequencer_sendSignal_checkTimersIn();
}

void FpySequencer::pingIn_handler(FwIndexType portNum, /*!< The port number*/
                                  U32 key              /*!< Value to return to pinger*/
) {
    // send ping response
    this->pingOut_out(0, key);
}

//! Handler for input port cmdResponseIn
void FpySequencer::cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                                         FwOpcodeType opCode,             //!< Command Op Code
                                         U32 cmdSeq,                      //!< Command Sequence
                                         const Fw::CmdResponse& response  //!< The command response argument
) {
    // if we aren't in the RUNNING state:
    if (!this->isRunningState(sequencer_getState())) {
        // must be a coding error from an outside component (off nom), or due to CANCEL while running a command (nom).
        // because we can't be sure that it wasn't a nominal sequence of events leading to this, don't fail the
        // sequence, just report it
        this->log_WARNING_LO_CmdResponseWhileNotRunningSequence(static_cast<I32>(this->sequencer_getState()), opCode,
                                                                response);
        return;
    }

    // okay, we're running a sequence. now let's use the cmdUid to check if the response was for a cmd
    // from this sequence

    // the cmdSeq arg is confusingly not the cmdSeq in this case, according to the current implementation
    // of the CmdDisp. instead, it is the context that we passed in when we originally sent the cmd out.
    // this context is in turn the cmdUid that we calculated just before sending it. rename the variable for
    // clarity's sake
    U32 cmdUid = cmdSeq;

    // pull the sequence index (modulo 2^16) out of the cmdUid. see the comment in FpySequencer::dispatchCommand
    // for info on the binary format of this cmdUid. as a reminder, this should be equal to the first 16 bits of
    // the m_sequencesStarted variable
    U16 sequenceIndex = static_cast<U16>((cmdUid & 0xFFFF0000) >> 16);
    U16 currentSequenceIndex = static_cast<U16>(this->m_sequencesStarted & 0xFFFF);

    // if it was from a different sequence:
    if (sequenceIndex != currentSequenceIndex) {
        this->log_WARNING_LO_CmdResponseFromOldSequence(opCode, response, sequenceIndex, currentSequenceIndex);
        return;
    }

    // okay, it was from this sequence. now if anything's wrong from this point on we should fail the sequence

    // first, make sure we're actually awaiting a statement response
    if (this->sequencer_getState() != State::RUNNING_AWAITING_STATEMENT_RESPONSE) {
        // okay, crap. something from this sequence responded, and we weren't awaiting anything. end it all
        this->log_WARNING_HI_CmdResponseWhileNotAwaiting(opCode, response);
        this->sequencer_sendSignal_stmtResponse_unexpected();
        return;
    }

    if (this->m_runtime.currentStatementOpcode != Fpy::DirectiveId::CONST_CMD &&
        this->m_runtime.currentStatementOpcode != Fpy::DirectiveId::STACK_CMD) {
        // we were not awaiting a cmd response, we were waiting for a directive
        this->log_WARNING_HI_CmdResponseWhileAwaitingDirective(opCode, response,
                                                               this->m_runtime.currentStatementOpcode);
        this->sequencer_sendSignal_stmtResponse_unexpected();
        return;
    }

    // okay, we were awaiting a cmd response. were we awaiting this opcode?
    if (opCode != this->m_runtime.currentCmdOpcode) {
        // we were not awaiting this opcode. coding error, likely on the part of the responding component or cmd
        // dispatcher
        this->log_WARNING_HI_WrongCmdResponseOpcode(opCode, response, this->m_runtime.currentCmdOpcode);
        this->sequencer_sendSignal_stmtResponse_unexpected();
        return;
    }

    // okay, we were awaiting this opcode. but was it from this exact statement, or a different one with the same opcode
    // in the same file?

    // pull the cmd index (modulo 2^16) out of cmdUid. this should be equal to the first 16 bits of the
    // m_statementsDispatched variable
    U16 cmdIndex = static_cast<U16>(cmdUid & 0xFFFF);
    // check for coding errors. at this point in the function, we have definitely dispatched a stmt
    FW_ASSERT(this->m_statementsDispatched > 0);
    U16 currentCmdIndex = static_cast<U16>((this->m_statementsDispatched) & 0xFFFF);

    if (cmdIndex != currentCmdIndex) {
        // we were not awaiting this exact statement, it was a different one with the same opcode. coding error
        this->log_WARNING_HI_WrongCmdResponseIndex(opCode, response, cmdIndex, currentCmdIndex);
        this->sequencer_sendSignal_stmtResponse_unexpected();
        return;
    }

    // okay, got the right cmd back. we have verified:
    // 1) we are in the RUNNING state
    // 2) the response is from this sequence
    // 3) the response is from the correct opcode
    // 4) the response is from the correct instance of that opcode in the sequence

    if (response == Fw::CmdResponse::OK) {
        this->sequencer_sendSignal_stmtResponse_success();
    } else {
        this->log_WARNING_HI_CommandFailed(opCode, this->currentStatementIdx(), this->m_sequenceFilePath, response);
        this->sequencer_sendSignal_stmtResponse_failure();
    }
    // push the cmd response to the stack so we can branch off of it
    this->push(static_cast<I32>(response.e));
}

//! Handler for input port seqRunIn
void FpySequencer::seqRunIn_handler(FwIndexType portNum, const Fw::StringBase& filename) {
    // can only run a seq while in idle
    if (sequencer_getState() != State::IDLE) {
        this->log_WARNING_HI_InvalidSeqRunCall(static_cast<I32>(sequencer_getState()));
        return;
    }

    // seqRunIn is never blocking
    this->sequencer_sendSignal_cmd_RUN(FpySequencer_SequenceExecutionArgs(filename, FpySequencer_BlockState::NO_BLOCK));
}

//! Handler for input port tlmWrite
void FpySequencer::tlmWrite_handler(FwIndexType portNum,  //!< The port number
                                    U32 context           //!< The call order
) {
    this->tlmWrite_State(static_cast<I32>(this->sequencer_getState()));
    this->tlmWrite_StatementsDispatched(this->m_statementsDispatched);
    this->tlmWrite_StatementsFailed(this->m_tlm.statementsFailed);
    this->tlmWrite_SequencesCancelled(this->m_tlm.sequencesCancelled);
    this->tlmWrite_SequencesSucceeded(this->m_tlm.sequencesSucceeded);
    this->tlmWrite_SequencesFailed(this->m_tlm.sequencesFailed);
    this->tlmWrite_LastDirectiveError(this->m_tlm.lastDirectiveError);
    this->tlmWrite_SeqPath(this->m_sequenceFilePath);
    this->tlmWrite_DebugBreakpointIdx(this->m_debug.breakpointIndex);
    this->tlmWrite_Debug(this->getDebugTelemetry());
}

FpySequencer_DebugTelemetry FpySequencer::getDebugTelemetry() {
    // only send debug tlm when we are paused in debug break
    if (this->sequencer_getState() == State::RUNNING_DEBUG_BROKEN) {
        if (this->m_runtime.nextStatementIndex >= this->m_sequenceObj.get_header().get_statementCount()) {
            // reached end of file, turn on EOF flag and otherwise send some default tlm
            return FpySequencer_DebugTelemetry(true, false, 0, 0);
        }

        const Fpy::Statement& nextStmt = this->m_sequenceObj.get_statements()[this->m_runtime.nextStatementIndex];
        DirectiveUnion directiveUnion;
        Fw::Success status = this->deserializeDirective(nextStmt, directiveUnion);
        if (status != Fw::Success::SUCCESS) {
            return FpySequencer_DebugTelemetry(false, false, nextStmt.get_opCode(), 0);
        }
        if (nextStmt.get_opCode() == Fpy::DirectiveId::CONST_CMD) {
            // send opcode of the cmd to the ground
            return FpySequencer_DebugTelemetry(false, true, nextStmt.get_opCode(),
                                               directiveUnion.constCmd.get_opCode());
        }

        return FpySequencer_DebugTelemetry(false, true, nextStmt.get_opCode(), 0);
    }
    // send some default tlm when we aren't in debug break
    return FpySequencer_DebugTelemetry(false, false, 0, 0);
}

void FpySequencer::parametersLoaded() {
    Fw::ParamValid valid;
    this->paramGet_STATEMENT_TIMEOUT_SECS(valid);
    // check for coding errors--should have a default
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT);
}

void FpySequencer::parameterUpdated(FwPrmIdType id) {
    Fw::ParamValid valid;
    switch (id) {
        case PARAMID_STATEMENT_TIMEOUT_SECS: {
            this->tlmWrite_PRM_STATEMENT_TIMEOUT_SECS(this->paramGet_STATEMENT_TIMEOUT_SECS(valid));
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(id));  // coding error, forgot to include in switch statement
        }
    }
}

bool FpySequencer::isRunningState(State state) {
    // TODO ask Rob if there's a better way to check if we're in a superstate. I don't want to have
    // to update this every time I add a new substate to the RUNNING state.

    return this->sequencer_getState() == State::RUNNING_AWAITING_STATEMENT_RESPONSE ||
           this->sequencer_getState() == State::RUNNING_DISPATCH_STATEMENT ||
           this->sequencer_getState() == State::RUNNING_DEBUG_BROKEN ||
           this->sequencer_getState() == State::RUNNING_SLEEPING;
}

}  // namespace Svc
