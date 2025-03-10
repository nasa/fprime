#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc {

//! Internal interface handler for directive_waitRel
void FpySequencer::directive_waitRel_internalInterfaceHandler(const Svc::FpySequencer_WaitRelDirective& directive) {
    Fw::Time wakeupTime = getTime();

    wakeupTime.add(directive.getduration().getSeconds(), directive.getduration().getUSeconds());

    m_runtime.wakeupTime = wakeupTime;
    
}

//! Internal interface handler for directive_waitAbs
void FpySequencer::directive_waitAbs_internalInterfaceHandler(const Svc::FpySequencer_WaitAbsDirective& directive) {
    m_runtime.wakeupTime = directive.getwakeupTime();
}
}  // namespace Svc