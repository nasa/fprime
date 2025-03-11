#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc {

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const Svc::FpySequencer_WaitRelDirective& directive) {
    Fw::Time wakeupTime = getTime();

    wakeupTime.add(directive.getduration().getSeconds(), directive.getduration().getUSeconds());
    this->sequencer_sendSignal_directiveResponse_beginSleep(wakeupTime);
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const Svc::FpySequencer_WaitAbsDirective& directive) {
    this->sequencer_sendSignal_directiveResponse_beginSleep(directive.getwakeupTime());
}
}  // namespace Svc