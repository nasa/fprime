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

FpySequencer ::FpySequencer(const char* const compName) : FpySequencerComponentBase(compName) {}

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
    if (sequencer_getState() != FpySequencer_SequencerStateMachineStateMachineBase::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(sequencer_getState()));
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
    if (sequencer_getState() != FpySequencer_SequencerStateMachineStateMachineBase::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(sequencer_getState()));
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
    if (sequencer_getState() != FpySequencer_SequencerStateMachineStateMachineBase::State::AWAITING_CMD_RUN_VALIDATED) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (block == FpySequencer_BlockState::BLOCK) {
        // save the opCode and cmdSeq so we can respond later
        this->m_savedOpCode = opCode;
        this->m_savedCmdSeq = cmdSeq;
    }

    this->sequencer_sendSignal_cmd_RUN_VALIDATED(FpySequencer_SequenceExecutionArgs(m_sequenceFilePath, block));

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
    if (sequencer_getState() == FpySequencer_SequencerStateMachineStateMachineBase::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->sequencer_sendSignal_cmd_CANCEL();

    // cancel returns immediately and always succeeds
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
    if (this->sequencer_getState() !=
        FpySequencer_SequencerStateMachineStateMachineBase::State::RUNNING_AWAITING_STATEMENT_RESPONSE) {
        this->log_WARNING_LO_UnexpectedStatementResponseForState(static_cast<I32>(this->sequencer_getState()), opCode,
                                                                 response);
        // ignore it, hopefully that wasn't important :D
        return;
    }
    if (opCode != this->m_runtime.currentStatementOpcode) {
        this->log_WARNING_LO_WrongStatementResponseOpcode(this->m_runtime.currentStatementOpcode, opCode, response);
        // uh oh... we're awaiting a cmd but got the wrong one back...
        // not much we can do but keep waiting
        return;
    }

    // okay, got the right cmd back
    if (response == Fw::CmdResponse::OK) {
        this->sequencer_sendSignal_stmtResponse_success();
    } else {
        this->sequencer_sendSignal_stmtResponse_failure();
    }
}

//! Handler for input port tlmWrite
void FpySequencer::tlmWrite_handler(FwIndexType portNum,  //!< The port number
                                    U32 context           //!< The call order
) {
    this->tlmWrite_StatementsDispatched(this->m_tlm.statementsDispatched);
    this->tlmWrite_StatementsFailed(this->m_tlm.statementsFailed);
    this->tlmWrite_SequencesSucceeded(this->m_tlm.sequencesSucceeded);
    this->tlmWrite_SequencesFailed(this->m_tlm.sequencesFailed);
    this->tlmWrite_SeqPath(this->m_sequenceFilePath);
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
            this->tlmWrite_STATEMENT_TIMEOUT_SECS(this->paramGet_STATEMENT_TIMEOUT_SECS(valid));
            break;
        }
        default: {
            FW_ASSERT(0, id);  // coding error, forgot to include in switch statement
        }
    }
}

}  // namespace Svc
