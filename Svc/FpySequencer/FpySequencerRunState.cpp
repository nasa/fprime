#include <new>
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
namespace Svc {

// returns the index of the current statement
U32 FpySequencer::currentStatementIdx() {
    if (this->m_runtime.nextStatementIndex == 0) {
        // haven't started executing the sequence yet
        return 0;
    }
    return this->m_runtime.nextStatementIndex - 1;
}

Signal FpySequencer::dispatchStatement() {
    // check to make sure no array out of bounds, or if it is out of bounds it's only 1 out of bound
    // as that indicates eof
    FW_ASSERT(this->m_runtime.nextStatementIndex <= this->m_sequenceObj.get_header().get_statementCount());

    if (this->m_runtime.nextStatementIndex == this->m_sequenceObj.get_header().get_statementCount()) {
        return Signal::result_dispatchStatement_noMoreStatements;
    }

    const Fpy::Statement& nextStatement = this->m_sequenceObj.get_statements()[this->m_runtime.nextStatementIndex];
    this->m_runtime.nextStatementIndex++;
    this->m_runtime.currentStatementOpcode = nextStatement.get_opCode();
    this->m_runtime.currentCmdOpcode = 0;  // we haven't deserialized the directive yet, so we don't know if it's a cmd

    Fw::Success result;
    DirectiveUnion directiveUnion;

    result = this->deserializeDirective(nextStatement, directiveUnion);

    if (!result) {
        return Signal::result_dispatchStatement_failure;
    }

    if (this->m_runtime.currentStatementOpcode == Fpy::DirectiveId::CONST_CMD) {
        // update the opcode of the cmd we will await
        this->m_runtime.currentCmdOpcode = directiveUnion.constCmd.get_opCode();
    }

    this->dispatchDirective(directiveUnion,
                            Fpy::DirectiveId(static_cast<Fpy::DirectiveId::T>(nextStatement.get_opCode())));
    this->m_runtime.currentStatementDispatchTime =
        getTime();  // set dispatch time right after we have successfully dispatched

    this->m_statementsDispatched++;

    return Signal::result_dispatchStatement_success;
}

// deserializes a directive from bytes into the Fpy type
// returns success if able to deserialize, and returns the Fpy type object
// as a reference, in a union of all the possible directive type objects
Fw::Success FpySequencer::deserializeDirective(const Fpy::Statement& stmt, DirectiveUnion& deserializedDirective) {
    Fw::SerializeStatus status;
    // make our own esb so we can deser from stmt without breaking its constness
    Fw::ExternalSerializeBuffer argBuf(const_cast<U8*>(stmt.get_argBuf().getBuffAddr()),
                                       stmt.get_argBuf().getBuffLength());
    argBuf.setBuffLen(stmt.get_argBuf().getBuffLength());

    switch (stmt.get_opCode()) {
        case Fpy::DirectiveId::WAIT_REL: {
            // in order to use a type with non trivial ctor in cpp union, have to manually construct and destruct it
            new (&deserializedDirective.waitRel) FpySequencer_WaitRelDirective();
            // wait rel does not need deser
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            new (&deserializedDirective.waitAbs) FpySequencer_WaitAbsDirective();
            // wait abs does not need deser
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::GOTO: {
            new (&deserializedDirective.gotoDirective) FpySequencer_GotoDirective();
            status = argBuf.deserialize(deserializedDirective.gotoDirective);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::IF: {
            new (&deserializedDirective.ifDirective) FpySequencer_IfDirective();
            status = argBuf.deserialize(deserializedDirective.ifDirective);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::NO_OP: {
            new (&deserializedDirective.noOp) FpySequencer_NoOpDirective();
            // no op does not need deser
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::STORE_TLM_VAL: {
            new (&deserializedDirective.storeTlmVal) FpySequencer_StoreTlmValDirective();
            status = argBuf.deserialize(deserializedDirective.storeTlmVal);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::STORE_PRM: {
            new (&deserializedDirective.storePrm) FpySequencer_StorePrmDirective();
            status = argBuf.deserialize(deserializedDirective.storePrm);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::CONST_CMD: {
            new (&deserializedDirective.constCmd) FpySequencer_ConstCmdDirective();

            // first deserialize the opcode
            FwOpcodeType opcode;
            status = argBuf.deserialize(opcode);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            deserializedDirective.constCmd.set_opCode(opcode);
            //  how many bytes are left?
            FwSizeType cmdArgBufSize = argBuf.getBuffLeft();

            // check to make sure the value will fit in the FpySequencer_ConstCmdDirective::argBuf
            if (cmdArgBufSize > Fpy::MAX_DIRECTIVE_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // okay, it will fit. put it in
            status = argBuf.deserialize(deserializedDirective.constCmd.get_argBuf(), cmdArgBufSize,
                                        Fw::Serialization::OMIT_LENGTH);

            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // now there should be nothing left, otherwise coding err
            FW_ASSERT(argBuf.getBuffLeft() == 0, static_cast<FwAssertArgType>(argBuf.getBuffLeft()));

            // and set the buf size now that we know it
            deserializedDirective.constCmd.set__argBufSize(cmdArgBufSize);
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
        case Fpy::DirectiveId::NOT:
        case Fpy::DirectiveId::FPEXT:
        case Fpy::DirectiveId::FPTRUNC:
        case Fpy::DirectiveId::FPTOSI:
        case Fpy::DirectiveId::FPTOUI:
        case Fpy::DirectiveId::SITOFP:
        case Fpy::DirectiveId::UITOFP:
        case Fpy::DirectiveId::IADD:
        case Fpy::DirectiveId::ISUB:
        case Fpy::DirectiveId::IMUL:
        case Fpy::DirectiveId::UDIV:
        case Fpy::DirectiveId::SDIV:
        case Fpy::DirectiveId::UMOD:
        case Fpy::DirectiveId::SMOD:
        case Fpy::DirectiveId::FADD:
        case Fpy::DirectiveId::FSUB:
        case Fpy::DirectiveId::FMUL:
        case Fpy::DirectiveId::FDIV:
        case Fpy::DirectiveId::FLOAT_FLOOR_DIV:
        case Fpy::DirectiveId::FPOW:
        case Fpy::DirectiveId::FLOG:
        case Fpy::DirectiveId::FMOD:
        case Fpy::DirectiveId::SIEXT_8_64:
        case Fpy::DirectiveId::SIEXT_16_64:
        case Fpy::DirectiveId::SIEXT_32_64:
        case Fpy::DirectiveId::ZIEXT_8_64:
        case Fpy::DirectiveId::ZIEXT_16_64:
        case Fpy::DirectiveId::ZIEXT_32_64:
        case Fpy::DirectiveId::ITRUNC_64_8:
        case Fpy::DirectiveId::ITRUNC_64_16:
        case Fpy::DirectiveId::ITRUNC_64_32: {
            new (&deserializedDirective.stackOp) FpySequencer_StackOpDirective();
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            deserializedDirective.stackOp.set__op(stmt.get_opCode());
            break;
        }
        case Fpy::DirectiveId::EXIT: {
            new (&deserializedDirective.exit) FpySequencer_ExitDirective();
            if (argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_SIZE_MISMATCH,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::ALLOCATE: {
            new (&deserializedDirective.allocate) FpySequencer_AllocateDirective();
            status = argBuf.deserialize(deserializedDirective.allocate);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::STORE: {
            new (&deserializedDirective.store) FpySequencer_StoreDirective();
            status = argBuf.deserialize(deserializedDirective.store);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::LOAD: {
            new (&deserializedDirective.load) FpySequencer_LoadDirective();
            status = argBuf.deserialize(deserializedDirective.load);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::PUSH_VAL: {
            new (&deserializedDirective.pushVal) FpySequencer_PushValDirective();

            // how many bytes are left?
            FwSizeType bufSize = argBuf.getBuffLeft();

            // check to make sure the value will fit in the FpySequencer_PushValDirective::val buf
            if (bufSize > Fpy::MAX_DIRECTIVE_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(),
                                                               Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // okay, it will fit. put it in
            status =
                argBuf.deserialize(deserializedDirective.pushVal.get_val(), bufSize, Fw::Serialization::OMIT_LENGTH);

            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // now there should be nothing left, otherwise coding err
            FW_ASSERT(argBuf.getBuffLeft() == 0, static_cast<FwAssertArgType>(argBuf.getBuffLeft()));

            // and set the buf size now that we know it
            deserializedDirective.pushVal.set__valSize(bufSize);
            break;
        }
        case Fpy::DirectiveId::DISCARD: {
            new (&deserializedDirective.discard) FpySequencer_DiscardDirective();
            status = argBuf.deserialize(deserializedDirective.discard);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::MEMCMP: {
            new (&deserializedDirective.memCmp) FpySequencer_MemCmpDirective();
            status = argBuf.deserialize(deserializedDirective.memCmp);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        case Fpy::DirectiveId::STACK_CMD: {
            new (&deserializedDirective.stackCmd) FpySequencer_StackCmdDirective();
            status = argBuf.deserialize(deserializedDirective.stackCmd);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK || argBuf.getBuffLeft() != 0) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.get_opCode(), this->currentStatementIdx(), status,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }
            break;
        }
        default: {
            // unsure what this opcode is. check compiler version matches sequencer
            this->log_WARNING_HI_UnknownSequencerDirective(stmt.get_opCode(), this->currentStatementIdx(),
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
        case Fpy::DirectiveId::STORE_TLM_VAL: {
            this->directive_storeTlmVal_internalInterfaceInvoke(directive.storeTlmVal);
            return;
        }
        case Fpy::DirectiveId::STORE_PRM: {
            this->directive_storePrm_internalInterfaceInvoke(directive.storePrm);
            return;
        }
        case Fpy::DirectiveId::CONST_CMD: {
            this->directive_constCmd_internalInterfaceInvoke(directive.constCmd);
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
        case Fpy::DirectiveId::FGE:
        case Fpy::DirectiveId::NOT:
        case Fpy::DirectiveId::FPEXT:
        case Fpy::DirectiveId::FPTRUNC:
        case Fpy::DirectiveId::FPTOSI:
        case Fpy::DirectiveId::FPTOUI:
        case Fpy::DirectiveId::SITOFP:
        case Fpy::DirectiveId::UITOFP:
        case Fpy::DirectiveId::IADD:
        case Fpy::DirectiveId::ISUB:
        case Fpy::DirectiveId::IMUL:
        case Fpy::DirectiveId::UDIV:
        case Fpy::DirectiveId::SDIV:
        case Fpy::DirectiveId::UMOD:
        case Fpy::DirectiveId::SMOD:
        case Fpy::DirectiveId::FADD:
        case Fpy::DirectiveId::FSUB:
        case Fpy::DirectiveId::FMUL:
        case Fpy::DirectiveId::FDIV:
        case Fpy::DirectiveId::FLOAT_FLOOR_DIV:
        case Fpy::DirectiveId::FPOW:
        case Fpy::DirectiveId::FLOG:
        case Fpy::DirectiveId::FMOD:
        case Fpy::DirectiveId::SIEXT_8_64:
        case Fpy::DirectiveId::SIEXT_16_64:
        case Fpy::DirectiveId::SIEXT_32_64:
        case Fpy::DirectiveId::ZIEXT_8_64:
        case Fpy::DirectiveId::ZIEXT_16_64:
        case Fpy::DirectiveId::ZIEXT_32_64:
        case Fpy::DirectiveId::ITRUNC_64_8:
        case Fpy::DirectiveId::ITRUNC_64_16:
        case Fpy::DirectiveId::ITRUNC_64_32: {
            this->directive_stackOp_internalInterfaceInvoke(directive.stackOp);
            return;
        }
        case Fpy::DirectiveId::EXIT: {
            this->directive_exit_internalInterfaceInvoke(directive.exit);
            return;
        }
        case Fpy::DirectiveId::ALLOCATE: {
            this->directive_allocate_internalInterfaceInvoke(directive.allocate);
            return;
        }
        case Fpy::DirectiveId::STORE: {
            this->directive_store_internalInterfaceInvoke(directive.store);
            return;
        }
        case Fpy::DirectiveId::LOAD: {
            this->directive_load_internalInterfaceInvoke(directive.load);
            return;
        }
        case Fpy::DirectiveId::PUSH_VAL: {
            this->directive_pushVal_internalInterfaceInvoke(directive.pushVal);
            return;
        }
        case Fpy::DirectiveId::DISCARD: {
            this->directive_discard_internalInterfaceInvoke(directive.discard);
            return;
        }
        case Fpy::DirectiveId::MEMCMP: {
            this->directive_memCmp_internalInterfaceInvoke(directive.memCmp);
            return;
        }
        case Fpy::DirectiveId::STACK_CMD: {
            this->directive_stackCmd_internalInterfaceInvoke(directive.stackCmd);
            return;
        }
    }
    // coding err
    FW_ASSERT(0, static_cast<FwAssertArgType>(id));
}

Signal FpySequencer::checkShouldWake() {
    Fw::Time currentTime = this->getTime();

    if (currentTime.getTimeBase() != this->m_runtime.wakeupTime.getTimeBase()) {
        // cannot compare these times.
        this->log_WARNING_HI_MismatchedTimeBase(currentTime.getTimeBase(), this->m_runtime.wakeupTime.getTimeBase());

        return Signal::result_timeOpFailed;
    }

    // Do not compare time context

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

    if (currentTime.getTimeBase() != this->m_runtime.currentStatementDispatchTime.getTimeBase()) {
        // can't compare time base. must have changed
        this->log_WARNING_HI_MismatchedTimeBase(currentTime.getTimeBase(),
                                                this->m_runtime.currentStatementDispatchTime.getTimeBase());
        return Signal::result_timeOpFailed;
    }

    // Do not compare time context

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
    if (this->m_runtime.currentStatementOpcode == Fpy::DirectiveId::CONST_CMD ||
        this->m_runtime.currentStatementOpcode == Fpy::DirectiveId::STACK_CMD) {
        // if we were executing a command, warn that the cmd timed out with its opcode
        this->log_WARNING_HI_CommandTimedOut(this->m_runtime.currentCmdOpcode, this->currentStatementIdx(),
                                             this->m_sequenceFilePath);
    } else {
        this->log_WARNING_HI_DirectiveTimedOut(this->m_runtime.currentStatementOpcode, this->currentStatementIdx(),
                                               this->m_sequenceFilePath);
    }

    return Signal::result_checkStatementTimeout_statementTimeout;
}

}  // namespace Svc
