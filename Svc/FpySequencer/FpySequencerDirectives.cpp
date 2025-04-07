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

}  // namespace Svc
