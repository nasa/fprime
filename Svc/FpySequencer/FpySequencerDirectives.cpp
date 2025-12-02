#include <cmath>
#include <cstring>
#include <type_traits>
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

// utility method for updating telemetry based on a directive error code
void FpySequencer::handleDirectiveErrorCode(Fpy::DirectiveId id, DirectiveError err) {
    this->m_tlm.lastDirectiveError = err;
    if (err != DirectiveError::NO_ERROR) {
        this->m_tlm.directiveErrorIndex = this->currentStatementIdx();
        this->m_tlm.directiveErrorId = id;
    }
}

Fw::Success FpySequencer::sendCmd(FwOpcodeType opcode, const U8* argBuf, FwSizeType argBufSize) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat =
        cmdBuf.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
    // TODO should I assert here? this really shouldn't fail, I should just add a static assert
    // on com buf size and then assert here
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serializeFrom(opcode);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serializeFrom(argBuf, argBufSize, Fw::Serialization::OMIT_LENGTH);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
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

    this->cmdOut_out(0, cmdBuf, cmdUid);

    return Fw::Success::SUCCESS;
}

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const FpySequencer_WaitRelDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->waitRel_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::WAIT_REL, error);
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const FpySequencer_WaitAbsDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->waitAbs_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::WAIT_ABS, error);
}

//! Internal interface handler for directive_goto
void FpySequencer::directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->goto_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::GOTO, error);
}

//! Internal interface handler for directive_if
void FpySequencer::directive_if_internalInterfaceHandler(const Svc::FpySequencer_IfDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->if_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::IF, error);
}

//! Internal interface handler for directive_noOp
void FpySequencer::directive_noOp_internalInterfaceHandler(const Svc::FpySequencer_NoOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->noOp_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::NO_OP, error);
}

//! Internal interface handler for directive_pushTlmVal
void FpySequencer::directive_pushTlmVal_internalInterfaceHandler(
    const Svc::FpySequencer_PushTlmValDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushTlmVal_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PUSH_TLM_VAL, error);
}

//! Internal interface handler for directive_pushTlmValAndTime
void FpySequencer::directive_pushTlmValAndTime_internalInterfaceHandler(
    const Svc::FpySequencer_PushTlmValAndTimeDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushTlmValAndTime_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PUSH_TLM_VAL_AND_TIME, error);
}

//! Internal interface handler for directive_pushPrm
void FpySequencer::directive_pushPrm_internalInterfaceHandler(const Svc::FpySequencer_PushPrmDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushPrm_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PUSH_PRM, error);
}

//! Internal interface handler for directive_constCmd
void FpySequencer::directive_constCmd_internalInterfaceHandler(const Svc::FpySequencer_ConstCmdDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->constCmd_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::CONST_CMD, error);
}

//! Internal interface handler for directive_stackOp
void FpySequencer::directive_stackOp_internalInterfaceHandler(const Svc::FpySequencer_StackOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->stackOp_directiveHandler(directive, error));
    handleDirectiveErrorCode(directive.get__op(), error);
}

//! Internal interface handler for directive_exit
void FpySequencer::directive_exit_internalInterfaceHandler(const Svc::FpySequencer_ExitDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->exit_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::EXIT, error);
}

//! Internal interface handler for directive_allocate
void FpySequencer::directive_allocate_internalInterfaceHandler(const Svc::FpySequencer_AllocateDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->allocate_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::ALLOCATE, error);
}

//! Internal interface handler for directive_storeConstOffset
void FpySequencer::directive_storeConstOffset_internalInterfaceHandler(
    const Svc::FpySequencer_StoreConstOffsetDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->storeConstOffset_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::STORE_CONST_OFFSET, error);
}

//! Internal interface handler for directive_pushVal
void FpySequencer::directive_pushVal_internalInterfaceHandler(const Svc::FpySequencer_PushValDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushVal_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PUSH_VAL, error);
}

//! Internal interface handler for directive_load
void FpySequencer::directive_load_internalInterfaceHandler(const Svc::FpySequencer_LoadDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->load_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::LOAD, error);
}

//! Internal interface handler for directive_discard
void FpySequencer::directive_discard_internalInterfaceHandler(const Svc::FpySequencer_DiscardDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->discard_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::DISCARD, error);
}

//! Internal interface handler for directive_memCmp
void FpySequencer::directive_memCmp_internalInterfaceHandler(const Svc::FpySequencer_MemCmpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->memCmp_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::MEMCMP, error);
}

//! Internal interface handler for directive_stackCmd
void FpySequencer::directive_stackCmd_internalInterfaceHandler(const Svc::FpySequencer_StackCmdDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->stackCmd_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::STACK_CMD, error);
}

//! Internal interface handler for directive_pushTime
void FpySequencer::directive_pushTime_internalInterfaceHandler(const Svc::FpySequencer_PushTimeDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushTime_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PUSH_TIME, error);
}

//! Internal interface handler for directive_setFlag
void FpySequencer::directive_setFlag_internalInterfaceHandler(const Svc::FpySequencer_SetFlagDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->setFlag_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::SET_FLAG, error);
}

//! Internal interface handler for directive_getFlag
void FpySequencer::directive_getFlag_internalInterfaceHandler(const Svc::FpySequencer_GetFlagDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->getFlag_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::GET_FLAG, error);
}

//! Internal interface handler for directive_getField
void FpySequencer::directive_getField_internalInterfaceHandler(const Svc::FpySequencer_GetFieldDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->getField_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::GET_FIELD, error);
}

//! Internal interface handler for directive_peek
void FpySequencer::directive_peek_internalInterfaceHandler(const Svc::FpySequencer_PeekDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->peek_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::PEEK, error);
}

//! Internal interface handler for directive_store
void FpySequencer::directive_store_internalInterfaceHandler(const Svc::FpySequencer_StoreDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->store_directiveHandler(directive, error));
    handleDirectiveErrorCode(Fpy::DirectiveId::STORE, error);
}

//! Internal interface handler for directive_waitRel
Signal FpySequencer::waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < 8) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    Fw::Time wakeupTime = this->getTime();

    U32 uSeconds = this->m_runtime.stack.pop<U32>();
    U32 seconds = this->m_runtime.stack.pop<U32>();

    wakeupTime.add(seconds, uSeconds);
    this->m_runtime.wakeupTime = wakeupTime;
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_waitAbs
Signal FpySequencer::waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < 10 + sizeof(FwTimeContextStoreType)) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    U32 uSeconds = this->m_runtime.stack.pop<U32>();
    U32 seconds = this->m_runtime.stack.pop<U32>();
    FwTimeContextStoreType ctx = this->m_runtime.stack.pop<FwTimeContextStoreType>();
    U16 base = this->m_runtime.stack.pop<U16>();

    this->m_runtime.wakeupTime = Fw::Time(static_cast<TimeBase::T>(base), ctx, seconds, uSeconds);
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_goto
Signal FpySequencer::goto_directiveHandler(const FpySequencer_GotoDirective& directive, DirectiveError& error) {
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.get_statementIndex() > m_sequenceObj.get_header().get_statementCount()) {
        error = DirectiveError::STMT_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    m_runtime.nextStatementIndex = directive.get_statementIndex();
    return Signal::stmtResponse_success;
}

//! Internal interface handler for directive_if
Signal FpySequencer::if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < 1) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.get_falseGotoStmtIndex() > m_sequenceObj.get_header().get_statementCount()) {
        error = DirectiveError::STMT_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    if (this->m_runtime.stack.pop<U8>() != 0) {
        // proceed to next instruction
        return Signal::stmtResponse_success;
    }

    // conditional false case
    this->m_runtime.nextStatementIndex = directive.get_falseGotoStmtIndex();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::noOp_directiveHandler(const FpySequencer_NoOpDirective& directive, DirectiveError& error) {
    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushTlmVal_directiveHandler(const FpySequencer_PushTlmValDirective& directive,
                                                 DirectiveError& error) {
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        error = DirectiveError::TLM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }
    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    Fw::TlmValid valid = this->getTlmChan_out(0, directive.get_chanId(), tlmTime, tlmValue);

    if (valid != Fw::TlmValid::VALID) {
        // could not find this tlm chan
        error = DirectiveError::TLM_CHAN_NOT_FOUND;
        return Signal::stmtResponse_failure;
    }

    if (Fpy::MAX_STACK_SIZE - tlmValue.getSize() < this->m_runtime.stack.size) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    this->m_runtime.stack.push(tlmValue.getBuffAddr(), static_cast<Fpy::StackSizeType>(tlmValue.getSize()));
    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushTlmValAndTime_directiveHandler(const FpySequencer_PushTlmValAndTimeDirective& directive,
                                                        DirectiveError& error) {
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        error = DirectiveError::TLM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }

    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    Fw::TlmValid valid = this->getTlmChan_out(0, directive.get_chanId(), tlmTime, tlmValue);

    if (valid != Fw::TlmValid::VALID) {
        // could not find this tlm chan
        error = DirectiveError::TLM_CHAN_NOT_FOUND;
        return Signal::stmtResponse_failure;
    }

    U8 tlmTimeBuf[Fw::Time::SERIALIZED_SIZE] = {};
    Fw::ExternalSerializeBuffer timeEsb(tlmTimeBuf, Fw::Time::SERIALIZED_SIZE);
    Fw::SerializeStatus stat = timeEsb.serializeFrom(tlmTime);

    // coding error if this failed, we should have enough space
    FW_ASSERT(stat == Fw::SerializeStatus::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(stat));

    // check that our stack won't overflow if we put both val and time on it
    if (Fpy::MAX_STACK_SIZE - tlmValue.getSize() - timeEsb.getSize() < this->m_runtime.stack.size) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }

    // push tlm to end of stack
    this->m_runtime.stack.push(tlmValue.getBuffAddr(), static_cast<Fpy::StackSizeType>(tlmValue.getSize()));
    // now push time to end of stack
    this->m_runtime.stack.push(timeEsb.getBuffAddr(), static_cast<Fpy::StackSizeType>(timeEsb.getSize()));
    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushPrm_directiveHandler(const FpySequencer_PushPrmDirective& directive, DirectiveError& error) {
    if (!this->isConnected_prmGet_OutputPort(0)) {
        error = DirectiveError::PRM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }

    Fw::ParamBuffer prmValue;
    Fw::ParamValid valid = this->getParam_out(0, directive.get_prmId(), prmValue);

    if (valid != Fw::ParamValid::VALID) {
        // could not find this prm in the DB
        error = DirectiveError::PRM_NOT_FOUND;
        return Signal::stmtResponse_failure;
    }

    if (Fpy::MAX_STACK_SIZE - prmValue.getSize() < this->m_runtime.stack.size) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }

    this->m_runtime.stack.push(prmValue.getBuffAddr(), static_cast<Fpy::StackSizeType>(prmValue.getSize()));
    return Signal::stmtResponse_success;
}

Signal FpySequencer::constCmd_directiveHandler(const FpySequencer_ConstCmdDirective& directive, DirectiveError& error) {
    if (this->sendCmd(directive.get_opCode(), directive.get_argBuf(), directive.get__argBufSize()) ==
        Fw::Success::FAILURE) {
        return Signal::stmtResponse_failure;
    } else {
        // now tell the SM to wait some more until we get the cmd response back
        // if we've already got the response back this should be harmless
        return Signal::stmtResponse_keepWaiting;
    }
}

DirectiveError FpySequencer::op_or() {
    if (this->m_runtime.stack.size < sizeof(U8) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U8>(this->m_runtime.stack.pop<U8>() | this->m_runtime.stack.pop<U8>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_and() {
    if (this->m_runtime.stack.size < sizeof(U8) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U8>(this->m_runtime.stack.pop<U8>() & this->m_runtime.stack.pop<U8>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ieq() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U8>(this->m_runtime.stack.pop<I64>() == this->m_runtime.stack.pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ine() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U8>(this->m_runtime.stack.pop<I64>() != this->m_runtime.stack.pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ult() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    U64 lhs = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs < rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ule() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    U64 lhs = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs <= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ugt() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    U64 lhs = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs > rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_uge() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    U64 lhs = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs >= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_slt() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs < rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sle() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs <= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sgt() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs > rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sge() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    this->m_runtime.stack.push(static_cast<U8>(lhs >= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_feq() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    // eq is true if they are equal and neither is nan
    this->m_runtime.stack.push(static_cast<U8>((lhs == rhs) ? 1 : 0));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fne() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    // ne is true if they are not equal or either is nan
    this->m_runtime.stack.push(static_cast<U8>((lhs != rhs) ? 1 : 0));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_flt() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<U8>(std::isless(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fle() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<U8>(std::islessequal(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fgt() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<U8>(std::isgreater(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fge() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<U8>(std::isgreaterequal(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_not() {
    if (this->m_runtime.stack.size < sizeof(U8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U8>(this->m_runtime.stack.pop<U8>() == 0));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fpext() {
    // convert F32 to F64
    if (this->m_runtime.stack.size < sizeof(F32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<F64>(this->m_runtime.stack.pop<F32>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptrunc() {
    // convert F64 to F32
    if (this->m_runtime.stack.size < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<F32>(this->m_runtime.stack.pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptosi() {
    if (this->m_runtime.stack.size < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<I64>(this->m_runtime.stack.pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sitofp() {
    if (this->m_runtime.stack.size < sizeof(I64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<F64>(this->m_runtime.stack.pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptoui() {
    if (this->m_runtime.stack.size < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<U64>(this->m_runtime.stack.pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_uitofp() {
    if (this->m_runtime.stack.size < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->m_runtime.stack.push(static_cast<F64>(this->m_runtime.stack.pop<U64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_add() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    // Check for overflow and underflow and return the appropriate error code
    // Overflow can only occur with both operands positive and occurs when one operand is greater than the maximum value
    // less the other operand.  If either operand is negative or zero, overflow cannot occur.
    if ((rhs > 0) && (lhs > 0) && ((std::numeric_limits<I64>::max() - rhs) < lhs)) {
        return DirectiveError::ARITHMETIC_OVERFLOW;
    }
    // Underflow can only occur with both operands negative and occurs when one operand is less than the minimum value
    // minus the other operand.  If either operand is positive or zero, underflow cannot occur.
    else if ((rhs < 0) && (lhs < 0) && ((std::numeric_limits<I64>::min() - rhs) > lhs)) {
        return DirectiveError::ARITHMETIC_UNDERFLOW;
    }
    this->m_runtime.stack.push(static_cast<I64>(lhs + rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sub() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    // Check for overflow and underflow and return the appropriate error code
    // Overflow can only occur when the left operand is positive and the right operand is negative. It occurs when the
    // left (positive) operand is greater than the maximum value plus the other (negative) operand. If the right
    // operand is positive or zero, overflow cannot occur.
    if ((rhs < 0) && (lhs > 0) && ((std::numeric_limits<I64>::max() + rhs) < lhs)) {
        return DirectiveError::ARITHMETIC_OVERFLOW;
    }
    // Underflow can only occur when the left operand is negative and the right operand is positive. It occurs when the
    // left (negative) operand is less than the minimum value plus the other (positive) operand. If the right operand
    // is negative or zero, underflow cannot occur.
    else if ((rhs > 0) && (lhs < 0) && ((std::numeric_limits<I64>::min() + rhs) > lhs)) {
        return DirectiveError::ARITHMETIC_UNDERFLOW;
    }
    this->m_runtime.stack.push(static_cast<I64>(lhs - rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_mul() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    // Check for overflow and underflow and return the appropriate error code
    // Overflow can only occur with operands of matching signs and occurs when one operand is greater (or less) than the
    // maximum value divided by the other operand. Either operand being zero precludes overflow.
    // Check the both positive case.
    if ((rhs > 0) && (lhs > 0) && ((std::numeric_limits<I64>::max() / rhs) < lhs)) {
        return DirectiveError::ARITHMETIC_OVERFLOW;
    }
    // Check the both negative case
    else if ((rhs < 0) && (lhs < 0) && ((std::numeric_limits<I64>::max() / (-1 * rhs)) < (-1 * lhs))) {
        return DirectiveError::ARITHMETIC_OVERFLOW;
    }
    // Underflow can occur with operands of differing signs and occurs when one operand is less than the minimum value
    // divided by the other operand. Either operand being zero precludes underflow.
    // Check the case where lhs is positive.
    else if ((rhs < 0) && (lhs > 0) && ((std::numeric_limits<I64>::min() / lhs) > rhs)) {
        return DirectiveError::ARITHMETIC_UNDERFLOW;
    }
    // Check the case where rhs is positive.
    else if ((rhs > 0) && (lhs < 0) && ((std::numeric_limits<I64>::min() / rhs) > lhs)) {
        return DirectiveError::ARITHMETIC_UNDERFLOW;
    }
    this->m_runtime.stack.push(static_cast<I64>(lhs * rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_udiv() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    U64 lhs = this->m_runtime.stack.pop<U64>();
    // Prevent division by zero
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    this->m_runtime.stack.push(static_cast<U64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sdiv() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }

    I64 rhs = this->m_runtime.stack.pop<I64>();
    I64 lhs = this->m_runtime.stack.pop<I64>();
    // Prevent division by zero
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    this->m_runtime.stack.push(static_cast<I64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_umod() {
    if (this->m_runtime.stack.size < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->m_runtime.stack.pop<U64>();
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    U64 lhs = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U64>(lhs % rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_smod() {
    if (this->m_runtime.stack.size < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->m_runtime.stack.pop<I64>();
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    I64 lhs = this->m_runtime.stack.pop<I64>();
    I64 res = static_cast<I64>(lhs % rhs);
    // in order to match Python's behavior,
    // if the signs of the remainder and divisor differ, adjust the result.
    // this happens when the result should be positive but is negative, or vice-versa.
    // credit Gemini 2.5 pro
    if ((res > 0 && rhs < 0) || (res < 0 && rhs > 0)) {
        res += rhs;
    }
    this->m_runtime.stack.push(res);
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fadd() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(lhs + rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fsub() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(lhs - rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fmul() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(lhs * rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fdiv() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fpow() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(pow(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_flog() {
    if (this->m_runtime.stack.size < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 val = this->m_runtime.stack.pop<F64>();
    if (val <= 0.0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    this->m_runtime.stack.push(static_cast<F64>(log(val)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fmod() {
    if (this->m_runtime.stack.size < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->m_runtime.stack.pop<F64>();
    if (rhs == 0.0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    F64 lhs = this->m_runtime.stack.pop<F64>();
    this->m_runtime.stack.push(static_cast<F64>(lhs - rhs * std::floor(lhs / rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_8_64() {
    if (this->m_runtime.stack.size < sizeof(I8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I8 src = this->m_runtime.stack.pop<I8>();
    this->m_runtime.stack.push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_16_64() {
    if (this->m_runtime.stack.size < sizeof(I16)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I16 src = this->m_runtime.stack.pop<I16>();
    this->m_runtime.stack.push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_32_64() {
    if (this->m_runtime.stack.size < sizeof(I32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I32 src = this->m_runtime.stack.pop<I32>();
    this->m_runtime.stack.push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_8_64() {
    if (this->m_runtime.stack.size < sizeof(U8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U8 src = this->m_runtime.stack.pop<U8>();
    this->m_runtime.stack.push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_16_64() {
    if (this->m_runtime.stack.size < sizeof(U16)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U16 src = this->m_runtime.stack.pop<U16>();
    this->m_runtime.stack.push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_32_64() {
    if (this->m_runtime.stack.size < sizeof(U32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U32 src = this->m_runtime.stack.pop<U32>();
    this->m_runtime.stack.push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_8() {
    if (this->m_runtime.stack.size < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U8>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_16() {
    if (this->m_runtime.stack.size < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U16>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_32() {
    if (this->m_runtime.stack.size < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->m_runtime.stack.pop<U64>();
    this->m_runtime.stack.push(static_cast<U32>(src));
    return DirectiveError::NO_ERROR;
}
Signal FpySequencer::stackOp_directiveHandler(const FpySequencer_StackOpDirective& directive, DirectiveError& error) {
    // coding error, should not have gotten to this stack op handler
    FW_ASSERT(directive.get__op() >= Fpy::DirectiveId::OR && directive.get__op() <= Fpy::DirectiveId::ITRUNC_64_32,
              static_cast<FwAssertArgType>(directive.get__op()));

    switch (directive.get__op()) {
        case Fpy::DirectiveId::OR:
            error = this->op_or();
            break;
        case Fpy::DirectiveId::AND:
            error = this->op_and();
            break;
        case Fpy::DirectiveId::IEQ:
            error = this->op_ieq();
            break;
        case Fpy::DirectiveId::INE:
            error = this->op_ine();
            break;
        case Fpy::DirectiveId::ULT:
            error = this->op_ult();
            break;
        case Fpy::DirectiveId::ULE:
            error = this->op_ule();
            break;
        case Fpy::DirectiveId::UGT:
            error = this->op_ugt();
            break;
        case Fpy::DirectiveId::UGE:
            error = this->op_uge();
            break;
        case Fpy::DirectiveId::SLT:
            error = this->op_slt();
            break;
        case Fpy::DirectiveId::SLE:
            error = this->op_sle();
            break;
        case Fpy::DirectiveId::SGT:
            error = this->op_sgt();
            break;
        case Fpy::DirectiveId::SGE:
            error = this->op_sge();
            break;
        case Fpy::DirectiveId::FEQ:
            error = this->op_feq();
            break;
        case Fpy::DirectiveId::FNE:
            error = this->op_fne();
            break;
        case Fpy::DirectiveId::FLT:
            error = this->op_flt();
            break;
        case Fpy::DirectiveId::FLE:
            error = this->op_fle();
            break;
        case Fpy::DirectiveId::FGT:
            error = this->op_fgt();
            break;
        case Fpy::DirectiveId::FGE:
            error = this->op_fge();
            break;
        case Fpy::DirectiveId::NOT:
            error = this->op_not();
            break;
        case Fpy::DirectiveId::FPEXT:
            error = this->op_fpext();
            break;
        case Fpy::DirectiveId::FPTRUNC:
            error = this->op_fptrunc();
            break;
        case Fpy::DirectiveId::FPTOSI:
            error = this->op_fptosi();
            break;
        case Fpy::DirectiveId::FPTOUI:
            error = this->op_fptoui();
            break;
        case Fpy::DirectiveId::SITOFP:
            error = this->op_sitofp();
            break;
        case Fpy::DirectiveId::UITOFP:
            error = this->op_uitofp();
            break;
        case Fpy::DirectiveId::ADD:
            error = this->op_add();
            break;
        case Fpy::DirectiveId::SUB:
            error = this->op_sub();
            break;
        case Fpy::DirectiveId::MUL:
            error = this->op_mul();
            break;
        case Fpy::DirectiveId::UDIV:
            error = this->op_udiv();
            break;
        case Fpy::DirectiveId::SDIV:
            error = this->op_sdiv();
            break;
        case Fpy::DirectiveId::UMOD:
            error = this->op_umod();
            break;
        case Fpy::DirectiveId::SMOD:
            error = this->op_smod();
            break;
        case Fpy::DirectiveId::FADD:
            error = this->op_fadd();
            break;
        case Fpy::DirectiveId::FSUB:
            error = this->op_fsub();
            break;
        case Fpy::DirectiveId::FMUL:
            error = this->op_fmul();
            break;
        case Fpy::DirectiveId::FDIV:
            error = this->op_fdiv();
            break;
        case Fpy::DirectiveId::FPOW:
            error = this->op_fpow();
            break;
        case Fpy::DirectiveId::FLOG:
            error = this->op_flog();
            break;
        case Fpy::DirectiveId::FMOD:
            error = this->op_fmod();
            break;
        case Fpy::DirectiveId::SIEXT_8_64:
            error = this->op_siext_8_64();
            break;
        case Fpy::DirectiveId::SIEXT_16_64:
            error = this->op_siext_16_64();
            break;
        case Fpy::DirectiveId::SIEXT_32_64:
            error = this->op_siext_32_64();
            break;
        case Fpy::DirectiveId::ZIEXT_8_64:
            error = this->op_ziext_8_64();
            break;
        case Fpy::DirectiveId::ZIEXT_16_64:
            error = this->op_ziext_16_64();
            break;
        case Fpy::DirectiveId::ZIEXT_32_64:
            error = this->op_ziext_32_64();
            break;
        case Fpy::DirectiveId::ITRUNC_64_8:
            error = this->op_itrunc_64_8();
            break;
        case Fpy::DirectiveId::ITRUNC_64_16:
            error = this->op_itrunc_64_16();
            break;
        case Fpy::DirectiveId::ITRUNC_64_32:
            error = this->op_itrunc_64_32();
            break;
        default:
            FW_ASSERT(0, directive.get__op());
            break;
    }
    if (error != DirectiveError::NO_ERROR) {
        return Signal::stmtResponse_failure;
    }
    return Signal::stmtResponse_success;
}

Signal FpySequencer::exit_directiveHandler(const FpySequencer_ExitDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < 1) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    U8 errorCode = this->m_runtime.stack.pop<U8>();
    // exit(0), no error
    if (errorCode == 0) {
        // just goto the end of the sequence
        this->m_runtime.nextStatementIndex = this->m_sequenceObj.get_header().get_statementCount();
        return Signal::stmtResponse_success;
    }
    // otherwise, kill the sequence here
    // raise the user defined error code as an event
    this->log_WARNING_HI_SequenceExitedWithError(this->m_sequenceFilePath, errorCode);
    error = DirectiveError::EXIT_WITH_ERROR;
    return Signal::stmtResponse_failure;
}

Signal FpySequencer::allocate_directiveHandler(const FpySequencer_AllocateDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size + directive.get_size() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    this->m_runtime.stack.pushZeroes(directive.get_size());
    return Signal::stmtResponse_success;
}

Signal FpySequencer::storeConstOffset_directiveHandler(const FpySequencer_StoreConstOffsetDirective& directive,
                                                       DirectiveError& error) {
    if (this->m_runtime.stack.size < directive.get_size()) {
        // not enough bytes to pop
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Fpy::StackSizeType stackOffset = this->m_runtime.stack.lvarOffset() + directive.get_lvarOffset();
    // if we popped these bytes off, and put them in lvar array, would we go out of bounds
    if (stackOffset + directive.get_size() > this->m_runtime.stack.size - directive.get_size()) {
        // write into lvar array would go out of bounds
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // i believe we can be sure the regions are not overlapping, due to the above check
    memcpy(this->m_runtime.stack.bytes + stackOffset, this->m_runtime.stack.top() - directive.get_size(),
           directive.get_size());
    this->m_runtime.stack.size -= directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::load_directiveHandler(const FpySequencer_LoadDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size + directive.get_size() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    // calculate the offset in the lvar array we're going to pull from
    Fpy::StackSizeType stackOffset = this->m_runtime.stack.lvarOffset() + directive.get_lvarOffset();
    // if we accessed these bytes, would we go out of bounds
    if (stackOffset + directive.get_size() > this->m_runtime.stack.size) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // copy from lvar array to top of stack, add to stack size.
    memcpy(this->m_runtime.stack.top(), this->m_runtime.stack.bytes + stackOffset, directive.get_size());
    this->m_runtime.stack.size += directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushVal_directiveHandler(const FpySequencer_PushValDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size + directive.get__valSize() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    // copy from the bytearray in the directive to the stack, add to stack size.
    memcpy(this->m_runtime.stack.top(), directive.get_val(), directive.get__valSize());
    this->m_runtime.stack.size += static_cast<Fpy::StackSizeType>(directive.get__valSize());
    return Signal::stmtResponse_success;
}

Signal FpySequencer::discard_directiveHandler(const FpySequencer_DiscardDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < directive.get_size()) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // drop the specified amount of bytes off the stack. simple as.
    this->m_runtime.stack.size -= directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::memCmp_directiveHandler(const FpySequencer_MemCmpDirective& directive, DirectiveError& error) {
    // we are going to pop 2x the size off the stack. check that we can
    if (this->m_runtime.stack.size < directive.get_size() * 2) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // find the starting offsets of the two byte arrays
    U64 lhsOffset = this->m_runtime.stack.size - directive.get_size() * 2;
    U64 rhsOffset = this->m_runtime.stack.size - directive.get_size();

    // "officially" remove them from the stack
    // you have to do this before pushing to the stack, otherwise the result would get placed
    // after the byte arrays
    this->m_runtime.stack.size -= directive.get_size() * 2;

    // memcmp the two byte arrays, push 1 if they were equal, 0 otherwise
    if (memcmp(this->m_runtime.stack.bytes + lhsOffset, this->m_runtime.stack.bytes + rhsOffset,
               directive.get_size()) == 0) {
        this->m_runtime.stack.push<U8>(1);
    } else {
        this->m_runtime.stack.push<U8>(0);
    }
    return Signal::stmtResponse_success;
}

Signal FpySequencer::stackCmd_directiveHandler(const FpySequencer_StackCmdDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < static_cast<U64>(directive.get_argsSize() + sizeof(FwOpcodeType))) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // pop the opcode of the cmd off the stack
    // note this means that, unlike the actual byte array that the dispatcher gets,
    // these cmds have opcode after the argument buffer
    FwOpcodeType opcode = this->m_runtime.stack.pop<FwOpcodeType>();
    U64 argBufOffset = this->m_runtime.stack.size - directive.get_argsSize();

    // update the opcode of the cmd we will await
    this->m_runtime.currentCmdOpcode = opcode;

    // also pop the args off the stack
    this->m_runtime.stack.size -= directive.get_argsSize();

    if (this->sendCmd(opcode, this->m_runtime.stack.bytes + argBufOffset, directive.get_argsSize()) ==
        Fw::Success::FAILURE) {
        return Signal::stmtResponse_failure;
    } else {
        // now tell the SM to wait some more until we get the cmd response back
        // if we've already got the response back this should be harmless
        return Signal::stmtResponse_keepWaiting;
    }

    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushTime_directiveHandler(const FpySequencer_PushTimeDirective& directive, DirectiveError& error) {
    if (Fpy::MAX_STACK_SIZE - Fw::Time::SERIALIZED_SIZE < this->m_runtime.stack.size) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }

    Fw::Time currentTime = this->getTime();

    U8 currentTimeBuf[Fw::Time::SERIALIZED_SIZE] = {};
    Fw::ExternalSerializeBuffer timeEsb(currentTimeBuf, Fw::Time::SERIALIZED_SIZE);
    Fw::SerializeStatus stat = timeEsb.serializeFrom(currentTime);

    // coding error if this failed, we should have enough space
    FW_ASSERT(stat == Fw::SerializeStatus::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(stat));

    // push time to end of stack
    memcpy(this->m_runtime.stack.bytes + this->m_runtime.stack.size, timeEsb.getBuffAddr(), timeEsb.getSize());
    this->m_runtime.stack.size += static_cast<Fpy::StackSizeType>(timeEsb.getSize());
    return Signal::stmtResponse_success;
}

Signal FpySequencer::setFlag_directiveHandler(const FpySequencer_SetFlagDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < 1) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (directive.get_flagIdx() >= Fpy::FLAG_COUNT) {
        error = DirectiveError::FLAG_IDX_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // 1 if the stack bool is nonzero, 0 otherwise
    U8 flagVal = this->m_runtime.stack.pop<U8>() != 0;

    this->m_runtime.flags[directive.get_flagIdx()] = flagVal == 1;
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getFlag_directiveHandler(const FpySequencer_GetFlagDirective& directive, DirectiveError& error) {
    if (Fpy::MAX_STACK_SIZE - this->m_runtime.stack.size < 1) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    if (directive.get_flagIdx() >= Fpy::FLAG_COUNT) {
        error = DirectiveError::FLAG_IDX_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    bool flagVal = this->m_runtime.flags[directive.get_flagIdx()];
    this->m_runtime.stack.push<U8>(flagVal);
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getField_directiveHandler(const FpySequencer_GetFieldDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < sizeof(Fpy::StackSizeType) ||
        this->m_runtime.stack.size < directive.get_parentSize()) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    Fpy::StackSizeType offset = this->m_runtime.stack.pop<Fpy::StackSizeType>();

    if (offset + directive.get_memberSize() > directive.get_parentSize()) {
        // i think it's somewhat ambiguous whether this is a stack access out of bounds
        // but there isn't really an error code that better reflects this, and i guess
        // it's technically true
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    // the resulting bytes should move to the start of the parent array

    // get pointer to the start of the parent
    U8* parentStartPtr = this->m_runtime.stack.top() - directive.get_parentSize();
    // move the field bytes to the start of the parent
    memmove(parentStartPtr, parentStartPtr + offset, directive.get_memberSize());
    // adjust stack size by the diff between the member and the parent
    this->m_runtime.stack.size -= (directive.get_parentSize() - directive.get_memberSize());
    return Signal::stmtResponse_success;
}

Signal FpySequencer::peek_directiveHandler(const FpySequencer_PeekDirective& directive, DirectiveError& error) {
    // must have at least two StackSizeType on stack
    if (this->m_runtime.stack.size < sizeof(Fpy::StackSizeType) * 2) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    Fpy::StackSizeType offset = this->m_runtime.stack.pop<Fpy::StackSizeType>();
    if (offset > this->m_runtime.stack.size) {
        // would access past the bottom of the stack
        // note we allow the equals case because the byteCount might be 0
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Fpy::StackSizeType byteCount = this->m_runtime.stack.pop<Fpy::StackSizeType>();
    if (Fpy::MAX_STACK_SIZE - this->m_runtime.stack.size < byteCount) {
        // we would overflow the stack if we pushed this many bytes to it
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    if (this->m_runtime.stack.size < byteCount + offset) {
        // would access past the bottom of the stack
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // start copying from the lowest byte of the src array
    U8* src = this->m_runtime.stack.top() - offset - byteCount;
    this->m_runtime.stack.push(src, byteCount);
    return Signal::stmtResponse_success;
}

Signal FpySequencer::store_directiveHandler(const FpySequencer_StoreDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stack.size < directive.get_size() + sizeof(Fpy::StackSizeType)) {
        // not enough bytes to pop the value and the stack offset
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Fpy::StackSizeType stackOffset =
        this->m_runtime.stack.lvarOffset() + this->m_runtime.stack.pop<Fpy::StackSizeType>();
    // if we popped these bytes off, and put them in lvar array, would we go out of bounds
    if (stackOffset + directive.get_size() > this->m_runtime.stack.size - directive.get_size()) {
        // write into lvar array would go out of bounds
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // i believe we can be sure the regions are not overlapping, due to the above check
    memcpy(this->m_runtime.stack.bytes + stackOffset, this->m_runtime.stack.top() - directive.get_size(),
           directive.get_size());
    this->m_runtime.stack.size -= directive.get_size();
    return Signal::stmtResponse_success;
}

}  // namespace Svc
