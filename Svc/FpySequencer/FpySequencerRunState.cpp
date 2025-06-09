#include <new>
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Time/Time.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
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
    this->m_runtime.currentStatementType = nextStatement.gettype();
    this->m_runtime.currentStatementDispatchTime = getTime();

    Fw::Success result;

    // based on the statement type (directive or cmd)
    // send it to where it needs to go
    if (nextStatement.gettype() == Fpy::StatementType::DIRECTIVE) {
        // directives need to be deserialized first before dispatching
        DirectiveUnion directiveUnion;
        result = this->deserializeDirective(nextStatement, directiveUnion);
        if (result) {
            this->dispatchDirective(directiveUnion,
                                    Fpy::DirectiveId(static_cast<Fpy::DirectiveId::T>(nextStatement.getopCode())));
        }
    } else {
        result = this->dispatchCommand(nextStatement);
    }

    this->m_statementsDispatched++;

    if (result == Fw::Success::SUCCESS) {
        return Signal::result_dispatchStatement_success;
    } else {
        return Signal::result_dispatchStatement_failure;
    }
}

// dispatches a command out via port.
// return true if successfully dispatched.
Fw::Success FpySequencer::dispatchCommand(const Fpy::Statement& stmt) {
    FW_ASSERT(stmt.gettype() == Fpy::StatementType::COMMAND);
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_COMMAND));
    // TODO should I assert here? this really shouldn't fail, I should just add a static assert
    // on com buf size and then assert here
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_CommandSerializeError(stmt.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   sizeof(Fw::ComPacket::FW_PACKET_COMMAND), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Fw::Success::FAILURE;
    }
    // TODO same as above
    stat = cmdBuf.serialize(stmt.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_CommandSerializeError(stmt.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   sizeof(stmt.getopCode()), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serialize(stmt.getargBuf().getBuffAddr(), stmt.getargBuf().getBuffLength(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_CommandSerializeError(stmt.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   stmt.getargBuf().getBuffLength(), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Fw::Success::FAILURE;
    }

    // calculate the unique command identifier:
    // cmd UID is formatted like XXYY, where XX are the first two bytes of the m_sequencesStarted counter
    // and YY are the first two bytes of the m_statementsDispatched counter.
    // this way, we know when we get a cmd back A) whether or not it's from this sequence (modulo 2^16) and B)
    // whether or not it's this specific instance of the cmd in the sequence, and not another one with the same opcode
    // somewhere else in the file.
    // if we put this uid in the context we send to the cmdDisp, we will get it back when the cmd returns
    U32 cmdUid =
        static_cast<U32>(((this->m_sequencesStarted & 0xFFFF) << 16) | (this->m_statementsDispatched & 0xFFFF));

    // little note--theoretically this could produce a cmdResponse before we send the
    // dispatchSuccess signal. however b/c of priorities the dispatchSuccess signal will
    // always get processed first, leaving us in the right state for the cmdresponse
    this->cmdOut_out(0, cmdBuf, cmdUid);

    return Fw::Success::SUCCESS;
}

// deserializes a directive from bytes into the Fpy type
// returns success if able to deserialize, and returns the Fpy type object
// as a reference, in a union of all the possible directive type objects
Fw::Success FpySequencer::deserializeDirective(const Fpy::Statement& stmt, DirectiveUnion& deserializedDirective) {
    FW_ASSERT(stmt.gettype() == Fpy::StatementType::DIRECTIVE);
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
        case Fpy::DirectiveId::SET_LVAR: {
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

            new (&deserializedDirective.setLVar) FpySequencer_SetLocalVarDirective();
            deserializedDirective.setLVar.setindex(index);

            // okay, now deserialize the remaining bytes in the stmt arg buf into the value buf

            //  how many bytes are left?
            FwSizeType valueSize = argBuf.getBuffLeft();

            // check to make sure the value will fit in the FpySequencer_SetLocalVarDirective::value buf
            if (valueSize > Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR,
                                                               argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // okay, it will fit. put it in
            status = argBuf.deserialize(deserializedDirective.setLVar.getvalue(), valueSize, true);

            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->log_WARNING_HI_DirectiveDeserializeError(stmt.getopCode(), this->m_runtime.nextStatementIndex - 1,
                                                               status, argBuf.getBuffLeft(), argBuf.getBuffLength());
                return Fw::Success::FAILURE;
            }

            // now there should be nothing left, otherwise coding err
            FW_ASSERT(argBuf.getBuffLeft() == 0, static_cast<FwAssertArgType>(argBuf.getBuffLeft()));

            // and set the buf size now that we know it
            deserializedDirective.setLVar.set_valueSize(valueSize);
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
// return success if successfully handled.
void FpySequencer::dispatchDirective(const DirectiveUnion& directive, const Fpy::DirectiveId& id) {
    switch (id) {
        case Fpy::DirectiveId::WAIT_REL: {
            this->directive_waitRel_internalInterfaceInvoke(directive.waitRel);
            break;
        }
        case Fpy::DirectiveId::WAIT_ABS: {
            this->directive_waitAbs_internalInterfaceInvoke(directive.waitAbs);
            break;
        }
        case Fpy::DirectiveId::SET_LVAR: {
            this->directive_setLocalVar_internalInterfaceInvoke(directive.setLVar);
            break;
        }
        case Fpy::DirectiveId::GOTO: {
            this->directive_goto_internalInterfaceInvoke(directive.gotoDirective);
            break;
        }
        case Fpy::DirectiveId::IF: {
            this->directive_if_internalInterfaceInvoke(directive.ifDirective);
            break;
        }
        case Fpy::DirectiveId::NO_OP: {
            this->directive_noOp_internalInterfaceInvoke(directive.noOp);
            break;
        }
        case Fpy::DirectiveId::GET_TLM: {
            this->directive_getTlm_internalInterfaceInvoke(directive.getTlm);
            break;
        }
        case Fpy::DirectiveId::GET_PRM: {
            this->directive_getPrm_internalInterfaceInvoke(directive.getPrm);
            break;
        }
        default: {
            FW_ASSERT(0, id);  // coding error, forgot to add switch case/port for this directive
        }
    }
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

    this->log_WARNING_HI_StatementTimedOut(this->m_runtime.currentStatementType, this->m_runtime.currentStatementOpcode,
                                           this->m_runtime.nextStatementIndex - 1, this->m_sequenceFilePath);
    return Signal::result_checkStatementTimeout_statementTimeout;
}

}  // namespace Svc
