// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FpySequencerTester.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/MallocAllocator.hpp"
#include "Svc/FpySequencer/FppConstantsAc.hpp"

namespace Svc {

using Signal = FpySequencer_SequencerStateMachineStateMachineBase::Signal;
using State = FpySequencer_SequencerStateMachineStateMachineBase::State;

TEST_F(FpySequencerTester, waitRel) {
    FpySequencer_WaitRelDirective directive(Fw::TimeInterval(5, 123));
    Fw::Time testTime(100, 100);
    setTestTime(testTime);

    Signal result = cmp.waitRel_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(cmp.m_runtime.wakeupTime, Fw::Time(105, 223));
}

TEST_F(FpySequencerTester, waitAbs) {
    FpySequencer_WaitAbsDirective directive(Fw::Time(5, 123));

    Signal result = cmp.waitAbs_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(cmp.m_runtime.wakeupTime, Fw::Time(5, 123));
}

TEST_F(FpySequencerTester, goto) {
    FpySequencer_GotoDirective directive(123);
    cmp.m_sequenceObj.getheader().setstatementCount(456);
    Signal result = cmp.goto_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, 123);

    cmp.m_runtime.nextStatementIndex = 0;
    // out of bounds
    directive.setstatementIndex(111111);
    result = cmp.goto_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, 0);

    cmp.m_runtime.nextStatementIndex = 0;
    // just inside bounds
    directive.setstatementIndex(456);
    result = cmp.goto_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, 456);
}

TEST_F(FpySequencerTester, setLvar) {
    U8 buf[Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE];
    memset(buf, 1, sizeof(buf));
    FpySequencer_SetLocalVarDirective directive(static_cast<U8>(0), 1, static_cast<FwSizeType>(sizeof(buf)));
    Signal result = cmp.setLocalVar_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(cmp.m_runtime.localVariables[0].valueSize, sizeof(buf));
    ASSERT_EQ(memcmp(buf, cmp.m_runtime.localVariables[0].value, sizeof(buf)), 0);

    // outside of lvar range
    directive.setindex(Fpy::MAX_SEQUENCE_LOCAL_VARIABLES);
    result = cmp.setLocalVar_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);

    // check what happens if buf too big
    directive = FpySequencer_SetLocalVarDirective(static_cast<U8>(0), 1, Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE + 1);

    ASSERT_DEATH_IF_SUPPORTED(cmp.setLocalVar_directiveHandler(directive), "Assert: ");
}

TEST_F(FpySequencerTester, if) {
    cmp.m_runtime.nextStatementIndex = 100;
    cmp.m_sequenceObj.getheader().setstatementCount(123);
    Fw::ExternalSerializeBuffer buf(cmp.m_runtime.localVariables[0].value, Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE);
    buf.serialize(true);
    cmp.m_runtime.localVariables[0].valueSize = buf.getBuffLength();
    FpySequencer_IfDirective directive(0, 111);
    Signal result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    // should not have changed stmtidx
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, 100);

    buf.resetSer();
    buf.serialize(false);
    cmp.m_runtime.localVariables[0].valueSize = buf.getBuffLength();
    result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    // should have changed stmtidx
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, 111);

    cmp.m_runtime.nextStatementIndex = 100;

    directive.setfalseGotoStmtIndex(cmp.m_sequenceObj.getheader().getstatementCount());
    result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    // should have succeeded
    ASSERT_EQ(cmp.m_runtime.nextStatementIndex, cmp.m_sequenceObj.getheader().getstatementCount());

    cmp.m_runtime.nextStatementIndex = 100;

    buf.resetSer();
    // check failure to interpret as bool
    buf.serialize(static_cast<U8>(111));
    cmp.m_runtime.localVariables[0].valueSize = buf.getBuffLength();
    result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    // should not have changed stmtidx
    ASSERT_NE(cmp.m_runtime.nextStatementIndex, 111);

    directive.setconditionalLocalVarIndex(Fpy::MAX_SEQUENCE_LOCAL_VARIABLES);
    result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    // should not have changed stmtidx
    ASSERT_NE(cmp.m_runtime.nextStatementIndex, 111);

    directive.setconditionalLocalVarIndex(0);
    directive.setfalseGotoStmtIndex(cmp.m_sequenceObj.getheader().getstatementCount() + 1);
    result = cmp.if_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    // should not have changed stmtidx
    ASSERT_NE(cmp.m_runtime.nextStatementIndex, 111);
}

TEST_F(FpySequencerTester, noOp) {
    FpySequencer_NoOpDirective directive;
    Signal result = cmp.noOp_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
}

TEST_F(FpySequencerTester, getTlm) {
    FpySequencer_GetTlmDirective directive(0, 1, 456);
    nextTlmId = 456;
    nextTlmValue.setBuffLen(1);
    nextTlmValue.getBuffAddr()[0] = 200;
    nextTlmTime.set(888, 777);
    Signal result = cmp.getTlm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_from_getTlmChan(0, 456, Fw::Time(), Fw::TlmBuffer());
    ASSERT_EQ(cmp.m_runtime.localVariables[0].value[0], nextTlmValue.getBuffAddr()[0]);
    ASSERT_EQ(cmp.m_runtime.localVariables[0].valueSize, nextTlmValue.getBuffLength());
    Fw::ExternalSerializeBuffer timeBuf(cmp.m_runtime.localVariables[1].value,
                                        cmp.m_runtime.localVariables[1].valueSize);
    timeBuf.setBuffLen(cmp.m_runtime.localVariables[1].valueSize);
    Fw::Time resultTime;
    ASSERT_EQ(timeBuf.deserialize(resultTime), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(resultTime, nextTlmTime);
    clearHistory();

    // try getting a nonexistent chan
    directive.setchanId(111);
    result = cmp.getTlm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    directive.setchanId(456);

    // try setting bad value lvar
    directive.setvalueDestLvar(Fpy::MAX_SEQUENCE_LOCAL_VARIABLES);
    result = cmp.getTlm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    directive.setvalueDestLvar(0);

    // try setting bad time lvar
    directive.settimeDestLvar(Fpy::MAX_SEQUENCE_LOCAL_VARIABLES);
    result = cmp.getTlm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    directive.settimeDestLvar(0);
}

TEST_F(FpySequencerTester, getPrm) {
    FpySequencer_GetPrmDirective directive(0, 456);
    nextPrmId = 456;
    nextPrmValue.setBuffLen(1);
    nextPrmValue.getBuffAddr()[0] = 200;
    Signal result = cmp.getPrm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_from_getParam(0, 456, Fw::ParamBuffer());
    ASSERT_EQ(cmp.m_runtime.localVariables[0].value[0], nextPrmValue.getBuffAddr()[0]);
    ASSERT_EQ(cmp.m_runtime.localVariables[0].valueSize, nextPrmValue.getBuffLength());
    clearHistory();

    // try getting a nonexistent param
    directive.setprmId(111);
    result = cmp.getPrm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    directive.setprmId(456);

    // try setting bad lvar
    directive.setdestLvarIndex(Fpy::MAX_SEQUENCE_LOCAL_VARIABLES);
    result = cmp.getPrm_directiveHandler(directive);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    directive.setdestLvarIndex(0);
}

TEST_F(FpySequencerTester, checkShouldWakeMismatchBase) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    cmp.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 100, 100);
    Signal result = cmp.checkShouldWake();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, checkShouldWakeMismatchContext) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    cmp.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 100, 100);
    Signal result = cmp.checkShouldWake();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, checkShouldWake) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);

    // wake up at 200, currently 100
    cmp.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    Signal result = cmp.checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_keepSleeping);

    // wake up at 50, currently 100
    cmp.m_runtime.wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 50, 100);
    result = cmp.checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_wakeup);
}

TEST_F(FpySequencerTester, checkStatementTimeout) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    // no timeout
    F32 timeout = 0;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    Signal result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);

    // dispatched at 200, currently 300 (should time out)
    cmp.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);

    // dispatched at 295, currently 300 (should not time out)
    cmp.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 295, 100);
    result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    // dispatched at 290, currently 300 (should time out)
    cmp.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 290, 100);
    result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);
}

TEST_F(FpySequencerTester, checkStatementTimeoutMismatchBase) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    F32 timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    cmp.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 200, 100);
    Signal result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, checkStatementTimeoutMismatchContext) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    F32 timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    cmp.m_runtime.currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 200, 100);
    Signal result = cmp.checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, cmd_RUN) {
    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::VALIDATING);
    ASSERT_EQ(cmp.m_sequencesStarted, 0);
    ASSERT_EQ(cmp.m_statementsDispatched, 0);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    ASSERT_EQ(cmp.m_sequencesStarted, 1);
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(cmp.m_statementsDispatched, 1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
    this->clearHistory();

    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::NO_BLOCK);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // blocking will take some queue emptying to respond
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::BLOCK);

    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);

    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);

    this->clearHistory();

    // try running while already running
    cmp.m_stateMachine_sequencer.m_state = State::RUNNING_DISPATCH_STATEMENT;
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::BLOCK);
    // dispatch cmd
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
    removeFile("test.bin");
}

TEST_F(FpySequencerTester, cmd_VALIDATE) {
    sendCmd_VALIDATE(0, 0, Fw::String("invalid seq"));
    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_VALIDATE, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::VALIDATING);
    ASSERT_EQ(cmp.m_sequencesStarted, 0);
    ASSERT_EQ(cmp.m_statementsDispatched, 0);
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_VALIDATE, 0, Fw::CmdResponse::OK);
    this->clearHistory();

    cmp.m_stateMachine_sequencer.m_state = State::VALIDATING;
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_VALIDATE, 0, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(FpySequencerTester, cmd_RUN_VALIDATED) {
    // should fail because in idle
    cmp.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN_VALIDATED, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    this->clearHistory();
    // should succeed immediately
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN_VALIDATED, 0, Fw::CmdResponse::OK);
    // should go back to IDLE because sequence is bad
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    clearHistory();

    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    this->clearHistory();
    // should succeed immediately
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::BLOCK);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(0);
    // should go back to IDLE because sequence is bad
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN_VALIDATED, 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, cmd_CANCEL) {
    cmp.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_CANCEL(0, 0);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail if we're in IDLE
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_CANCEL, 0, Fw::CmdResponse::EXECUTION_ERROR);

    dispatchCurrentMessages(cmp);
    ASSERT_EQ(cmp.sequencer_getState(), State::IDLE);

    this->clearHistory();
    cmp.m_stateMachine_sequencer.m_state = State::RUNNING_SLEEPING;
    sendCmd_CANCEL(0, 0);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should succeed instantly
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_CANCEL, 0, Fw::CmdResponse::OK);
    // should go back to idle
    dispatchUntilState(State::IDLE);
}

TEST_F(FpySequencerTester, cmd_DEBUG_CLEAR_BREAKPOINT) {
    cmp.m_debug.breakOnBreakpoint = true;
    sendCmd_DEBUG_CLEAR_BREAKPOINT(0, 0);
    // dispatch cmd
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CLEAR_BREAKPOINT, 0, Fw::CmdResponse::OK);
    // dispatch signal
    cmp.doDispatch();
    ASSERT_FALSE(cmp.m_debug.breakOnBreakpoint);
}

TEST_F(FpySequencerTester, cmd_DEBUG_SET_BREAKPOINT) {
    sendCmd_DEBUG_SET_BREAKPOINT(0, 0, 123, true);
    // dispatch cmd handler
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_SET_BREAKPOINT, 0, Fw::CmdResponse::OK);
    // dispatch signal
    cmp.doDispatch();
    ASSERT_TRUE(cmp.m_debug.breakOnBreakpoint);
    ASSERT_TRUE(cmp.m_debug.breakOnlyOnceOnBreakpoint);
    ASSERT_EQ(cmp.m_debug.breakpointIndex, 123);
}

TEST_F(FpySequencerTester, cmd_DEBUG_BREAK) {
    cmp.m_stateMachine_sequencer.m_state = State::IDLE;
    cmp.m_debug.breakOnBreakpoint = false;
    cmp.m_debug.breakOnlyOnceOnBreakpoint = false;
    sendCmd_DEBUG_BREAK(0, 0, true);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in idle
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_BREAK, 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FALSE(cmp.m_debug.breakOnBreakpoint);

    // now try while running
    this->clearHistory();
    cmp.m_stateMachine_sequencer.m_state = State::RUNNING_AWAITING_STATEMENT_RESPONSE;
    sendCmd_DEBUG_BREAK(0, 0, true);
    // dispatch cmd handler
    dispatchCurrentMessages(cmp);
    // dispatch signal handler
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in running
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_BREAK, 0, Fw::CmdResponse::OK);
    ASSERT_TRUE(cmp.m_debug.breakOnBreakpoint);
    ASSERT_TRUE(cmp.m_debug.breakOnlyOnceOnBreakpoint);
}

TEST_F(FpySequencerTester, cmd_DEBUG_CONTINUE) {
    cmp.m_stateMachine_sequencer.m_state = State::IDLE;
    sendCmd_DEBUG_CONTINUE(0, 0);
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in IDLE
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CONTINUE, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    cmp.m_stateMachine_sequencer.m_state = State::RUNNING_DEBUG_BROKEN;
    sendCmd_DEBUG_CONTINUE(0, 0);
    // dispatch cmd handler
    cmp.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in debug_broken
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_DEBUG_CONTINUE, 0, Fw::CmdResponse::OK);
    // dispatch signal handler
    cmp.doDispatch();
    // should have gone to dispatch stmt
    ASSERT_EQ(cmp.sequencer_getState(), State::RUNNING_DISPATCH_STATEMENT);
}

TEST_F(FpySequencerTester, readHeader) {
    U8 seqBuf[Fpy::Header::SERIALIZED_SIZE] = {0};
    cmp.m_sequenceBuffer.setExtBuffer(seqBuf, sizeof(seqBuf));
    Fpy::Header header;
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    header.setbodySize(50);
    header.setschemaVersion(Fpy::SCHEMA_VERSION);
    header.setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(cmp.m_sequenceBuffer.serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(cmp.readHeader(), Fw::Success::SUCCESS);
    ASSERT_EQ(cmp.m_sequenceObj.getheader(), header);

    // check not enough bytes
    cmp.m_sequenceBuffer.resetDeser();
    cmp.m_sequenceBuffer.setBuffLen(cmp.m_sequenceBuffer.getBuffLength() - 1);
    ASSERT_EQ(cmp.readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileReadDeserializeError_SIZE(1);

    // check wrong schema version
    cmp.m_sequenceBuffer.resetSer();
    header.setschemaVersion(Fpy::SCHEMA_VERSION + 1);
    ASSERT_EQ(cmp.m_sequenceBuffer.serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(cmp.readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    header.setschemaVersion(Fpy::SCHEMA_VERSION);
    clearHistory();

    // check too many args
    cmp.m_sequenceBuffer.resetSer();
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT + 1);
    ASSERT_EQ(cmp.m_sequenceBuffer.serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(cmp.readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceArgs_SIZE(1);
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);

    // check too many stmts
    cmp.m_sequenceBuffer.resetSer();
    header.setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT + 1);
    ASSERT_EQ(cmp.m_sequenceBuffer.serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(cmp.readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceStatements_SIZE(1);
}

TEST_F(FpySequencerTester, readBody) {
    U8 data[Fpy::MAX_SEQUENCE_ARG_COUNT + Fpy::MAX_SEQUENCE_STATEMENT_COUNT * Fpy::Statement::SERIALIZED_SIZE];

    cmp.m_sequenceBuffer.setExtBuffer(data, sizeof(data));
    // write some args mappings
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        // map arg idx ii to lvar pos 123
        ASSERT_EQ(cmp.m_sequenceBuffer.serialize(static_cast<U8>(123)), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // write some statements
    Fpy::Statement stmt(Fpy::StatementType::DIRECTIVE, Fpy::DirectiveId::NO_OP, Fw::StatementArgBuffer());
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(cmp.m_sequenceBuffer.serialize(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    cmp.m_sequenceObj.getheader().setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    cmp.m_sequenceObj.getheader().setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(cmp.readBody(), Fw::Success::SUCCESS);

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        ASSERT_EQ(cmp.m_sequenceObj.getargs()[ii], 123);
    }

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(cmp.m_sequenceObj.getstatements()[ii], stmt);
    }

    cmp.m_sequenceBuffer.resetSer();
    cmp.m_sequenceObj.getheader().setstatementCount(0);
    // now see what happens if we don't write enough args
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT - 1; ii++) {
        // map arg idx ii to lvar pos 123
        ASSERT_EQ(cmp.m_sequenceBuffer.serialize(static_cast<U8>(123)), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // don't write any stmts otherwise their bytes will be interpreted as arg mappings and it will trigger
    // the wrong branch
    ASSERT_EQ(cmp.readBody(), Fw::Success::FAILURE);

    // now see what happens if we don't write enough stmts
    cmp.m_sequenceBuffer.resetSer();
    cmp.m_sequenceObj.getheader().setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    cmp.m_sequenceObj.getheader().setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        // map arg idx ii to lvar pos 123
        ASSERT_EQ(cmp.m_sequenceBuffer.serialize(static_cast<U8>(123)), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // the -1 here is the intended mistake
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT - 1; ii++) {
        ASSERT_EQ(cmp.m_sequenceBuffer.serialize(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    ASSERT_EQ(cmp.readBody(), Fw::Success::FAILURE);
}

TEST_F(FpySequencerTester, readFooter) {
    U8 data[Fpy::Footer::SERIALIZED_SIZE];

    cmp.m_sequenceBuffer.setExtBuffer(data, sizeof(data));

    cmp.m_computedCRC = 0x12345678;
    Fpy::Footer footer(static_cast<U32>(~0x12345678));
    ASSERT_EQ(cmp.m_sequenceBuffer.serialize(footer), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(cmp.readFooter(), Fw::Success::SUCCESS);
    ASSERT_EQ(cmp.m_sequenceObj.getfooter(), footer);

    cmp.m_sequenceBuffer.resetDeser();
    // try wrong crc
    cmp.m_computedCRC = 0x44444444;
    ASSERT_EQ(cmp.readFooter(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongCRC_SIZE(1);

    // try not enough remaining
    ASSERT_EQ(cmp.m_sequenceBuffer.setBuffLen(cmp.m_sequenceBuffer.getBuffLength() - 1),
              Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(cmp.readFooter(), Fw::Success::FAILURE);
}

TEST_F(FpySequencerTester, readBytes) {
    // no statements, just header and footer
    writeToFile("test.bin");
    U8 data[Fpy::Sequence::SERIALIZED_SIZE] = {0};
    cmp.m_sequenceBuffer.setExtBuffer(data, sizeof(data));
    Os::File seqFile;
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(cmp.readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE), Fw::Success::SUCCESS);
    seqFile.close();

    // check capacity too low
    cmp.m_sequenceBuffer.setExtBuffer(data, Fpy::Header::SERIALIZED_SIZE - 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(cmp.readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE), Fw::Success::FAILURE);
    seqFile.close();

    // check not enough bytes
    cmp.m_sequenceBuffer.setExtBuffer(data, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(cmp.readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1),
              Fw::Success::FAILURE);

    seqFile.close();
    removeFile("test.bin");

    // read after close
    ASSERT_DEATH_IF_SUPPORTED(cmp.readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE),
                              "Assert: ");
}

TEST_F(FpySequencerTester, validate) {
    // nominal
    add_NO_OP();
    writeToFile("test.bin");
    U8 data[Fpy::Sequence::SERIALIZED_SIZE] = {0};
    cmp.m_sequenceBuffer.setExtBuffer(data, sizeof(data));
    cmp.m_sequenceFilePath = "test.bin";
    ASSERT_EQ(cmp.validate(), Fw::Success::SUCCESS);

    // cause validation failure to open
    removeFile("test.bin");
    ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    this->clearHistory();

    // cause not enough bytes for header
    cmp.m_sequenceBuffer.resetSer();
    writeToFile("test.bin", Fpy::Header::SERIALIZED_SIZE - 1);
    ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_EndOfFileError_SIZE(1);
    this->clearHistory();
    removeFile("test.bin");

    cmp.m_sequenceBuffer.resetSer();
    // cause fail to validate header
    seq.getheader().setschemaVersion(Fpy::SCHEMA_VERSION + 1);
    writeToFile("test.bin");
    ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    seq.getheader().setschemaVersion(Fpy::SCHEMA_VERSION);
    this->clearHistory();
    removeFile("test.bin");

    // cause not enough bytes for body
    writeToFile("test.bin", Fpy::Header::SERIALIZED_SIZE + 1);
    ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_EndOfFileError_SIZE(1);
    this->clearHistory();
    removeFile("test.bin");

    // cause body deser error
    // TODO cannot figure out how to cause this
    // // bad statement arg buf len
    // seq.getstatements()[0].getargBuf().m_serLoc = Fpy::Sequence::SERIALIZED_SIZE + 1;
    // writeToFile("test.bin");
    // ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    // ASSERT_EVENTS_FileReadDeserializeError_SIZE(1);
    // this->clearHistory();
    // removeFile("test.bin");

    // write some more bytes after the file end
    writeToFile("test.bin");
    Os::File file;
    file.open("test.bin", Os::FileInterface::OPEN_APPEND);
    U8 extraByte[1] = {0};
    FwSizeType size = 1;
    ASSERT_EQ(file.write(extraByte, size), Os::File::OP_OK);
    file.close();
    ASSERT_EQ(cmp.validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_ExtraBytesInSequence_SIZE(1);
}

TEST_F(FpySequencerTester, allocateBuffer) {
    Fw::MallocAllocator alloc;
    cmp.allocateBuffer(0, alloc, 100);
    ASSERT_NE(cmp.m_sequenceBuffer.getBuffAddr(), nullptr);
    ASSERT_EQ(cmp.m_sequenceBuffer.getBuffCapacity(), 100);
    cmp.deallocateBuffer(alloc);
    ASSERT_EQ(cmp.m_sequenceBuffer.getBuffAddr(), nullptr);
    ASSERT_EQ(cmp.m_sequenceBuffer.getBuffCapacity(), 0);
}

// caught a bug
TEST_F(FpySequencerTester, dispatchStatement) {
    Fw::Time time(123, 123);
    setTestTime(time);

    add_NO_OP();
    cmp.m_sequenceObj = seq;
    Signal result = cmp.dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_success);
    ASSERT_EQ(cmp.m_runtime.currentStatementOpcode, Fpy::DirectiveId::NO_OP);
    ASSERT_EQ(cmp.m_runtime.currentStatementType, Fpy::StatementType::DIRECTIVE);
    ASSERT_EQ(cmp.m_runtime.currentStatementDispatchTime, time);
    ASSERT_EQ(cmp.m_statementsDispatched, 1);
    // try dispatching again, should fail cuz no more stmts
    result = cmp.dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_noMoreStatements);
    ASSERT_EQ(cmp.m_statementsDispatched, 1);
    // reset counter, try dispatching a bad statement
    cmp.m_runtime.nextStatementIndex = 0;
    cmp.m_sequenceObj.getstatements()[0].setopCode(Fpy::DirectiveId::NUM_CONSTANTS);
    result = cmp.dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_failure);

    clearSeq();
    time = Fw::Time(456, 123);
    setTestTime(time);
    // okay try adding a command
    addCmd(123);
    cmp.m_sequenceObj = seq;
    cmp.m_runtime.nextStatementIndex = 0;
    result = cmp.dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_success);
    ASSERT_EQ(cmp.m_runtime.currentStatementOpcode, 123);
    ASSERT_EQ(cmp.m_runtime.currentStatementType, Fpy::StatementType::COMMAND);
    ASSERT_EQ(cmp.m_runtime.currentStatementDispatchTime, time);

    cmp.m_runtime.nextStatementIndex = cmp.m_sequenceObj.getheader().getstatementCount() + 1;
    ASSERT_DEATH_IF_SUPPORTED(cmp.dispatchStatement(), "Assert: ");
}

TEST_F(FpySequencerTester, dispatchCommand) {
    U8 data[4] = {0x12, 0x23, 0x34, 0x45};
    Fw::StatementArgBuffer buf(data, sizeof(data));
    buf.setBuffLen(sizeof(data));
    Fpy::Statement cmd(Fpy::StatementType::COMMAND, 123, buf);
    Fw::Success result = cmp.dispatchCommand(cmd);
    ASSERT_EQ(result, Fw::Success::SUCCESS);

    Fw::ComBuffer expected;
    ASSERT_EQ(expected.serialize(Fw::ComPacketType::FW_PACKET_COMMAND), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(cmd.getopCode()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(buf.getBuffAddr(), buf.getBuffLength(), true), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_from_cmdOut_SIZE(1);
    ASSERT_from_cmdOut(0, expected, 0);
    this->clearHistory();

    // try dispatching again, make sure cmd uid is right
    cmp.m_statementsDispatched = 123;
    result = cmp.dispatchCommand(cmd);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_from_cmdOut(0, expected, cmp.m_statementsDispatched);
    this->clearHistory();

    // modify sequences started, make sure correct
    cmp.m_sequencesStarted = 456;
    result = cmp.dispatchCommand(cmd);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_from_cmdOut(0, expected,
                       (((cmp.m_sequencesStarted & 0xFFFF) << 16) | (cmp.m_statementsDispatched & 0xFFFF)));

    cmd.settype(Fpy::StatementType::DIRECTIVE);
    ASSERT_DEATH_IF_SUPPORTED(cmp.dispatchCommand(cmd), "Assert: ");
}

TEST_F(FpySequencerTester, deserialize_waitRel) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitRelDirective waitRel(Fw::TimeInterval(123, 123));
    add_WAIT_REL(waitRel);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitRel, waitRel);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}
TEST_F(FpySequencerTester, deserialize_waitAbs) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitAbsDirective waitAbs(Fw::Time(123, 123));
    add_WAIT_ABS(waitAbs);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitAbs, waitAbs);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_setLVar) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_SetLocalVarDirective setLVar(0, 123, 10);
    add_SET_LVAR(setLVar);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    // for some reason, can't just use the equality method here... i get a huge asan err
    ASSERT_EQ(actual.setLVar.get_valueSize(), setLVar.get_valueSize());
    ASSERT_EQ(actual.setLVar.getindex(), setLVar.getindex());
    ASSERT_EQ(memcmp(actual.setLVar.getvalue(), setLVar.getvalue(), setLVar.get_valueSize()), 0);
    // write some junk after buf, setlocalvar should eat it up and succeed
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    this->clearHistory();
    // clear buf, should fail cuz no valueSize or whatever
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);

    this->clearHistory();
    clearSeq();
    // run with valueSize too big
    if (Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE + 6 <= FW_STATEMENT_ARG_BUFFER_MAX_SIZE) {
        // we can test this
        setLVar = FpySequencer_SetLocalVarDirective(0, 123, Fpy::MAX_LOCAL_VARIABLE_BUFFER_SIZE + 1);
        add_SET_LVAR(setLVar);
        result = cmp.deserializeDirective(seq.getstatements()[0], actual);
        ASSERT_EQ(result, Fw::Success::FAILURE);
        ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    }
}

TEST_F(FpySequencerTester, deserialize_goto) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GotoDirective gotoDir(123);
    add_GOTO(gotoDir);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.gotoDirective, gotoDir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_if) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_IfDirective ifDir(123, 9999);
    add_IF(ifDir);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.ifDirective, ifDir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_noOp) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_NoOpDirective noOp;
    add_NO_OP();
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.noOp, noOp);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, should succeed
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
}

TEST_F(FpySequencerTester, deserialize_getTlm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetTlmDirective dir(123, 234, 456);
    add_GET_TLM(dir);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getTlm, dir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_getPrm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetPrmDirective dir(123, 456);
    add_GET_PRM(dir);
    Fw::Success result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getPrm, dir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = cmp.deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

// caught a bug
TEST_F(FpySequencerTester, checkTimers) {
    allocMem();
    add_WAIT_REL(FpySequencer_WaitRelDirective(Fw::TimeInterval(10, 0)));
    writeToFile("test.bin");
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    Fw::Time time(0, 0);
    setTestTime(time);
    dispatchUntilState(State::RUNNING_SLEEPING);

    time = Fw::Time(5, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    dispatchCurrentMessages(cmp);
    // should not leave sleep
    ASSERT_EQ(cmp.sequencer_getState(), State::RUNNING_SLEEPING);

    time = Fw::Time(10, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    // should leave sleep
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
    clearHistory();

    // okay now make sure it also works for checking timeout
    paramSet_STATEMENT_TIMEOUT_SECS(5, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    clearHistory();

    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_SLEEPING);
    time = Fw::Time(12, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    dispatchCurrentMessages(cmp);
    // should not leave sleep
    ASSERT_EQ(cmp.sequencer_getState(), State::RUNNING_SLEEPING);
    time = Fw::Time(15, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // timed out, should give exec error
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(FpySequencerTester, ping) {
    invoke_to_pingIn(0, 0);
    cmp.doDispatch();
    ASSERT_from_pingOut_SIZE(1);
}

TEST_F(FpySequencerTester, cmdResponse) {
    invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    cmp.doDispatch();
    ASSERT_EVENTS_CmdResponseWhileNotRunningSequence_SIZE(1);
    clearHistory();

    allocMem();
    addCmd(123);
    writeToFile("test.bin");
    cmp.m_sequencesStarted = 255;
    cmp.m_statementsDispatched = 255;
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);

    // should be 256 for seq idx and 255 for cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x010000FF, Fw::CmdResponse::OK);
    // should be successful
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
    clearHistory();
    // let's try that again but with a command that fails
    cmp.m_sequencesStarted = 255;
    cmp.m_statementsDispatched = 255;
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);

    invoke_to_cmdResponseIn(0, 123, 0x010000FF, Fw::CmdResponse::EXECUTION_ERROR);
    dispatchUntilState(State::IDLE);
    // should fail seq
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    cmp.m_sequencesStarted = 255;
    cmp.m_statementsDispatched = 255;
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    // send wrong cmd uid
    // should be 256 for seq idx and 255 for cmd idx
    // but we're gonna send 255 for seq idx and 255 for cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x00FF00FF, Fw::CmdResponse::OK);
    // should fail on seq idx, but should stay in running
    dispatchCurrentMessages(cmp);
    ASSERT_EQ(cmp.sequencer_getState(), State::RUNNING_AWAITING_STATEMENT_RESPONSE);

    // okay now send right seq idx but wrong cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x01000100, Fw::CmdResponse::OK);
    // should fail on cmd idx and go back to IDLE
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    // okay now have a command response come in from this seq
    // while sleeping (coding err)
    clearSeq();
    add_WAIT_REL(FpySequencer_WaitRelDirective(Fw::TimeInterval(10, 0)));
    addCmd(123);
    writeToFile("test.bin");
    cmp.m_sequencesStarted = 255;
    cmp.m_statementsDispatched = 255;
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_SLEEPING);
    invoke_to_cmdResponseIn(0, 123, 0x010000FF, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    // okay now have the wrong opcode come in
    clearSeq();
    addCmd(123);
    writeToFile("test.bin");
    cmp.m_sequencesStarted = 255;
    cmp.m_statementsDispatched = 255;
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    invoke_to_cmdResponseIn(0, 456, 0x010000FF, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();
}

TEST_F(FpySequencerTester, tlmWrite) {
    invoke_to_tlmWrite(0, 0);
    cmp.doDispatch();
    // make sure that all tlm is written every call
    ASSERT_TLM_SIZE(9);
}

TEST_F(FpySequencerTester, seqRunIn) {
    allocMem();
    add_NO_OP();
    writeToFile("test.bin");

    invoke_to_seqRunIn(0, Fw::String("test.bin"));
    cmp.doDispatch();
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    dispatchUntilState(State::IDLE);

    this->clearHistory();

    // try running while already running
    cmp.m_stateMachine_sequencer.m_state = State::RUNNING_DISPATCH_STATEMENT;
    invoke_to_seqRunIn(0, Fw::String("test.bin"));
    // dispatch cmd
    cmp.doDispatch();
    ASSERT_EVENTS_InvalidSeqRunCall_SIZE(1);
    removeFile("test.bin");
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}