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
    return this->m_runtime.regs[idx];
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

//! Internal interface handler for directive_setSerReg
void FpySequencer::directive_setSerReg_internalInterfaceHandler(
    const Svc::FpySequencer_SetSerRegDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->setSerReg_directiveHandler(directive, error));
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

//! Internal interface handler for directive_deserSerReg
void FpySequencer::directive_deserSerReg_internalInterfaceHandler(const Svc::FpySequencer_DeserSerRegDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->deserSerReg_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_setReg
void FpySequencer::directive_setReg_internalInterfaceHandler(const Svc::FpySequencer_SetRegDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->setReg_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_binaryCmp
void FpySequencer::directive_binaryCmp_internalInterfaceHandler(const Svc::FpySequencer_BinaryCmpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->binaryCmp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_not
void FpySequencer::directive_not_internalInterfaceHandler(const Svc::FpySequencer_NotDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->not_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_exit
void FpySequencer::directive_exit_internalInterfaceHandler(const Svc::FpySequencer_ExitDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->exit_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_waitRel
Signal FpySequencer::waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& error) {
    Fw::Time wakeupTime = this->getTime();

    wakeupTime.add(directive.getseconds(), directive.getuSeconds());
    this->m_runtime.wakeupTime = wakeupTime;
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_waitAbs
Signal FpySequencer::waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& error) {
    this->m_runtime.wakeupTime = directive.getwakeupTime();
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_setSerReg
Signal FpySequencer::setSerReg_directiveHandler(const FpySequencer_SetSerRegDirective& directive, DirectiveError& error) {
    if (directive.getindex() >= Fpy::NUM_SERIALIZABLE_REGISTERS) {
        error = DirectiveError::SER_REG_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // coding error. should have checked this when we were deserializing the directive. prefer to crash
    // rather than just fail the sequence
    FW_ASSERT(directive.get_valueSize() <= Fpy::MAX_SERIALIZABLE_REGISTER_SIZE,
              static_cast<FwAssertArgType>(directive.get_valueSize()),
              static_cast<FwAssertArgType>(Fpy::MAX_SERIALIZABLE_REGISTER_SIZE));

    this->m_runtime.serRegs[directive.getindex()].valueSize = directive.get_valueSize();

    (void)memcpy(this->m_runtime.serRegs[directive.getindex()].value, directive.getvalue(),
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
    if (directive.getvalueDestSerReg() >= Fpy::NUM_SERIALIZABLE_REGISTERS) {
        error = DirectiveError::SER_REG_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (directive.gettimeDestSerReg() >= Fpy::NUM_SERIALIZABLE_REGISTERS) {
        error = DirectiveError::SER_REG_OUT_OF_BOUNDS;
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
    FW_ASSERT(tlmValue.getBuffLength() <= Fpy::MAX_SERIALIZABLE_REGISTER_SIZE,
              static_cast<FwAssertArgType>(tlmValue.getBuffLength()));

    // copy value into serReg
    Runtime::SerializableReg& valueSerReg = this->m_runtime.serRegs[directive.getvalueDestSerReg()];
    memcpy(valueSerReg.value, tlmValue.getBuffAddr(), static_cast<size_t>(tlmValue.getBuffLength()));
    valueSerReg.valueSize = tlmValue.getBuffLength();

    // serialize time into serReg
    Runtime::SerializableReg& timeSerReg = this->m_runtime.serRegs[directive.gettimeDestSerReg()];
    // clear the serReg in case of early return
    timeSerReg.valueSize = 0;
    Fw::ExternalSerializeBuffer esb(timeSerReg.value, Fpy::MAX_SERIALIZABLE_REGISTER_SIZE);
    Fw::SerializeStatus stat = esb.serialize(tlmTime);

    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // failed to serialize Fw::Time into the serReg
        error = DirectiveError::SER_REG_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }

    timeSerReg.valueSize = esb.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive, DirectiveError& error) {
    if (directive.getdestSerRegIndex() >= Fpy::NUM_SERIALIZABLE_REGISTERS) {
        error = DirectiveError::SER_REG_OUT_OF_BOUNDS;
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

    if (prmValue.getBuffLength() > Fpy::MAX_SERIALIZABLE_REGISTER_SIZE) {
        // cannot setReg the prm value in the serReg
        error = DirectiveError::SER_REG_SERIALIZE_FAILURE;
        return Signal::stmtResponse_failure;
    }
    // copy value into serReg
    Runtime::SerializableReg& serReg = this->m_runtime.serRegs[directive.getdestSerRegIndex()];
    memcpy(serReg.value, prmValue.getBuffAddr(), static_cast<size_t>(prmValue.getBuffLength()));
    serReg.valueSize = prmValue.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::cmd_directiveHandler(const FpySequencer_CmdDirective& directive, DirectiveError& error) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat = cmdBuf.serialize(Fw::ComPacketType::FW_PACKET_COMMAND);
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
    stat = cmdBuf.serialize(directive.getargBuf(), directive.get_argBufSize(), Fw::Serialization::OMIT_LENGTH);
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

Signal FpySequencer::deserSerReg_directiveHandler(const FpySequencer_DeserSerRegDirective& directive, DirectiveError& error) {
    if (directive.getsrcSerRegIdx() >= Fpy::NUM_SERIALIZABLE_REGISTERS) {
        error = DirectiveError::SER_REG_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (directive.getdestReg() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Runtime::SerializableReg& serReg = this->m_runtime.serRegs[directive.getsrcSerRegIdx()];
    if (directive.getsrcOffset() + directive.get_deserSize() > serReg.valueSize) {
        error = DirectiveError::SER_REG_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // TODO can I use htons/htonl? this code could be way simpler
    Fw::ExternalSerializeBuffer esb(serReg.value, serReg.valueSize);
    esb.setBuffLen(serReg.valueSize);
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

Signal FpySequencer::setReg_directiveHandler(const FpySequencer_SetRegDirective& directive, DirectiveError& error) {
    if (directive.getdest() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    reg(directive.getdest()) = directive.getvalue();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::binaryCmp_directiveHandler(const FpySequencer_BinaryCmpDirective& directive, DirectiveError& error) {

    // coding error, should not have gotten to this binary cmp handler
    FW_ASSERT(directive.get_op() >= Fpy::DirectiveId::OR && directive.get_op() <= Fpy::DirectiveId::SGE, static_cast<FwAssertArgType>(directive.get_op()));

    if (directive.getlhs() >= Fpy::NUM_REGISTERS 
        || directive.getrhs() >= Fpy::NUM_REGISTERS 
        || directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    I64 lhs = reg(directive.getlhs());
    I64 rhs = reg(directive.getrhs());
    I64& res = reg(directive.getres());
    
    if (directive.get_op() == Fpy::DirectiveId::EQ) {
        res = lhs == rhs;
        return Signal::stmtResponse_success;
    }

    if (directive.get_op() == Fpy::DirectiveId::NE) {
        res = lhs != rhs;
        return Signal::stmtResponse_success;
    }

    if (directive.get_op() == Fpy::DirectiveId::OR) {
        res = lhs | rhs;
        return Signal::stmtResponse_success;
    }

    if (directive.get_op() == Fpy::DirectiveId::AND) {
        res = lhs & rhs;
        return Signal::stmtResponse_success;
    }

    // okay, it is an inequality comparison

    // whether the comparison is signed or unsigned
    bool sign = true;

    if (directive.get_op() >= Fpy::DirectiveId::ULT && directive.get_op() <= Fpy::DirectiveId::UGE) {
        sign = false;
    }

    I8 cmpResult;

    if (sign) {
        cmpResult = (lhs == rhs) ? 0 : (lhs < rhs) ? -1 : 1;
    } else {
        // unsigned comparison. static cast to unsigned longs
        U64 ulhs = static_cast<U64>(lhs);
        U64 urhs = static_cast<U64>(rhs);
        cmpResult = (ulhs == urhs) ? 0 : (ulhs < urhs) ? -1 : 1;
    }

    if (cmpResult == 0) {
        // values were equal
        // result is true if equality is okay
        res = (directive.get_op() == Fpy::DirectiveId::UGE 
            || directive.get_op() == Fpy::DirectiveId::ULE 
            || directive.get_op() == Fpy::DirectiveId::SGE 
            || directive.get_op() == Fpy::DirectiveId::SLE);
    } else if (cmpResult == -1) {
        // lhs < rhs
        // result is true if < is okay
        res = (directive.get_op() == Fpy::DirectiveId::ULT
            || directive.get_op() == Fpy::DirectiveId::ULE 
            || directive.get_op() == Fpy::DirectiveId::SLT
            || directive.get_op() == Fpy::DirectiveId::SLE);
    } else {
        // lhs > rhs
        // result is true if > is okay
        res = (directive.get_op() == Fpy::DirectiveId::UGT
            || directive.get_op() == Fpy::DirectiveId::UGE 
            || directive.get_op() == Fpy::DirectiveId::SGT
            || directive.get_op() == Fpy::DirectiveId::SGE);
    }

    return Signal::stmtResponse_success;
}

Signal FpySequencer::not_directiveHandler(const FpySequencer_NotDirective& directive, DirectiveError& error) {
    if (directive.getsrc() >= Fpy::NUM_REGISTERS 
        || directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    I64 src = reg(directive.getsrc());
    I64& res = reg(directive.getres());
    res = ~src;
    return Signal::stmtResponse_success;
}

Signal FpySequencer::exit_directiveHandler(const FpySequencer_ExitDirective& directive, DirectiveError& error) {
    if (directive.getsuccess()) {
        // just goto the end of the sequence
        this->m_runtime.nextStatementIndex = this->m_sequenceObj.getheader().getstatementCount();
        return Signal::stmtResponse_success;
    }
    // otherwise, kill the sequence here
    error = DirectiveError::DELIBERATE_FAILURE;
    return Signal::stmtResponse_failure;
}
}  // namespace Svc
