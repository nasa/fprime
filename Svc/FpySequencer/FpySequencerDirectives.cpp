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

//! Internal interface handler for directive_getTlmTime
void FpySequencer::directive_getTlmTime_internalInterfaceHandler(
    const Svc::FpySequencer_GetTlmTimeDirective& directive) {
    this->sendSignal(this->getTlmTime_directiveHandler(directive));
}

//! Internal interface handler for directive_getTlmValue
void FpySequencer::directive_getTlmValue_internalInterfaceHandler(
    const Svc::FpySequencer_GetTlmValueDirective& directive) {
    this->sendSignal(this->getTlmValue_directiveHandler(directive));
}

//! Internal interface handler for directive_getPrmValue
void FpySequencer::directive_getPrmValue_internalInterfaceHandler(
    const Svc::FpySequencer_GetPrmValueDirective& directive) {
    this->sendSignal(this->getPrmValue_directiveHandler(directive));
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

Signal FpySequencer::getTlmTime_directiveHandler(const FpySequencer_GetTlmTimeDirective& directive) {
    if (directive.getdestLvarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        return Signal::stmtResponse_failure;
    }
    if (static_cast<U64>(Fw::Time::SERIALIZED_SIZE) > static_cast<U64>(Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE)) {
        // cannot store the tlm time in the lvar
        return Signal::stmtResponse_failure;
    }
    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    this->getTlmChan_out(0, directive.getchanId(), tlmTime, tlmValue);

    if (tlmValue.getBuffLength() == 0) {
        // could not find this tlm chan
        // impl of port says to set buf len to 0 in this case
        return Signal::stmtResponse_failure;
    }

    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getdestLvarIndex()];
    // clear the lvar in case of early return
    lvar.valueSize = 0;
    Fw::ExternalSerializeBuffer esb(lvar.value, Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE);
    Fw::SerializeStatus stat = esb.serialize(tlmTime);

    if (stat != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        // failed to serialize Fw::Time into the lvar
        return Signal::stmtResponse_failure;
    }

    lvar.valueSize = esb.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getTlmValue_directiveHandler(const FpySequencer_GetTlmValueDirective& directive) {
    if (directive.getdestLvarIndex() >= Fpy::MAX_SEQUENCE_LOCAL_VARIABLES) {
        return Signal::stmtResponse_failure;
    }
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        return Signal::stmtResponse_failure;
    }
    Fw::Time tlmTime;
    Fw::TlmBuffer tlmValue;
    this->getTlmChan_out(0, directive.getchanId(), tlmTime, tlmValue);

    if (tlmValue.getBuffLength() == 0) {
        // could not find this tlm chan
        // impl of port says to set buf len to 0 in this case
        return Signal::stmtResponse_failure;
    }

    if (tlmValue.getBuffLength() > Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE) {
        // cannot store the tlm value in the lvar
        return Signal::stmtResponse_failure;
    }
    // copy value into lvar
    Runtime::LocalVariable& lvar = this->m_runtime.localVariables[directive.getdestLvarIndex()];
    memcpy(lvar.value, tlmValue.getBuffAddr(), tlmValue.getBuffLength());
    lvar.valueSize = tlmValue.getBuffLength();
    return Signal::stmtResponse_success;
}

Signal FpySequencer::getPrmValue_directiveHandler(const FpySequencer_GetPrmValueDirective& directive) {
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
    memcpy(lvar.value, prmValue.getBuffAddr(), prmValue.getBuffLength());
    lvar.valueSize = prmValue.getBuffLength();
    return Signal::stmtResponse_success;
}
}  // namespace Svc
