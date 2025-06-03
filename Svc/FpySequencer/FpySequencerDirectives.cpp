#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Fw/Com/ComPacket.hpp"

namespace Svc {

void FpySequencer::sendSignal(Signal signal) {
    switch (signal) {
        case Signal::stmtResponse_beginSleep: {
            this->sequencer_sendSignal_stmtResponse_beginSleep();
            break;
        }
        case Signal::stmtResponse_success: {
            this->sequencer_sendSignal_stmtResponse_success();
            break;
        }
        case Signal::stmtResponse_failure: {
            this->sequencer_sendSignal_stmtResponse_failure();
            break;
        }
        case Signal::stmtResponse_keepWaiting: {
            this->sequencer_sendSignal_stmtResponse_keepWaiting();
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(signal));
        }
    }
}

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const FpySequencer_WaitRelDirective& directive) {
    this->sendSignal(this->waitRel_directiveHandler(directive));
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const FpySequencer_WaitAbsDirective& directive) {
    this->sendSignal(this->waitAbs_directiveHandler(directive));
}

//! Internal interface handler for directive_setLocalVar
void FpySequencer::directive_setLocalVar_internalInterfaceHandler(
    const Svc::FpySequencer_SetLocalVarDirective& directive) {
    this->sendSignal(this->setLocalVar_directiveHandler(directive));
}

//! Internal interface handler for directive_goto
void FpySequencer::directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) {
    this->sendSignal(this->goto_directiveHandler(directive));
}

//! Internal interface handler for directive_if
void FpySequencer::directive_if_internalInterfaceHandler(const Svc::FpySequencer_IfDirective& directive) {
    this->sendSignal(this->if_directiveHandler(directive));
}

//! Internal interface handler for directive_noOp
void FpySequencer::directive_noOp_internalInterfaceHandler(const Svc::FpySequencer_NoOpDirective& directive) {
    this->sendSignal(this->noOp_directiveHandler(directive));
}

//! Internal interface handler for directive_getTlm
void FpySequencer::directive_getTlm_internalInterfaceHandler(const Svc::FpySequencer_GetTlmDirective& directive) {
    this->sendSignal(this->getTlm_directiveHandler(directive));
}

//! Internal interface handler for directive_getPrm
void FpySequencer::directive_getPrm_internalInterfaceHandler(const Svc::FpySequencer_GetPrmDirective& directive) {
    this->sendSignal(this->getPrm_directiveHandler(directive));
}

//! Internal interface handler for directive_cmd
void FpySequencer::directive_cmd_internalInterfaceHandler(const Svc::FpySequencer_CmdDirective& directive) {
    this->sendSignal(this->cmd_directiveHandler(directive));
}

//! Internal interface handler for directive_waitRel
Signal FpySequencer::waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive) {
    Fw::Time wakeupTime = this->getTime();

    wakeupTime.add(directive.getduration().getSeconds(), directive.getduration().getUSeconds());
    this->m_runtime.wakeupTime = wakeupTime;
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_waitAbs
Signal FpySequencer::waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive) {
    this->m_runtime.wakeupTime = directive.getwakeupTime();
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_setLocalVar
Signal FpySequencer::setLocalVar_directiveHandler(const FpySequencer_SetLocalVarDirective& directive) {
    if (directive.getindex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    // coding error. should have checked this when we were deserializing the directive. prefer to crash
    // rather than just fail the sequence
    FW_ASSERT(directive.get_valueSize() <= Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE,
              static_cast<FwAssertArgType>(directive.get_valueSize()),
              static_cast<FwAssertArgType>(Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE));

    this->m_runtime.localVariables[directive.getindex()].valueSize = directive.get_valueSize();

    (void)memcpy(this->m_runtime.localVariables[directive.getindex()].value, directive.getvalue(),
                 static_cast<size_t>(directive.get_valueSize()));

    return Signal::stmtResponse_success;
}

//! Internal interface handler for directive_goto
Signal FpySequencer::goto_directiveHandler(const FpySequencer_GotoDirective& directive) {
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.getstatementIndex() > m_sequenceObj.getheader().getstatementCount()) {
        return Signal::stmtResponse_failure;
    }
    m_runtime.nextStatementIndex = directive.getstatementIndex();
    return Signal::stmtResponse_success;
}

//! Internal interface handler for directive_if
Signal FpySequencer::if_directiveHandler(const FpySequencer_IfDirective& directive) {
    if (directive.getconditionalLocalVarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.getfalseGotoStmtIndex() > m_sequenceObj.getheader().getstatementCount()) {
        return Signal::stmtResponse_failure;
    }

    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getconditionalLocalVarIndex()];
    // create an esb so we don't modify the actual buf
    Fw::ExternalSerializeBuffer conditionalEsb(lvar.value, lvar.valueSize);
    Fw::SerializeStatus status = conditionalEsb.setBuffLen(lvar.valueSize);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);  // coding error if this fails
    bool conditional;
    status = conditionalEsb.deserialize(conditional);

    if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // failed to interpret this local variable as a boolean
        return Signal::stmtResponse_failure;
    }

    if (conditionalEsb.getBuffLeft() != 0) {
        // fail cuz this buf contained more than just a boolean
        return Signal::stmtResponse_failure;
    }

    if (conditional) {
        // proceed to next instruction
        return Signal::stmtResponse_success;
    }

    // conditional false case
    this->m_runtime.nextStatementIndex = directive.getfalseGotoStmtIndex();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::noOp_directiveHandler(const FpySequencer_NoOpDirective& directive) {
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getTlm_directiveHandler(const FpySequencer_GetTlmDirective& directive) {
    if (directive.getvalueDestLvar() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    if (directive.gettimeDestLvar() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        return Signal::stmtResponse_failure;
    }
    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    Fw::TlmValid valid = this->getTlmChan_out(0, directive.getchanId(), tlmTime, tlmValue);

    if (valid != Fw::TlmValid::VALID) {
        // could not find this tlm chan
        return Signal::stmtResponse_failure;
    }

    // this is an assert in the hpp, the buf should never be bigger than TLM_BUF_MAX
    FW_ASSERT(tlmValue.getBuffLength() <= Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE,
              static_cast<FwAssertArgType>(tlmValue.getBuffLength()));

    // copy value into lvar
    Runtime::LocalVariable& valueLvar = this->m_runtime.localVariables[directive.getvalueDestLvar()];
    memcpy(valueLvar.value, tlmValue.getBuffAddr(), static_cast<size_t>(tlmValue.getBuffLength()));
    valueLvar.valueSize = tlmValue.getBuffLength();

    // serialize time into lvar
    Runtime::LocalVariable& timeLvar = this->m_runtime.localVariables[directive.gettimeDestLvar()];
    // clear the lvar in case of early return
    timeLvar.valueSize = 0;
    Fw::ExternalSerializeBuffer esb(timeLvar.value, Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE);
    Fw::SerializeStatus stat = esb.serialize(tlmTime);

    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // failed to serialize Fw::Time into the lvar
        return Signal::stmtResponse_failure;
    }

    timeLvar.valueSize = esb.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive) {
    if (directive.getdestLvarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_prmGet_OutputPort(0)) {
        return Signal::stmtResponse_failure;
    }
    Fw::ParamBuffer prmValue;
    // set buff len to 0 before call so we can detect if we failed to get it
    prmValue.setBuffLen(0);
    Fw::ParamValid valid = this->getParam_out(0, directive.getprmId(), prmValue);

    if (valid != Fw::ParamValid::VALID) {
        // could not find this prm in the DB
        return Signal::stmtResponse_failure;
    }

    if (prmValue.getBuffLength() > Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE) {
        // cannot store the prm value in the lvar
        return Signal::stmtResponse_failure;
    }
    // copy value into lvar
    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getdestLvarIndex()];
    memcpy(lvar.value, prmValue.getBuffAddr(), static_cast<size_t>(prmValue.getBuffLength()));
    lvar.valueSize = prmValue.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::cmd_directiveHandler(const FpySequencer_CmdDirective& directive) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacket::FW_PACKET_COMMAND);
    // TODO should I assert here? this really shouldn't fail, I should just add a static assert
    // on com buf size and then assert here
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_CommandSerializeError(directive.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   sizeof(Fw::ComPacket::FW_PACKET_COMMAND), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Signal::stmtResponse_failure;
    }
    // TODO same as above
    stat = cmdBuf.serialize(directive.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_CommandSerializeError(directive.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   sizeof(directive.getopCode()), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Signal::stmtResponse_failure;
    }
    stat = cmdBuf.serialize(directive.getargBuf(), directive.get_argBufSize(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // most failures of directives are not error messages... TODO should this just fail silently?
        this->log_WARNING_HI_CommandSerializeError(directive.getopCode(), cmdBuf.getBuffCapacity(), cmdBuf.getBuffLength(),
                                                   directive.get_argBufSize(), stat,
                                                   this->m_runtime.nextStatementIndex - 1);
        return Signal::stmtResponse_failure;
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

    // now tell the SM to wait some more until we get the cmd response back
    return Signal::stmtResponse_keepWaiting;
}
}  // namespace Svc
