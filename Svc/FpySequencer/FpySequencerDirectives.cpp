#include <cmath>
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

Fw::Success FpySequencer::sendCmd(FwOpcodeType opcode, const U8* argBuf, FwSizeType argBufSize) {
    Fw::ComBuffer cmdBuf;
    Fw::SerializeStatus stat =
        cmdBuf.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
    // TODO should I assert here? this really shouldn't fail, I should just add a static assert
    // on com buf size and then assert here
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serialize(opcode);
    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        return Fw::Success::FAILURE;
    }
    stat = cmdBuf.serialize(argBuf, argBufSize, Fw::Serialization::OMIT_LENGTH);
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

template <typename T>
T FpySequencer::pop() {
    static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "size must be 1, 2, 4, 8");
    FW_ASSERT(this->m_runtime.stackSize >= sizeof(T), static_cast<FwAssertArgType>(this->m_runtime.stackSize),
              static_cast<FwAssertArgType>(sizeof(T)));
    // first make a byte array which can definitely store our val
    U8 valBytes[8] = {0};
    // now move top of stack into byte array and shrink stack
    memcpy(valBytes, this->top() - sizeof(T), sizeof(T));
    this->m_runtime.stackSize -= sizeof(T);

    // now do appropriate byteswap on byte array
    if (sizeof(T) == 8) {
        return static_cast<T>((static_cast<T>(valBytes[7]) << 0) | (static_cast<T>(valBytes[6]) << 8) |
                              (static_cast<T>(valBytes[5]) << 16) | (static_cast<T>(valBytes[4]) << 24) |
                              (static_cast<T>(valBytes[3]) << 32) | (static_cast<T>(valBytes[2]) << 40) |
                              (static_cast<T>(valBytes[1]) << 48) | (static_cast<T>(valBytes[0]) << 56));
    } else if (sizeof(T) == 4) {
        return static_cast<T>((static_cast<T>(valBytes[3]) << 0) | (static_cast<T>(valBytes[2]) << 8) |
                              (static_cast<T>(valBytes[1]) << 16) | (static_cast<T>(valBytes[0]) << 24));
    } else if (sizeof(T) == 2) {
        return static_cast<T>((static_cast<T>(valBytes[1]) << 0) | (static_cast<T>(valBytes[0]) << 8));
    } else {
        return static_cast<T>(valBytes[0]);
    }
}

template U8 FpySequencer::pop();
template U16 FpySequencer::pop();
template U32 FpySequencer::pop();
template U64 FpySequencer::pop();
template I8 FpySequencer::pop();
template I16 FpySequencer::pop();
template I32 FpySequencer::pop();
template I64 FpySequencer::pop();

template <>
F32 FpySequencer::pop<F32>() {
    U32 endianness = this->pop<U32>();
    F32 val;
    memcpy(&val, &endianness, sizeof(val));
    return val;
}

template <>
F64 FpySequencer::pop<F64>() {
    U64 endianness = this->pop<U64>();
    F64 val;
    memcpy(&val, &endianness, sizeof(val));
    return val;
}

template <typename T>
void FpySequencer::push(T val) {
    static_assert(sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1, "size must be 1, 2, 4, 8");
    FW_ASSERT(this->m_runtime.stackSize + sizeof(val) < Fpy::MAX_STACK_SIZE,
              static_cast<FwAssertArgType>(this->m_runtime.stackSize), static_cast<FwAssertArgType>(sizeof(T)));
    // first make a byte array which can definitely store our val
    U8 valBytes[8] = {0};
    // convert val to unsigned to avoid undefined behavior for bitshifts of signed types
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT valUnsigned = static_cast<UnsignedT>(val);
    if (sizeof(T) == 8) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 56);
        valBytes[1] = static_cast<U8>(valUnsigned >> 48);
        valBytes[2] = static_cast<U8>(valUnsigned >> 40);
        valBytes[3] = static_cast<U8>(valUnsigned >> 32);
        valBytes[4] = static_cast<U8>(valUnsigned >> 24);
        valBytes[5] = static_cast<U8>(valUnsigned >> 16);
        valBytes[6] = static_cast<U8>(valUnsigned >> 8);
        valBytes[7] = static_cast<U8>(valUnsigned >> 0);
    } else if (sizeof(T) == 4) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 24);
        valBytes[1] = static_cast<U8>(valUnsigned >> 16);
        valBytes[2] = static_cast<U8>(valUnsigned >> 8);
        valBytes[3] = static_cast<U8>(valUnsigned >> 0);
    } else if (sizeof(T) == 2) {
        valBytes[0] = static_cast<U8>(valUnsigned >> 8);
        valBytes[1] = static_cast<U8>(valUnsigned >> 0);
    } else {
        valBytes[0] = static_cast<U8>(valUnsigned);
    }
    memcpy(this->top(), valBytes, sizeof(T));
    this->m_runtime.stackSize += sizeof(T);
}

template void FpySequencer::push(U8);
template void FpySequencer::push(U16);
template void FpySequencer::push(U32);
template void FpySequencer::push(U64);
template void FpySequencer::push(I8);
template void FpySequencer::push(I16);
template void FpySequencer::push(I32);
template void FpySequencer::push(I64);

template <>
void FpySequencer::push<F32>(F32 val) {
    U32 endianness;
    memcpy(&endianness, &val, sizeof(val));
    this->push(endianness);
}

template <>
void FpySequencer::push<F64>(F64 val) {
    U64 endianness;
    memcpy(&endianness, &val, sizeof(val));
    this->push(endianness);
}

U8* FpySequencer::top() {
    return &this->m_runtime.stack[this->m_runtime.stackSize];
}

U8* FpySequencer::lvars() {
    return this->m_runtime.stack + this->lvarOffset();
}

U16 FpySequencer::lvarOffset() {
    // at the moment, because we only have one stack frame,
    // lvars always start at 0
    return 0;
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

//! Internal interface handler for directive_storeTlmVal
void FpySequencer::directive_storeTlmVal_internalInterfaceHandler(
    const Svc::FpySequencer_StoreTlmValDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->storeTlmVal_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_storePrm
void FpySequencer::directive_storePrm_internalInterfaceHandler(const Svc::FpySequencer_StorePrmDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->storePrm_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_constCmd
void FpySequencer::directive_constCmd_internalInterfaceHandler(const Svc::FpySequencer_ConstCmdDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->constCmd_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_stackOp
void FpySequencer::directive_stackOp_internalInterfaceHandler(const Svc::FpySequencer_StackOpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->stackOp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_exit
void FpySequencer::directive_exit_internalInterfaceHandler(const Svc::FpySequencer_ExitDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->exit_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_allocate
void FpySequencer::directive_allocate_internalInterfaceHandler(const Svc::FpySequencer_AllocateDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->allocate_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_store
void FpySequencer::directive_store_internalInterfaceHandler(const Svc::FpySequencer_StoreDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->store_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_pushVal
void FpySequencer::directive_pushVal_internalInterfaceHandler(const Svc::FpySequencer_PushValDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->pushVal_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_load
void FpySequencer::directive_load_internalInterfaceHandler(const Svc::FpySequencer_LoadDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->load_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_discard
void FpySequencer::directive_discard_internalInterfaceHandler(const Svc::FpySequencer_DiscardDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->discard_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_memCmp
void FpySequencer::directive_memCmp_internalInterfaceHandler(const Svc::FpySequencer_MemCmpDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->memCmp_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_stackCmd
void FpySequencer::directive_stackCmd_internalInterfaceHandler(const Svc::FpySequencer_StackCmdDirective& directive) {
    DirectiveError error = DirectiveError::NO_ERROR;
    this->sendSignal(this->stackCmd_directiveHandler(directive, error));
    this->m_tlm.lastDirectiveError = error;
}

//! Internal interface handler for directive_waitRel
Signal FpySequencer::waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < 8) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    Fw::Time wakeupTime = this->getTime();

    U32 uSeconds = this->pop<U32>();
    U32 seconds = this->pop<U32>();

    wakeupTime.add(seconds, uSeconds);
    this->m_runtime.wakeupTime = wakeupTime;
    return Signal::stmtResponse_beginSleep;
}

//! Internal interface handler for directive_waitAbs
Signal FpySequencer::waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < 10 + sizeof(FwTimeContextStoreType)) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    U32 uSeconds = this->pop<U32>();
    U32 seconds = this->pop<U32>();
    FwTimeContextStoreType ctx = this->pop<FwTimeContextStoreType>();
    U16 base = this->pop<U16>();

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
    if (this->m_runtime.stackSize < 1) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // check within sequence bounds, or at EOF (we allow == case cuz this just ends the sequence)
    if (directive.get_falseGotoStmtIndex() > m_sequenceObj.get_header().get_statementCount()) {
        error = DirectiveError::STMT_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    if (this->pop<U8>() != 0) {
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

Signal FpySequencer::storeTlmVal_directiveHandler(const FpySequencer_StoreTlmValDirective& directive,
                                                  DirectiveError& error) {
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        error = DirectiveError::TLM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }
    U32 stackOffset = this->lvarOffset() + directive.get_lvarOffset();
    if (stackOffset >= this->m_runtime.stackSize) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
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

    // if we were to write this buf at this offset
    // would it go over the current size of the stack (NOT the max size b/c
    // we aren't supposed to add anything to the stack when we store)

    if (stackOffset + tlmValue.getBuffLength() > this->m_runtime.stackSize) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    memcpy(this->m_runtime.stack + stackOffset, tlmValue.getBuffAddr(), tlmValue.getBuffLength());
    return Signal::stmtResponse_success;
}

Signal FpySequencer::storePrm_directiveHandler(const FpySequencer_StorePrmDirective& directive, DirectiveError& error) {
    if (!this->isConnected_prmGet_OutputPort(0)) {
        error = DirectiveError::PRM_GET_NOT_CONNECTED;
        return Signal::stmtResponse_failure;
    }
    U32 stackOffset = this->lvarOffset() + directive.get_lvarOffset();
    if (stackOffset >= this->m_runtime.stackSize) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    Fw::ParamBuffer prmValue;
    Fw::ParamValid valid = this->getParam_out(0, directive.get_prmId(), prmValue);

    if (valid != Fw::ParamValid::VALID) {
        // could not find this prm in the DB
        error = DirectiveError::PRM_NOT_FOUND;
        return Signal::stmtResponse_failure;
    }

    // if we were to write this buf at this offset
    // would it overflow the current size of the stack (NOT the max size b/c
    // we aren't supposed to add anything to the stack when we store)

    if (stackOffset + prmValue.getBuffLength() > this->m_runtime.stackSize) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    memcpy(this->m_runtime.stack + stackOffset, prmValue.getBuffAddr(), prmValue.getBuffLength());
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

DirectiveError FpySequencer::op_or() {
    if (this->m_runtime.stackSize < sizeof(U8) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U8>(this->pop<U8>() | this->pop<U8>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_and() {
    if (this->m_runtime.stackSize < sizeof(U8) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U8>(this->pop<U8>() & this->pop<U8>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ieq() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U8>(this->pop<I64>() == this->pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ine() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U8>(this->pop<I64>() != this->pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ult() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U8>(lhs < rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ule() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U8>(lhs <= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ugt() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U8>(lhs > rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_uge() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U8>(lhs >= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_slt() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<U8>(lhs < rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sle() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<U8>(lhs <= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sgt() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<U8>(lhs > rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sge() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<U8>(lhs >= rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_feq() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    // eq is true if they are equal and neither is nan
    this->push(static_cast<U8>(floatCmp(lhs, rhs) == 0));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fne() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    I8 cmp = floatCmp(lhs, rhs);
    // ne is true if they are not equal and neither is nan
    this->push(static_cast<U8>(cmp != 0 && cmp != -2));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_flt() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<U8>(floatCmp(lhs, rhs) == -1));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fle() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    I8 cmp = floatCmp(lhs, rhs);
    this->push(static_cast<U8>(cmp == 0 || cmp == -1));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fgt() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<U8>(floatCmp(lhs, rhs) == 1));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fge() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    I8 cmp = floatCmp(lhs, rhs);
    this->push(static_cast<U8>(cmp == 0 || cmp == 1));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_not() {
    if (this->m_runtime.stackSize < sizeof(U8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U8>(this->pop<U8>() == 0));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fpext() {
    // convert F32 to F64
    if (this->m_runtime.stackSize < sizeof(F32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<F64>(this->pop<F32>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptrunc() {
    // convert F64 to F32
    if (this->m_runtime.stackSize < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<F32>(this->pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptosi() {
    if (this->m_runtime.stackSize < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<I64>(this->pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sitofp() {
    if (this->m_runtime.stackSize < sizeof(I64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<F64>(this->pop<I64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fptoui() {
    if (this->m_runtime.stackSize < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<U64>(this->pop<F64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_uitofp() {
    if (this->m_runtime.stackSize < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    this->push(static_cast<F64>(this->pop<U64>()));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_iadd() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<I64>(lhs + rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_isub() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<I64>(lhs - rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_imul() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<I64>(lhs * rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_udiv() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_sdiv() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    I64 lhs = this->pop<I64>();
    this->push(static_cast<I64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_umod() {
    if (this->m_runtime.stackSize < sizeof(U64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 rhs = this->pop<U64>();
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    U64 lhs = this->pop<U64>();
    this->push(static_cast<U64>(lhs % rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_smod() {
    if (this->m_runtime.stackSize < sizeof(I64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I64 rhs = this->pop<I64>();
    if (rhs == 0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    I64 lhs = this->pop<I64>();
    I64 res = static_cast<I64>(lhs % rhs);
    // in order to match Python's behavior,
    // if the signs of the remainder and divisor differ, adjust the result.
    // this happens when the result should be positive but is negative, or vice-versa.
    // credit Gemini 2.5 pro
    if ((res > 0 && rhs < 0) || (res < 0 && rhs > 0)) {
        res += rhs;
    }
    this->push(res);
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fadd() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(lhs + rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fsub() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(lhs - rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fmul() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(lhs * rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fdiv() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(lhs / rhs));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_float_floor_div() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(floor(lhs / rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fpow() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(pow(lhs, rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_flog() {
    if (this->m_runtime.stackSize < sizeof(F64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 val = this->pop<F64>();
    this->push(static_cast<F64>(log(val)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_fmod() {
    if (this->m_runtime.stackSize < sizeof(F64) * 2) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    F64 rhs = this->pop<F64>();
    if (rhs == 0.0) {
        return DirectiveError::DOMAIN_ERROR;
    }
    F64 lhs = this->pop<F64>();
    this->push(static_cast<F64>(lhs - rhs * std::floor(lhs / rhs)));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_8_64() {
    if (this->m_runtime.stackSize < sizeof(I8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I8 src = this->pop<I8>();
    this->push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_16_64() {
    if (this->m_runtime.stackSize < sizeof(I16)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I16 src = this->pop<I16>();
    this->push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_siext_32_64() {
    if (this->m_runtime.stackSize < sizeof(I32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    I32 src = this->pop<I32>();
    this->push(static_cast<I64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_8_64() {
    if (this->m_runtime.stackSize < sizeof(U8)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U8 src = this->pop<U8>();
    this->push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_16_64() {
    if (this->m_runtime.stackSize < sizeof(U16)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U16 src = this->pop<U16>();
    this->push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_ziext_32_64() {
    if (this->m_runtime.stackSize < sizeof(U32)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U32 src = this->pop<U32>();
    this->push(static_cast<U64>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_8() {
    if (this->m_runtime.stackSize < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->pop<U64>();
    this->push(static_cast<U8>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_16() {
    if (this->m_runtime.stackSize < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->pop<U64>();
    this->push(static_cast<U16>(src));
    return DirectiveError::NO_ERROR;
}
DirectiveError FpySequencer::op_itrunc_64_32() {
    if (this->m_runtime.stackSize < sizeof(U64)) {
        return DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
    }
    U64 src = this->pop<U64>();
    this->push(static_cast<U32>(src));
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
        case Fpy::DirectiveId::IADD:
            error = this->op_iadd();
            break;
        case Fpy::DirectiveId::ISUB:
            error = this->op_isub();
            break;
        case Fpy::DirectiveId::IMUL:
            error = this->op_imul();
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
        case Fpy::DirectiveId::FLOAT_FLOOR_DIV:
            error = this->op_float_floor_div();
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
    if (this->m_runtime.stackSize < 1) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    if (this->pop<U8>() != 0) {
        // just goto the end of the sequence
        this->m_runtime.nextStatementIndex = this->m_sequenceObj.get_header().get_statementCount();
        return Signal::stmtResponse_success;
    }
    // otherwise, kill the sequence here
    error = DirectiveError::DELIBERATE_FAILURE;
    return Signal::stmtResponse_failure;
}

Signal FpySequencer::allocate_directiveHandler(const FpySequencer_AllocateDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize + directive.get_size() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    // starting from the top, set n bytes to 0
    memset(this->top(), 0, directive.get_size());
    this->m_runtime.stackSize += directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::store_directiveHandler(const FpySequencer_StoreDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < directive.get_size()) {
        // not enough bytes to pop
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    U32 stackOffset = this->lvarOffset() + directive.get_lvarOffset();
    // if we popped these bytes off, and put them in lvar array, would we go out of bounds
    if (stackOffset + directive.get_size() > this->m_runtime.stackSize - directive.get_size()) {
        // write into lvar array would go out of bounds
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    // i believe we can be sure the regions are not overlapping, due to the above check
    memcpy(this->m_runtime.stack + stackOffset, this->top() - directive.get_size(), directive.get_size());
    this->m_runtime.stackSize -= directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::load_directiveHandler(const FpySequencer_LoadDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize + directive.get_size() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    U32 stackOffset = this->lvarOffset() + directive.get_lvarOffset();
    // if we accessed these bytes, would we go out of bounds
    if (stackOffset + directive.get_size() > this->m_runtime.stackSize) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    memcpy(this->top(), this->m_runtime.stack + stackOffset, directive.get_size());
    this->m_runtime.stackSize += directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::pushVal_directiveHandler(const FpySequencer_PushValDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize + directive.get__valSize() > Fpy::MAX_STACK_SIZE) {
        error = DirectiveError::STACK_OVERFLOW;
        return Signal::stmtResponse_failure;
    }
    memcpy(this->top(), directive.get_val(), directive.get__valSize());
    this->m_runtime.stackSize += directive.get__valSize();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::discard_directiveHandler(const FpySequencer_DiscardDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < directive.get_size()) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }
    this->m_runtime.stackSize -= directive.get_size();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::memCmp_directiveHandler(const FpySequencer_MemCmpDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < directive.get_size() * 2) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    U64 lhsOffset = this->m_runtime.stackSize - directive.get_size() * 2;
    U64 rhsOffset = this->m_runtime.stackSize - directive.get_size();
    this->m_runtime.stackSize -= directive.get_size() * 2;

    if (memcmp(this->m_runtime.stack + lhsOffset, this->m_runtime.stack + rhsOffset, directive.get_size()) == 0) {
        this->push<U8>(1);
    } else {
        this->push<U8>(0);
    }
    return Signal::stmtResponse_success;
}

Signal FpySequencer::stackCmd_directiveHandler(const FpySequencer_StackCmdDirective& directive, DirectiveError& error) {
    if (this->m_runtime.stackSize < static_cast<U64>(directive.get_argsSize() + sizeof(FwOpcodeType))) {
        error = DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS;
        return Signal::stmtResponse_failure;
    }

    FwOpcodeType opcode = this->pop<FwOpcodeType>();
    U64 argBufOffset = this->m_runtime.stackSize - directive.get_argsSize();

    // update the opcode of the cmd we will await
    this->m_runtime.currentCmdOpcode = opcode;

    // also pop the args off the stack
    this->m_runtime.stackSize -= directive.get_argsSize();

    if (this->sendCmd(opcode, this->m_runtime.stack + argBufOffset, directive.get_argsSize()) == Fw::Success::FAILURE) {
        return Signal::stmtResponse_failure;
    } else {
        // now tell the SM to wait some more until we get the cmd response back
        // if we've already got the response back this should be harmless
        return Signal::stmtResponse_keepWaiting;
    }

    return Signal::stmtResponse_success;
}
}  // namespace Svc
