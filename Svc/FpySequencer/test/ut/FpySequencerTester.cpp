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

using Signal = FpySequencer_SequencerStateMachineStateMachineBase::Signal;
using State = FpySequencer_SequencerStateMachineStateMachineBase::State;

void FpySequencerTester::test_waitRel() {
    FpySequencer_WaitRelDirective directive(Fw::TimeInterval(5, 123));
    Fw::Time testTime(100, 100);
    setTestTime(testTime);
    
    Signal result = component.waitRel_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(component.m_runtime.wakeupTime, Fw::Time(105, 223));
}

void FpySequencerTester::test_waitAbs() {
    FpySequencer_WaitAbsDirective directive(Fw::Time(5, 123));
    
    Signal result = component.waitAbs_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(component.m_runtime.wakeupTime, Fw::Time(5, 123));
}

void FpySequencerTester::test_checkShouldWakeMismatchBase() {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    component.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 100, 100);
    Signal result = component.checkShouldWake();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

void FpySequencerTester::test_checkShouldWakeMismatchContext() {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    component.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 100, 100);
    Signal result = component.checkShouldWake();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

void FpySequencerTester::test_checkShouldWake() {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    
    // wake up at 200, currently 100
    component.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    Signal result = component.checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_keepSleeping);

    // wake up at 50, currently 100
    component.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 50, 100);
    result = component.checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_wakeup);
}

void FpySequencerTester::test_checkStatementTimeout() {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    // no timeout
    F32 timeout = 0;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    Signal result = component.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);

    // dispatched at 200, currently 300 (should time out)
    component.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    result = component.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);

    // dispatched at 295, currently 300 (should not time out)
    component.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 295, 100);
    result = component.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    // dispatched at 290, currently 300 (should time out)
    component.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 290, 100);
    result = component.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);
}

void FpySequencerTester::test_checkStatementTimeoutMismatchBase() {
    // Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    // setTestTime(testTime);

    // F32 timeout = 10;
    // paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    // paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    // component.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 200, 100);
    // Signal result = component.checkStatementTimeout();
    // ASSERT_EQ(result, Signal::result_timeOpFailed);
}

void FpySequencerTester::test_checkStatementTimeoutMismatchContext() {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    F32 timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    component.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 200, 100);
    Signal result = component.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

void FpySequencerTester::test_cmd_RUN() {
    // non blocking should instantly respond no matter what
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::NO_BLOCK);
    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
    dispatchCurrentMessages(component);

    this->clearHistory();

    // blocking will take some queue emptying to respond
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::BLOCK);

    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);

    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
}

void FpySequencerTester::test_cmd_VALIDATE() {
    sendCmd_VALIDATE(0, 0, Fw::String("invalid seq"));
    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_VALIDATE, 0, Fw::CmdResponse::EXECUTION_ERROR);
}

void FpySequencerTester::test_cmd_RUN_VALIDATED() {
    // should fail because in idle
    component.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    dispatchCurrentMessages(component);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN_VALIDATED, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();


    // should succeed immediately
    component.m_sequenceFilePath = "<invalid seq>";
    component.m_stateMachine_sequencer.m_state = State::AWAITING_CMD_RUN_VALIDATED;
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN_VALIDATED, 0, Fw::CmdResponse::OK);
    // should go back to IDLE because sequence is bad
    dispatchUntilState(State::IDLE);
}

void FpySequencerTester::test_cmd_CANCEL() {
    component.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_CANCEL(0, 0);
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail if we're in IDLE
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_CANCEL, 0, Fw::CmdResponse::EXECUTION_ERROR);

    dispatchCurrentMessages(component);
    ASSERT_EQ(component.sequencer_getState(), State::IDLE);


    this->clearHistory();
    component.m_stateMachine_sequencer.m_state = State::RUNNING_SLEEPING;
    sendCmd_CANCEL(0, 0);
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should succeed instantly
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_CANCEL, 0, Fw::CmdResponse::OK);
    // should go back to idle
    dispatchUntilState(State::IDLE);
}

void FpySequencerTester::test_cmd_DEBUG_CLEAR_BREAKPOINT() {
    component.m_debug.breakOnBreakpoint = true;
    sendCmd_DEBUG_CLEAR_BREAKPOINT(0, 0);
    // dispatch cmd
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CLEAR_BREAKPOINT, 0, Fw::CmdResponse::OK);
    // dispatch signal
    component.doDispatch();
    ASSERT_FALSE(component.m_debug.breakOnBreakpoint);
}

void FpySequencerTester::test_cmd_DEBUG_SET_BREAKPOINT() {
    sendCmd_DEBUG_SET_BREAKPOINT(0, 0, 123, true);
    // dispatch cmd handler
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_SET_BREAKPOINT, 0, Fw::CmdResponse::OK);
    // dispatch signal
    component.doDispatch();
    ASSERT_TRUE(component.m_debug.breakOnBreakpoint);
    ASSERT_TRUE(component.m_debug.breakOnlyOnceOnBreakpoint);
    ASSERT_EQ(component.m_debug.breakpointIndex, 123);
}

void FpySequencerTester::test_cmd_DEBUG_BREAK() {
    component.m_stateMachine_sequencer.m_state = State::IDLE;
    component.m_debug.breakOnBreakpoint = false;
    component.m_debug.breakOnlyOnceOnBreakpoint = false;
    sendCmd_DEBUG_BREAK(0, 0, true);
    dispatchCurrentMessages(component);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in idle
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_BREAK, 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FALSE(component.m_debug.breakOnBreakpoint);

    // now try while running
    this->clearHistory();
    component.m_stateMachine_sequencer.m_state = State::RUNNING_AWAITING_STATEMENT_RESPONSE;
    sendCmd_DEBUG_BREAK(0, 0, true);
    // dispatch cmd handler
    dispatchCurrentMessages(component);
    // dispatch signal handler
    dispatchCurrentMessages(component);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in running
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_BREAK, 0, Fw::CmdResponse::OK);
    ASSERT_TRUE(component.m_debug.breakOnBreakpoint);
    ASSERT_TRUE(component.m_debug.breakOnlyOnceOnBreakpoint);
}

void FpySequencerTester::test_cmd_DEBUG_CONTINUE() {
    component.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_DEBUG_CONTINUE(0, 0);
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in IDLE
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CONTINUE, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    component.m_stateMachine_sequencer.m_state = State::RUNNING_DEBUG_BROKEN;
    sendCmd_DEBUG_CONTINUE(0, 0);
    // dispatch cmd handler
    component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in debug_broken
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CONTINUE, 0, Fw::CmdResponse::OK);
    // dispatch signal handler
    component.doDispatch();
    // should have gone to dispatch stmt
    ASSERT_EQ(component.sequencer_getState(), State::RUNNING_DISPATCH_STATEMENT);
}

// dispatches events from the queue until the component reaches the given state
void FpySequencerTester::dispatchUntilState(State state, U32 bound) {
    U64 iters = 0;
    while (component.sequencer_getState() != state && iters < bound) {
        dispatchCurrentMessages(component);
        iters++;
    }
    ASSERT_EQ(component.sequencer_getState(), state);
}

}  // namespace Svc
