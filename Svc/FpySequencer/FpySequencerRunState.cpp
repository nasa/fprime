#include <new>
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

Signal FpySequencer::dispatchStatement() {
    // check to make sure no array out of bounds, or if it is out of bounds it's only 1 out of bound
    // as that indicates eof
    FW_ASSERT(this->m_runtime.nextStatementIndex <= this->m_sequenceObj.getheader().getstatementCount());

    if (this->m_runtime.nextStatementIndex == this->m_sequenceObj.getheader().getstatementCount()) {
        return Signal::result_dispatchStatement_noMoreStatements;
    }

    const Fpy::Statement& nextStatement = this->m_sequenceObj.getstatements()[this->m_runtime.nextStatementIndex];
    this->m_runtime.nextStatementIndex++;
    this->m_runtime.currentStatementOpcode = nextStatement.getopCode();
    this->m_runtime.currentCmdOpcode = 0; // we haven't deserialized the directive yet, so we don't know if it's a cmd

    Fw::Success result;
    DirectiveUnion directiveUnion;

    result = this->deserializeDirective(nextStatement, directiveUnion);

    if (!result) {
        return Signal::result_dispatchStatement_failure;
    }

    if (this->m_runtime.currentStatementOpcode == Fpy::DirectiveId::CMD) {
        // update the opcode of the cmd we will await
        this->m_runtime.currentCmdOpcode = directiveUnion.cmd.getopCode();
    }

    this->dispatchDirective(directiveUnion,
                            Fpy::DirectiveId(static_cast<Fpy::DirectiveId::T>(nextStatement.getopCode())));
    this->m_runtime.currentStatementDispatchTime = getTime(); // set dispatch time right after we have successfully dispatched

    this->m_statementsDispatched++;

    return Signal::result_dispatchStatement_success;
}

// deserializes a directive from bytes into the Fpy type
// returns success if able to deserialize, and returns the Fpy type object
// as a reference, in a union of all the possible directive type objects
Fw::Success FpySequencer::deserializeDirective(const Fpy::Statement& stmt, DirectiveUnion& deserializedDirective) {
    Fw::SerializeStatus status;
    // make our own esb so we can deser from stmt without breaking its constness
    Fw::ExternalSerializeBuffer argBuf(const_cast<U8*>(stmt.getargBuf().getBuffAddr()),
                                       stmt.getargBuf().getBuffLength());
    argBuf.setBuffLen(stmt.getargBuf().getBuffLength());

    switch (stmt.getopCode()) {
        case Fpy::DirectiveId::WAIT_REL: {
            // in order to use a type with non trivial ctor in cpp union, have to manually construct and destruct it
            new (&deserializedDirective.waitRel) FpySequencer_WaitRelDirective();
            status = argBuf.deserialize(deserializedDirective.waitRel);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            new (&deserializedDirective.waitAbs) FpySequencer_WaitAbsDirective();
            status = argBuf.deserialize(deserializedDirective.waitAbs);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::SET_SER_REG: {
            new (&deserializedDirective.setSerReg) FpySequencer_SetSerRegDirective();
            // set local var has some custom deserialization behavior
            // we don't write a custom class for it though because that deserialization behavior only
            // applies for the initial time we deserialize it out of the statement

            // the behavior in question is that it will grab the entire remaining part of the statement
            // arg buf. that is, it uses the remaining length of the statement arg buf to determine the length
            // of its value buf. this way we get to save on serializing the value length

            // TODO do some trades on the best way to do this. not confident on this one

            // first deserialize the index
            U8 index;
            status = argBuf.deserialize(index);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            deserializedDirective.setSerReg.setindex(index);

            // okay, now deserialize the remaining bytes in the stmt arg buf into the value buf

            //  how many bytes are left?
            FwSizeType valueSize = argBuf.getBuffLeft();

            // check to make sure the value will fit in the FpySequencer_SetSerRegDirective::value buf
            if (valueSize > Fpy::MAX_SERIALIZABLE_REGISTER_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // okay, it will fit. put it in
            status = argBuf.deserialize(deserializedDirective.setSerReg.getvalue(), valueSize, Fw::Serialization::OMIT_LENGTH);

            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // now there should be nothing left, otherwise coding err
            FW_ASSERT(argBuf.getBuffLeft() == 0, static_cast<FwAssertArgType>(argBuf.getBuffLeft()));

            // and set the buf size now that we know it
            deserializedDirective.setSerReg.set_valueSize(valueSize);
            break;
        }
        case Fpy::DirectiveId::GOTO: {
            new (&deserializedDirective.gotoDirective) FpySequencer_GotoDirective();
            status = argBuf.deserialize(deserializedDirective.gotoDirective);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::IF: {
            new (&deserializedDirective.ifDirective) FpySequencer_IfDirective();
            status = argBuf.deserialize(deserializedDirective.ifDirective);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::NO_OP: {
            new (&deserializedDirective.noOp) FpySequencer_NoOpDirective();
            // no op does not need deser
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::GET_TLM: {
            new (&deserializedDirective.getTlm) FpySequencer_GetTlmDirective();
            status = argBuf.deserialize(deserializedDirective.getTlm);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::GET_PRM: {
            new (&deserializedDirective.getPrm) FpySequencer_GetPrmDirective();
            status = argBuf.deserialize(deserializedDirective.getPrm);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::CMD: {
            new (&deserializedDirective.cmd) FpySequencer_CmdDirective();
            // same deserialization behavior as SET_SER_REG

            // first deserialize the opcode
            FwOpcodeType opcode;
            status = argBuf.deserialize(opcode);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            deserializedDirective.cmd.setopCode(opcode);
            //  how many bytes are left?
            FwSizeType cmdArgBufSize = argBuf.getBuffLeft();

            // check to make sure the value will fit in the FpySequencer_CmdDirective::argBuf
            if (cmdArgBufSize > Fpy::MAX_SERIALIZABLE_REGISTER_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // okay, it will fit. put it in
            status = argBuf.deserialize(deserializedDirective.cmd.getargBuf(), cmdArgBufSize, Fw::Serialization::OMIT_LENGTH);

            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // now there should be nothing left, otherwise coding err
            FW_ASSERT(argBuf.getBuffLeft() == 0, static_cast<FwAssertArgType>(argBuf.getBuffLeft()));

            // and set the buf size now that we know it
            deserializedDirective.cmd.set_argBufSize(cmdArgBufSize);
            break;
        }
        // fallthrough on purpose
        case Fpy::DirectiveId::DESER_SER_REG_8:
        case Fpy::DirectiveId::DESER_SER_REG_4:
        case Fpy::DirectiveId::DESER_SER_REG_2:
        case Fpy::DirectiveId::DESER_SER_REG_1: {
            new (&deserializedDirective.deserSerReg) FpySequencer_DeserSerRegDirective();

            U8 deserSize;

            if (stmt.getopCode() == Fpy::DirectiveId::DESER_SER_REG_1) {
                deserSize = 1;
            } else if (stmt.getopCode() == Fpy::DirectiveId::DESER_SER_REG_2) {
                deserSize = 2;
            } else if (stmt.getopCode() == Fpy::DirectiveId::DESER_SER_REG_4) {
                deserSize = 4;
            } else if (stmt.getopCode() == Fpy::DirectiveId::DESER_SER_REG_8) {
                deserSize = 8;
            } else {
                FW_ASSERT(0, static_cast<FwAssertArgType>(stmt.getopCode()));
                return Fw::Success::FAILURE;
            }

            deserializedDirective.deserSerReg.set_deserSize(deserSize);

            U8 srcSerRegIdx;
            status = argBuf.deserialize(srcSerRegIdx);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.deserSerReg.setsrcSerRegIdx(srcSerRegIdx);

            FwSizeType srcOffset;
            status = argBuf.deserialize(srcOffset);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.deserSerReg.setsrcOffset(srcOffset);

            U8 destReg;
            status = argBuf.deserialize(destReg);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.deserSerReg.setdestReg(destReg);
            break;
        }
        case Fpy::DirectiveId::SET_REG: {
            new (&deserializedDirective.setReg) FpySequencer_SetRegDirective();
            status = argBuf.deserialize(deserializedDirective.setReg);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        // fallthrough on purpose
        case Fpy::DirectiveId::OR:
        case Fpy::DirectiveId::AND:
        case Fpy::DirectiveId::IEQ:
        case Fpy::DirectiveId::INE:
        case Fpy::DirectiveId::UGT:
        case Fpy::DirectiveId::ULT:
        case Fpy::DirectiveId::ULE:
        case Fpy::DirectiveId::UGE: 
        case Fpy::DirectiveId::SGT:
        case Fpy::DirectiveId::SLT:
        case Fpy::DirectiveId::SLE:
        case Fpy::DirectiveId::SGE: 
        case Fpy::DirectiveId::FEQ:
        case Fpy::DirectiveId::FNE:
        case Fpy::DirectiveId::FLT:
        case Fpy::DirectiveId::FLE:
        case Fpy::DirectiveId::FGT:
        case Fpy::DirectiveId::FGE:
        {
            new (&deserializedDirective.binaryCmp) FpySequencer_BinaryCmpDirective();
            
            U8 lhs;
            status = argBuf.deserialize(lhs);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.binaryCmp.setlhs(lhs);
            U8 rhs;
            status = argBuf.deserialize(rhs);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.binaryCmp.setrhs(rhs);
            U8 res;
            status = argBuf.deserialize(res);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.binaryCmp.setres(res);

            deserializedDirective.binaryCmp.set_op(stmt.getopCode());
            break;
        }
        case Fpy::DirectiveId::NOT: {
            new (&deserializedDirective.notDirective) FpySequencer_NotDirective();
            status = argBuf.deserialize(deserializedDirective.notDirective);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::EXIT: {
            new (&deserializedDirective.exit) FpySequencer_ExitDirective();
            status = argBuf.deserialize(deserializedDirective.exit);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        default: {
            // unsure what this opcode is. check compiler version matches sequencer
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                           this->m_sequenceFilePath);
            return Fw::Success::FAILURE;
        }
    }
    return Fw::Success::SUCCESS;
}

// dispatches a deserialized sequencer directive to the right handler.
void FpySequencer::dispatchDirective(const DirectiveUnion& directive, const Fpy::DirectiveId& id) {
    switch (id) {
        case Fpy::DirectiveId::INVALID: {
            // coding err
            FW_ASSERT(0);
            return;
        }
        case Fpy::DirectiveId::WAIT_REL: {
            this->directive_waitRel_internalInterfaceInvoke(directive.waitRel);
            return;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            this->directive_waitAbs_internalInterfaceInvoke(directive.waitAbs);
            return;
        }
        case Fpy::DirectiveId::SET_SER_REG: {
            this->directive_setSerReg_internalInterfaceInvoke(directive.setSerReg);
            return;
        }
        case Fpy::DirectiveId::GOTO: {
            this->directive_goto_internalInterfaceInvoke(directive.gotoDirective);
            return;
        }
        case Fpy::DirectiveId::IF: {
            this->directive_if_internalInterfaceInvoke(directive.ifDirective);
            return;
        }
        case Fpy::DirectiveId::NO_OP: {
            this->directive_noOp_internalInterfaceInvoke(directive.noOp);
            return;
        }
        case Fpy::DirectiveId::GET_TLM: {
            this->directive_getTlm_internalInterfaceInvoke(directive.getTlm);
            return;
        }
        case Fpy::DirectiveId::GET_PRM: {
            this->directive_getPrm_internalInterfaceInvoke(directive.getPrm);
            return;
        }
        case Fpy::DirectiveId::CMD: {
            this->directive_cmd_internalInterfaceInvoke(directive.cmd);
            return;
        }
            // fallthrough on purpose
        case Fpy::DirectiveId::DESER_SER_REG_8:
        case Fpy::DirectiveId::DESER_SER_REG_4:
        case Fpy::DirectiveId::DESER_SER_REG_2:
        case Fpy::DirectiveId::DESER_SER_REG_1: {
            this->directive_deserSerReg_internalInterfaceInvoke(directive.deserSerReg);
            return;
        }
        case Fpy::DirectiveId::SET_REG: {
            this->directive_setReg_internalInterfaceInvoke(directive.setReg);
            return;
        }
            // fallthrough on purpose
        case Fpy::DirectiveId::OR:
        case Fpy::DirectiveId::AND:
        case Fpy::DirectiveId::IEQ:
        case Fpy::DirectiveId::INE:
        case Fpy::DirectiveId::UGT:
        case Fpy::DirectiveId::ULT:
        case Fpy::DirectiveId::ULE:
        case Fpy::DirectiveId::UGE: 
        case Fpy::DirectiveId::SGT:
        case Fpy::DirectiveId::SLT:
        case Fpy::DirectiveId::SLE:
        case Fpy::DirectiveId::SGE:
        case Fpy::DirectiveId::FEQ:
        case Fpy::DirectiveId::FNE:
        case Fpy::DirectiveId::FLT:
        case Fpy::DirectiveId::FLE:
        case Fpy::DirectiveId::FGT:
        case Fpy::DirectiveId::FGE: {
            this->directive_binaryCmp_internalInterfaceInvoke(directive.binaryCmp);
            return;
        }
        case Fpy::DirectiveId::NOT: {
            this->directive_not_internalInterfaceInvoke(directive.notDirective);
            return;
        }
        case Fpy::DirectiveId::EXIT: {
            this->directive_exit_internalInterfaceInvoke(directive.exit);
            return;
        }
    }
    // coding err
    FW_ASSERT(0, static_cast<FwAssertArgType>(id));
}

Signal FpySequencer::checkShouldWake() {
    Fw::Time currentTime = this->getTime();

#ifdef FW_USE_TIME_BASE
    if (currentTime.getTimeBase() != this->m_runtime.wakeupTime.getTimeBase()) {
        // cannot compare these times.
        this->log_WARNING_HI_MismatchedTimeBase(currentTime.getTimeBase(), this->m_runtime.wakeupTime.getTimeBase());

        return Signal::result_timeOpFailed;
    }
#endif

#ifdef FW_USE_TIME_CONTEXT
    if (currentTime.getContext() != this->m_runtime.wakeupTime.getContext()) {
        // cannot compare these times.
        this->log_WARNING_HI_MismatchedTimeContext(currentTime.getContext(), this->m_runtime.wakeupTime.getContext());

        return Signal::result_timeOpFailed;
    }
#endif

    if (currentTime < this->m_runtime.wakeupTime) {
        // not time to wake up!
        return Signal::result_checkShouldWake_keepSleeping;
    }

    // say we've finished our sleep
    return Signal::result_checkShouldWake_wakeup;
}

// checks whether the currently executing statement timed out
Signal FpySequencer::checkStatementTimeout() {
    Fw::ParamValid valid;
    F32 timeout = this->paramGet_STATEMENT_TIMEOUT_SECS(valid);
    if (timeout <= 0 || timeout > static_cast<F32>(std::numeric_limits<U32>::max())) {
        // no timeout
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    Fw::Time currentTime = getTime();

#ifdef FW_USE_TIME_BASE
    if (currentTime.getTimeBase() != this->m_runtime.currentStatementDispatchTime.getTimeBase()) {
        // can't compare time base. must have changed
        this->log_WARNING_HI_MismatchedTimeBase(currentTime.getTimeBase(),
                                                this->m_runtime.currentStatementDispatchTime.getTimeBase());
        return Signal::result_timeOpFailed;
    }
#endif
#ifdef FW_USE_TIME_CONTEXT
    if (currentTime.getContext() != this->m_runtime.currentStatementDispatchTime.getContext()) {
        // can't compare time ctx. must have changed
        this->log_WARNING_HI_MismatchedTimeContext(currentTime.getContext(),
                                                   this->m_runtime.currentStatementDispatchTime.getContext());
        return Signal::result_timeOpFailed;
    }
#endif

    if (this->m_runtime.currentStatementDispatchTime.getSeconds() > currentTime.getSeconds()) {
        // somehow we've gone back in time... just ignore it and move on. should get fixed
        // if we wait I guess
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    if (this->m_runtime.currentStatementDispatchTime.getSeconds() == currentTime.getSeconds() &&
        this->m_runtime.currentStatementDispatchTime.getUSeconds() > currentTime.getUSeconds()) {
        // same as above
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    U64 currentUSeconds = currentTime.getSeconds() * 1000000 + currentTime.getUSeconds();
    U64 dispatchUSeconds = this->m_runtime.currentStatementDispatchTime.getSeconds() * 1000000 +
                           this->m_runtime.currentStatementDispatchTime.getUSeconds();

    U64 timeoutUSeconds = static_cast<U64>(timeout * 1000000.0f);

    if (currentUSeconds - dispatchUSeconds < timeoutUSeconds) {
        // not over timeout
        return Signal::result_checkStatementTimeout_noTimeout;
    }

    // we timed out
    if (this->m_runtime.currentStatementOpcode == Fpy::DirectiveId::CMD) {
        // if we were executing a command, warn that the cmd timed out with its opcode
        this->log_WARNING_HI_CommandTimedOut(this->m_runtime.currentCmdOpcode,
                                             this->m_runtime.nextStatementIndex - 1, this->m_sequenceFilePath);
    } else {
        this->log_WARNING_HI_DirectiveTimedOut(this->m_runtime.currentStatementOpcode,
                                               this->m_runtime.nextStatementIndex - 1, this->m_sequenceFilePath);
    }

    return Signal::result_checkStatementTimeout_statementTimeout;
}

}  // namespace Svc
