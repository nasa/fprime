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

I64& FpySequencer::reg(U8 idx) {
    return this->m_runtime.registers[idx];
}

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const FpySequencer_WaitRelDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->waitRel_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const FpySequencer_WaitAbsDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->waitAbs_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_setLocalVar
void FpySequencer::directive_setLocalVar_internalInterfaceHandler(
    const Svc::FpySequencer_SetLocalVarDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->setLocalVar_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_goto
void FpySequencer::directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->goto_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_if
void FpySequencer::directive_if_internalInterfaceHandler(const Svc::FpySequencer_IfDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->if_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_noOp
void FpySequencer::directive_noOp_internalInterfaceHandler(const Svc::FpySequencer_NoOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->noOp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_getTlm
void FpySequencer::directive_getTlm_internalInterfaceHandler(const Svc::FpySequencer_GetTlmDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->getTlm_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_getPrm
void FpySequencer::directive_getPrm_internalInterfaceHandler(const Svc::FpySequencer_GetPrmDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->getPrm_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_cmd
void FpySequencer::directive_cmd_internalInterfaceHandler(const Svc::FpySequencer_CmdDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->cmd_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_or
void FpySequencer::directive_or_internalInterfaceHandler(const Svc::FpySequencer_OrDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->or_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_deserLocalVar
void FpySequencer::directive_deserLocalVar_internalInterfaceHandler(const Svc::FpySequencer_DeserLocalVarDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->deserLocalVar_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_store
void FpySequencer::directive_store_internalInterfaceHandler(const Svc::FpySequencer_StoreDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->store_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_binaryCmp
void FpySequencer::directive_binaryCmp_internalInterfaceHandler(const Svc::FpySequencer_BinaryCmpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->binaryCmp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_waitRel
Signal FpySequencer::waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& error) {
    Fw::Time wakeupTime = this->getTime();

    wakeupTime.add(directive.getduration().getSeconds(), directive.getduration().getUSeconds());
    this->m_runtime.wakeupTime = wakeupTime;
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_waitAbs
Signal FpySequencer::waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& error) {
    this->m_runtime.wakeupTime = directive.getwakeupTime();
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_setLocalVar
Signal FpySequencer::setLocalVar_directiveHandler(const FpySequencer_SetLocalVarDirective& directive, DirectiveError& error) {
    if (directive.getindex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        error = DirectiveError::LVAR_OUT_OF_BOUNDS;
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
Signal FpySequencer::goto_directiveHandler(const FpySequencer_GotoDirective& directive, DirectiveError& error) {
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.getstatementIndex() > m_sequenceObj.getheader().getstatementCount()) {
        error = DirectiveError::STMT_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    m_runtime.nextStatementIndex = directive.getstatementIndex();
    return Signal::stmtResponse_success;
}

//! Internal interface handler for directive_if
Signal FpySequencer::if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& error) {
    if (directive.getconditionalReg() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.getfalseGotoStmtIndex() > m_sequenceObj.getheader().getstatementCount()) {
        error = DirectiveError::STMT_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    if (reg(directive.getconditionalReg())) {
        // proceed to next instruction
        return Signal::stmtResponse_success;
    }

    // conditional false case
    this->m_runtime.nextStatementIndex = directive.getfalseGotoStmtIndex();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::noOp_directiveHandler(const FpySequencer_NoOpDirective& directive, DirectiveError& error) {
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getTlm_directiveHandler(const FpySequencer_GetTlmDirective& directive, DirectiveError& error) {
    if (directive.getvalueDestLvar() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        error = DirectiveError::LVAR_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (directive.gettimeDestLvar() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        error = DirectiveError::LVAR_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        error = DirectiveError::TLM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }
    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    Fw::TlmValid valid = this->getTlmChan_out(0, directive.getchanId(), tlmTime, tlmValue);

    if (valid != Fw::TlmValid::VALID) {
        // could not find this tlm chan
        error = DirectiveError::TLM_CHAN_NOT_FOUND;
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
        error = DirectiveError::LVAR_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }

    timeLvar.valueSize = esb.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive, DirectiveError& error) {
    if (directive.getdestLvarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        error = DirectiveError::LVAR_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_prmGet_OutputPort(0)) {
        error = DirectiveError::PRM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }
    Fw::ParamBuffer prmValue;
    // set buff len to 0 before call so we can detect if we failed to get it
    prmValue.setBuffLen(0);
    Fw::ParamValid valid = this->getParam_out(0, directive.getprmId(), prmValue);

    if (valid != Fw::ParamValid::VALID) {
        // could not find this prm in the DB
        error = DirectiveError::PRM_NOT_FOUND;
        return Signal::stmtResponse_failure;
    }

    if (prmValue.getBuffLength() > Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE) {
        // cannot store the prm value in the lvar
        error = DirectiveError::LVAR_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }
    // copy value into lvar
    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getdestLvarIndex()];
    memcpy(lvar.value, prmValue.getBuffAddr(), static_cast<size_t>(prmValue.getBuffLength()));
    lvar.valueSize = prmValue.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::cmd_directiveHandler(const FpySequencer_CmdDirective& directive, DirectiveError& error) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacket::FW_PACKET_COMMAND);
    // TODO should I assert here? this really shouldn't fail, I should just add a static assert
    // on com buf size and then assert here
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        error = DirectiveError::CMD_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }
    stat = cmdBuf.serialize(directive.getopCode());
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        error = DirectiveError::CMD_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }
    stat = cmdBuf.serialize(directive.getargBuf(), directive.get_argBufSize(), true);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        error = DirectiveError::CMD_SERIALIZE_FAILURE;
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

Signal FpySequencer::or_directiveHandler(const FpySequencer_OrDirective& directive, DirectiveError& error) {
    
    if (directive.getlhs() >= Fpy::NUM_REGISTERS 
        || directive.getrhs() >= Fpy::NUM_REGISTERS 
        || directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    reg(directive.getres()) = reg(directive.getlhs()) | reg(directive.getrhs());

    return Signal::stmtResponse_success;
}

Signal FpySequencer::deserLocalVar_directiveHandler(const FpySequencer_DeserLocalVarDirective& directive, DirectiveError& error) {
    if (directive.getsrcLvarIdx() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        error = DirectiveError::LVAR_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (directive.getdestReg() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getsrcLvarIdx()];
    if (directive.getsrcOffset() + directive.get_deserSize() > lvar.valueSize) {
        error = DirectiveError::LVAR_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // TODO can I use htons/htonl? this code could be way simpler
    Fw::ExternalSerializeBuffer esb(lvar.value, lvar.valueSize);
    esb.setBuffLen(lvar.valueSize);
    FW_ASSERT(esb.deserializeSkip(directive.getsrcOffset()) == Fw::SerializeStatus::FW_SERIALIZE_OK);

    I8 oneByte;
    I16 twoBytes;
    I32 fourBytes;
    I64 eightBytes;

    switch (directive.get_deserSize()) {
        case 1: {
            // all these desers should succeed as we've already checked the size above
            FW_ASSERT(esb.deserialize(oneByte) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            reg(directive.getdestReg()) = oneByte;
            break;
        }
        case 2: {
            FW_ASSERT(esb.deserialize(twoBytes) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            reg(directive.getdestReg()) = twoBytes;
            break;
        }
        case 4: {
            FW_ASSERT(esb.deserialize(fourBytes) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            reg(directive.getdestReg()) = fourBytes;
            break;
        }
        case 8: {
            FW_ASSERT(esb.deserialize(eightBytes) == Fw::SerializeStatus::FW_SERIALIZE_OK);
            reg(directive.getdestReg()) = eightBytes;
            break;
        }
        default: {
            FW_ASSERT(0, static_cast<FwAssertArgType>(directive.get_deserSize()));
            return Signal::stmtResponse_failure;
        }
    }

    return Signal::stmtResponse_success;
}

Signal FpySequencer::store_directiveHandler(const FpySequencer_StoreDirective& directive, DirectiveError& error) {
    if (directive.getdest() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    reg(directive.getdest()) = directive.getvalue();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::binaryCmp_directiveHandler(const FpySequencer_BinaryCmpDirective& directive, DirectiveError& error) {
    if (directive.getlhs() >= Fpy::NUM_REGISTERS 
        || directive.getrhs() >= Fpy::NUM_REGISTERS 
        || directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    
    I8 cmpResult;

    if (reg(directive.getlhs()) < reg(directive.getrhs())) {
        cmpResult = -1;
    } else if (reg(directive.getlhs()) == reg(directive.getrhs())) {
        cmpResult = 0;
    } else {
        cmpResult = 1;
    }

    if ((directive.get_op() == FpySequencer_BinaryCmpOperation::LT
        || directive.get_op() == FpySequencer_BinaryCmpOperation::LE)
        && cmpResult == -1) {
        // op was less than, and cmp was less than
        reg(directive.getres()) = 1;
    } else if ((directive.get_op() == FpySequencer_BinaryCmpOperation::GT
                || directive.get_op() == FpySequencer_BinaryCmpOperation::GE)
                && cmpResult == 1) {
        // op was greater than, and cmp was greater than
        reg(directive.getres()) = 1;
    } else if ((directive.get_op() == FpySequencer_BinaryCmpOperation::EQ
                || directive.get_op() == FpySequencer_BinaryCmpOperation::GE
                || directive.get_op() == FpySequencer_BinaryCmpOperation::GT)
                && cmpResult == 0) {
        // op was equal to, and cmp was equal to
        reg(directive.getres()) = 1;
    } else {
        // no match between operation and cmp result
        reg(directive.getres()) = 0;
    }

    return Signal::stmtResponse_success;
}
}  // namespace Svc
