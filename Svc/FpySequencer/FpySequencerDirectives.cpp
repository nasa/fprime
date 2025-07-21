#include <cmath>
#include "Fw/Com/ComPacket.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"

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
void FpySequencer::directive_setSerReg_internalInterfaceHandler(const Svc::FpySequencer_SetSerRegDirective& directive) {
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
void FpySequencer::directive_deserSerReg_internalInterfaceHandler(
    const Svc::FpySequencer_DeserSerRegDirective& directive) {
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

//! Internal interface handler for directive_binaryRegOp
void FpySequencer::directive_binaryRegOp_internalInterfaceHandler(
    const Svc::FpySequencer_BinaryRegOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->binaryRegOp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_unaryRegOp
void FpySequencer::directive_unaryRegOp_internalInterfaceHandler(
    const Svc::FpySequencer_UnaryRegOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->unaryRegOp_directiveHandler(directive, error));
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
Signal FpySequencer::setSerReg_directiveHandler(const FpySequencer_SetSerRegDirective& directive,
                                                DirectiveError& error) {
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

Signal FpySequencer::deserSerReg_directiveHandler(const FpySequencer_DeserSerRegDirective& directive,
                                                  DirectiveError& error) {
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

I8 floatCmp(F64 lhs, F64 rhs) {
    if (std::isunordered(lhs, rhs)) {
        // nan is one of the args
        // always fail a comparison if nan
        return -2;
    } else if (std::isgreater(lhs, rhs)) {
        return 1;
    } else if (std::isless(lhs, rhs)) {
        return -1;
    }
    return 0;
}

I64 FpySequencer::binaryRegOp_or(I64 lhs, I64 rhs) {
    return lhs | rhs;
}
I64 FpySequencer::binaryRegOp_and(I64 lhs, I64 rhs) {
    return lhs & rhs;
}
I64 FpySequencer::binaryRegOp_ieq(I64 lhs, I64 rhs) {
    return lhs == rhs;
}
I64 FpySequencer::binaryRegOp_ine(I64 lhs, I64 rhs) {
    return lhs != rhs;
}
I64 FpySequencer::binaryRegOp_ult(I64 lhs, I64 rhs) {
    return static_cast<U64>(lhs) < static_cast<U64>(rhs);
}
I64 FpySequencer::binaryRegOp_ule(I64 lhs, I64 rhs) {
    return static_cast<U64>(lhs) <= static_cast<U64>(rhs);
}
I64 FpySequencer::binaryRegOp_ugt(I64 lhs, I64 rhs) {
    return static_cast<U64>(lhs) > static_cast<U64>(rhs);
}
I64 FpySequencer::binaryRegOp_uge(I64 lhs, I64 rhs) {
    return static_cast<U64>(lhs) >= static_cast<U64>(rhs);
}
I64 FpySequencer::binaryRegOp_slt(I64 lhs, I64 rhs) {
    return lhs < rhs;
}
I64 FpySequencer::binaryRegOp_sle(I64 lhs, I64 rhs) {
    return lhs <= rhs;
}
I64 FpySequencer::binaryRegOp_sgt(I64 lhs, I64 rhs) {
    return lhs > rhs;
}
I64 FpySequencer::binaryRegOp_sge(I64 lhs, I64 rhs) {
    return lhs >= rhs;
}
I64 FpySequencer::binaryRegOp_feq(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    return floatCmp(left, right) == 0;
}
I64 FpySequencer::binaryRegOp_fne(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    I8 cmp = floatCmp(left, right);
    // ne is true if they are not equal and neither is nan
    return cmp != 0 && cmp != -2;
}
I64 FpySequencer::binaryRegOp_flt(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    return floatCmp(left, right) == -1;
}
I64 FpySequencer::binaryRegOp_fle(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    I8 cmp = floatCmp(left, right);
    return cmp == 0 || cmp == -1;
}
I64 FpySequencer::binaryRegOp_fgt(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    return floatCmp(left, right) == 1;
}
I64 FpySequencer::binaryRegOp_fge(I64 lhs, I64 rhs) {
    F64 left;
    memcpy(&left, &lhs, sizeof(left));
    F64 right;
    memcpy(&right, &rhs, sizeof(right));
    I8 cmp = floatCmp(left, right);
    return cmp == 0 || cmp == 1;
}

Signal FpySequencer::binaryRegOp_directiveHandler(const FpySequencer_BinaryRegOpDirective& directive,
                                                  DirectiveError& error) {
    // coding error, should not have gotten to this binary reg op handler
    FW_ASSERT(directive.get_op() >= Fpy::DirectiveId::OR && directive.get_op() <= Fpy::DirectiveId::FGE,
              static_cast<FwAssertArgType>(directive.get_op()));

    if (directive.getlhs() >= Fpy::NUM_REGISTERS || directive.getrhs() >= Fpy::NUM_REGISTERS ||
        directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    I64 lhs = reg(directive.getlhs());
    I64 rhs = reg(directive.getrhs());
    I64& res = reg(directive.getres());

    switch (directive.get_op()) {
        case Fpy::DirectiveId::OR:
            res = this->binaryRegOp_or(lhs, rhs);
            break;
        case Fpy::DirectiveId::AND:
            res = this->binaryRegOp_and(lhs, rhs);
            break;
        case Fpy::DirectiveId::IEQ:
            res = this->binaryRegOp_ieq(lhs, rhs);
            break;
        case Fpy::DirectiveId::INE:
            res = this->binaryRegOp_ine(lhs, rhs);
            break;
        case Fpy::DirectiveId::ULT:
            res = this->binaryRegOp_ult(lhs, rhs);
            break;
        case Fpy::DirectiveId::ULE:
            res = this->binaryRegOp_ule(lhs, rhs);
            break;
        case Fpy::DirectiveId::UGT:
            res = this->binaryRegOp_ugt(lhs, rhs);
            break;
        case Fpy::DirectiveId::UGE:
            res = this->binaryRegOp_uge(lhs, rhs);
            break;
        case Fpy::DirectiveId::SLT:
            res = this->binaryRegOp_slt(lhs, rhs);
            break;
        case Fpy::DirectiveId::SLE:
            res = this->binaryRegOp_sle(lhs, rhs);
            break;
        case Fpy::DirectiveId::SGT:
            res = this->binaryRegOp_sgt(lhs, rhs);
            break;
        case Fpy::DirectiveId::SGE:
            res = this->binaryRegOp_sge(lhs, rhs);
            break;
        case Fpy::DirectiveId::FEQ:
            res = this->binaryRegOp_feq(lhs, rhs);
            break;
        case Fpy::DirectiveId::FNE:
            res = this->binaryRegOp_fne(lhs, rhs);
            break;
        case Fpy::DirectiveId::FLT:
            res = this->binaryRegOp_flt(lhs, rhs);
            break;
        case Fpy::DirectiveId::FLE:
            res = this->binaryRegOp_fle(lhs, rhs);
            break;
        case Fpy::DirectiveId::FGT:
            res = this->binaryRegOp_fgt(lhs, rhs);
            break;
        case Fpy::DirectiveId::FGE:
            res = this->binaryRegOp_fge(lhs, rhs);
            break;
        default:
            FW_ASSERT(0, directive.get_op());
            break;
    }
    return Signal::stmtResponse_success;
}
I64 FpySequencer::unaryRegOp_not(I64 src) {
    if (src) {
        return static_cast<I64>(false);
    }
    return static_cast<I64>(true);
}
I64 FpySequencer::unaryRegOp_fpext(I64 src) {
    // convert F32 to F64
    // first get the first 32 bits of src
    I32 trunc = static_cast<I32>(src);
    // then interpret as float
    F32 fsrc;
    memcpy(&fsrc, &trunc, sizeof(fsrc));
    // then cast to F64
    F64 ext = static_cast<F64>(fsrc);
    // then return bits as I64
    I64 iext;
    memcpy(&iext, &ext, sizeof(iext));
    return iext;
}
I64 FpySequencer::unaryRegOp_fptrunc(I64 src) {
    // convert F64 to F32
    // first interpret as F64
    F64 fsrc;
    memcpy(&fsrc, &src, sizeof(fsrc));
    // then cast to F32
    F32 trunc = static_cast<F32>(fsrc);
    // then interpret bits as I32
    I32 itrunc;
    memcpy(&itrunc, &trunc, sizeof(itrunc));
    // then extend to I64
    return static_cast<I64>(itrunc);
}
I64 FpySequencer::unaryRegOp_fptosi(I64 src) {
    // first interpret as F64
    F64 fsrc;
    memcpy(&fsrc, &src, sizeof(fsrc));
    // then static cast to int
    return static_cast<I64>(fsrc);
}
I64 FpySequencer::unaryRegOp_sitofp(I64 src) {
    // first static cast to float
    F64 fsrc = static_cast<F64>(src);
    // then return bits as I64
    I64 res;
    memcpy(&res, &fsrc, sizeof(res));
    return res;
}
I64 FpySequencer::unaryRegOp_fptoui(I64 src) {
    // first interpret as F64
    F64 fsrc;
    memcpy(&fsrc, &src, sizeof(fsrc));
    // then static cast to unsigned int
    // then return as a signed int
    return static_cast<I64>(static_cast<U64>(fsrc));
}
I64 FpySequencer::unaryRegOp_uitofp(I64 src) {
    // first static cast to unsigned, then to float
    F64 fsrc = static_cast<F64>(static_cast<U64>(src));
    // then return bits as I64
    I64 res;
    memcpy(&res, &fsrc, sizeof(res));
    return res;
}
Signal FpySequencer::unaryRegOp_directiveHandler(const FpySequencer_UnaryRegOpDirective& directive,
                                                 DirectiveError& error) {
    // coding error, should not have gotten to this unary reg op handler
    FW_ASSERT(directive.get_op() >= Fpy::DirectiveId::NOT && directive.get_op() <= Fpy::DirectiveId::UITOFP,
              static_cast<FwAssertArgType>(directive.get_op()));

    if (directive.getsrc() >= Fpy::NUM_REGISTERS || directive.getres() >= Fpy::NUM_REGISTERS) {
        error = DirectiveError::REGISTER_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    I64 src = reg(directive.getsrc());
    I64& res = reg(directive.getres());

    switch (directive.get_op()) {
        case Fpy::DirectiveId::NOT:
            res = this->unaryRegOp_not(src);
            break;
        case Fpy::DirectiveId::FPEXT:
            res = this->unaryRegOp_fpext(src);
            break;
        case Fpy::DirectiveId::FPTRUNC:
            res = this->unaryRegOp_fptrunc(src);
            break;
        case Fpy::DirectiveId::FPTOSI:
            res = this->unaryRegOp_fptosi(src);
            break;
        case Fpy::DirectiveId::FPTOUI:
            res = this->unaryRegOp_fptoui(src);
            break;
        case Fpy::DirectiveId::SITOFP:
            res = this->unaryRegOp_sitofp(src);
            break;
        case Fpy::DirectiveId::UITOFP:
            res = this->unaryRegOp_uitofp(src);
            break;
        default:
            FW_ASSERT(0, directive.get_op());
            break;
    }
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
