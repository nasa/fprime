// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  cpp file for FpySequencer test harness implementation class
// ======================================================================

#include "FpySequencerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FpySequencerTester ::FpySequencerTester()
    : FpySequencerGTestBase("FpySequencerTester", FpySequencerTester::MAX_HISTORY_SIZE), component("FpySequencer") {
    this->connectPorts();
    this->initComponents();
}

FpySequencerTester ::~FpySequencerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FpySequencerTester::test_waitRel() {
    FpySequencer_WaitRelDirective directive(Fw::TimeInterval(5, 123));
    Fw::Time testTime(0, 0);
    setTestTime(testTime);
    component.directive_waitRel_internalInterfaceInvoke(directive);
    component.doDispatch();

    
    // TODO how to assert on sm signal?
}

void FpySequencerTester::test_waitAbs() {
    FpySequencer_WaitAbsDirective directive(Fw::Time(5, 123));
    component.directive_waitAbs_internalInterfaceInvoke(directive);
    component.doDispatch();
    // TODO how to assert on sm signal?
}



}  // namespace Svc
