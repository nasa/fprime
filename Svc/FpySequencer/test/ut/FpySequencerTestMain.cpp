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
using DirectiveError = FpySequencer_DirectiveErrorCode;

TEST_F(FpySequencerTester, waitRel) {
    FpySequencer_WaitRelDirective directive{};
    Fw::Time testTime(100, 100);
    setTestTime(testTime);

    tester_push<U32>(5);
    tester_push<U32>(123);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_waitRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->wakeupTime, Fw::Time(105, 223));
}

TEST_F(FpySequencerTester, waitAbs) {
    FpySequencer_WaitAbsDirective directive{};

    tester_push<U16>(0);
    tester_push<U8>(0);
    tester_push<U32>(5);
    tester_push<U32>(123);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_waitAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->wakeupTime, Fw::Time(5, 123));
}

TEST_F(FpySequencerTester, goto) {
    FpySequencer_GotoDirective directive(123);
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(456);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 123);

    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    // out of bounds
    directive.set_statementIndex(111111);
    result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STMT_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 0);

    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    // just inside bounds
    directive.set_statementIndex(456);
    result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 456);
}

TEST_F(FpySequencerTester, if) {
    tester_get_m_runtime_ptr()->nextStatementIndex = 100;
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(123);
    tester_push<U8>(1);
    FpySequencer_IfDirective directive(111);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    // should not have changed stmtidx
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 100);

    tester_push<U8>(0);  // set it to false
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    // should have changed stmtidx
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 111);

    tester_get_m_runtime_ptr()->nextStatementIndex = 100;

    directive.set_falseGotoStmtIndex(tester_get_m_sequenceObj_ptr()->get_header().get_statementCount());
    tester_push<U8>(0);
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    // should have succeeded
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex,
              tester_get_m_sequenceObj_ptr()->get_header().get_statementCount());

    tester_get_m_runtime_ptr()->nextStatementIndex = 100;

    // check underflow
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    // should not have changed stmtidx
    ASSERT_NE(tester_get_m_runtime_ptr()->nextStatementIndex, 111);

    tester_push<U8>(1);
    directive.set_falseGotoStmtIndex(tester_get_m_sequenceObj_ptr()->get_header().get_statementCount() + 1);
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STMT_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    // should not have changed stmtidx
    ASSERT_NE(tester_get_m_runtime_ptr()->nextStatementIndex, 111);
}

TEST_F(FpySequencerTester, noOp) {
    FpySequencer_NoOpDirective directive;
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_noOp_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
}

TEST_F(FpySequencerTester, storeTlmVal) {
    FpySequencer_StoreTlmValDirective directive(456, 0);
    nextTlmId = 456;
    nextTlmValue.setBuffLen(1);
    nextTlmValue.getBuffAddr()[0] = 200;
    nextTlmTime.set(888, 777);
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->stackSize = 1;
    Signal result = tester_storeTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_from_getTlmChan(0, 456, Fw::Time(), Fw::TlmBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], nextTlmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, nextTlmValue.getBuffLength());
    clearHistory();

    // try getting a nonexistent chan
    directive.set_chanId(111);
    result = tester_storeTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::TLM_CHAN_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.set_chanId(456);

    // try setting bad lvar offset
    directive.set_lvarOffset(1);
    result = tester_storeTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
}

TEST_F(FpySequencerTester, storePrm) {
    FpySequencer_StorePrmDirective directive(456, 0);
    nextPrmId = 456;
    nextPrmValue.setBuffLen(1);
    nextPrmValue.getBuffAddr()[0] = 200;
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->stackSize = 1;
    Signal result = tester_storePrm_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_from_getParam(0, 456, Fw::ParamBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], nextPrmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, nextPrmValue.getBuffLength());
    clearHistory();

    // try getting a nonexistent param
    directive.set_prmId(111);
    result = tester_storePrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::PRM_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.set_prmId(456);

    // try setting bad lvar offset
    directive.set_lvarOffset(1);
    result = tester_storePrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
}

TEST_F(FpySequencerTester, cmd) {
    U8 data[4] = {0x12, 0x23, 0x34, 0x45};
    FpySequencer_ConstCmdDirective directive(123, 0, sizeof(data));
    memcpy(directive.get_argBuf(), data, sizeof(data));
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_constCmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);

    Fw::ComBuffer expected;
    ASSERT_EQ(expected.serialize(Fw::ComPacketType::FW_PACKET_COMMAND), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(directive.get_opCode()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(data, sizeof(data), Fw::Serialization::OMIT_LENGTH),
              Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_from_cmdOut_SIZE(1);
    ASSERT_from_cmdOut(0, expected, 0);
    this->clearHistory();

    // try dispatching again, make sure cmd uid is right
    tester_set_m_statementsDispatched(123);
    result = tester_constCmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);
    ASSERT_from_cmdOut(0, expected, tester_get_m_statementsDispatched());
    this->clearHistory();

    // modify sequences started, make sure correct
    tester_set_m_sequencesStarted(456);
    result = tester_constCmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);
    ASSERT_from_cmdOut(
        0, expected,
        (((tester_get_m_sequencesStarted() & 0xFFFF) << 16) | (tester_get_m_statementsDispatched() & 0xFFFF)));
}

TEST_F(FpySequencerTester, stackOp) {
    // Test EQ (equal)
    FpySequencer_StackOpDirective directiveEQ(Fpy::DirectiveId::IEQ);
    tester_push<I64>(10);
    tester_push<I64>(10);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_stackOp_directiveHandler(directiveEQ, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test NE (not equal)
    FpySequencer_StackOpDirective directiveNE(Fpy::DirectiveId::INE);
    tester_push<I64>(10);
    tester_push<I64>(20);
    result = tester_stackOp_directiveHandler(directiveNE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test OR (bitwise OR)
    FpySequencer_StackOpDirective directiveOR(Fpy::DirectiveId::OR);
    tester_push<U8>(5);   // 0b0101;
    tester_push<U8>(10);  // 0b1010;
    result = tester_stackOp_directiveHandler(directiveOR, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 15);  // 0b1111
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);  // 0b1111
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test AND (bitwise AND)
    FpySequencer_StackOpDirective directiveAND(Fpy::DirectiveId::AND);
    tester_push<U8>(12);  // 0b1100;
    tester_push<U8>(10);  // 0b1010;
    result = tester_stackOp_directiveHandler(directiveAND, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 8);   // 0b1111
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);  // 0b1111
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test signed comparison (SLT - signed less than)
    FpySequencer_StackOpDirective directiveSLT(Fpy::DirectiveId::SLT);
    tester_push<I64>(-5);
    tester_push<I64>(10);
    result = tester_stackOp_directiveHandler(directiveSLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test unsigned comparison (ULT - unsigned less than)
    FpySequencer_StackOpDirective directiveULT(Fpy::DirectiveId::ULT);
    tester_push<I64>(5);
    tester_push<I64>(10);
    result = tester_stackOp_directiveHandler(directiveULT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test unsigned comparison (UGT - unsigned greater than)
    FpySequencer_StackOpDirective directiveUGT(Fpy::DirectiveId::UGT);
    tester_push<I64>(10);
    tester_push<I64>(5);
    result = tester_stackOp_directiveHandler(directiveUGT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test floating-point comparison (FLT - floating-point less than)
    FpySequencer_StackOpDirective directiveFLT(Fpy::DirectiveId::FLT);
    tester_push<F64>(5.5);
    tester_push<F64>(10.1);
    result = tester_stackOp_directiveHandler(directiveFLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test floating-point comparison (FGE - floating-point greater or equal)
    FpySequencer_StackOpDirective directiveFGE(Fpy::DirectiveId::FGE);
    tester_push<F64>(10.1);
    tester_push<F64>(10.1);
    result = tester_stackOp_directiveHandler(directiveFGE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    // Test out-of-bounds stack access
    FpySequencer_StackOpDirective directiveOOB(Fpy::DirectiveId::IEQ);
    result = tester_stackOp_directiveHandler(directiveOOB, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test invalid operation
    FpySequencer_StackOpDirective directiveInvalid(Fpy::DirectiveId::NO_OP);
    ASSERT_DEATH_IF_SUPPORTED(tester_stackOp_directiveHandler(directiveInvalid, err), "Assert: ");
}

TEST_F(FpySequencerTester, ieq) {
    tester_push<I64>(-1);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_ieq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<I64>(-1);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_ieq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, ine) {
    tester_push<I64>(-1);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_ine(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<I64>(-1);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_ine(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, or) {
    tester_push<U8>(true);
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<U8>(true);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<U8>(false);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, and) {
    tester_push<U8>(false);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<U8>(true);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;
    tester_push<U8>(true);
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, ult) {
    tester_push<U64>(0);
    tester_push<U64>(std::numeric_limits<U64>::max());
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, ule) {
    tester_push<U64>(0);
    tester_push<U64>(std::numeric_limits<U64>::max());
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(2);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, ugt) {
    tester_push<U64>(std::numeric_limits<U64>::max());
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(1);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, uge) {
    tester_push<U64>(std::numeric_limits<U64>::max());
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(1);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<U64>(1);
    tester_push<U64>(2);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, slt) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, sle) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, sgt) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, sge) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
}

TEST_F(FpySequencerTester, flt) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, fle) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, fgt) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, fge) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, feq) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, fne) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 1);
    tester_get_m_runtime_ptr()->stackSize = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, not) {
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_not(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack[0], 0);
}

TEST_F(FpySequencerTester, fptrunc) {
    F64 src = 123.123;
    F32 expected = static_cast<F32>(src);

    tester_push<F64>(src);
    ASSERT_EQ(tester_op_fptrunc(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<F32>());
}

TEST_F(FpySequencerTester, fpext) {
    F32 src = 123.123f;
    F64 expected = static_cast<F64>(src);

    tester_push<F32>(src);
    ASSERT_EQ(tester_op_fpext(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<F64>());
}

TEST_F(FpySequencerTester, fptosi) {
    F64 src = 123.123;
    I64 expected = static_cast<I64>(src);

    tester_push<F64>(src);
    ASSERT_EQ(tester_op_fptosi(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<I64>());
}

TEST_F(FpySequencerTester, sitofp) {
    I64 src = 123;
    F64 expected = static_cast<F64>(src);

    tester_push<I64>(src);
    ASSERT_EQ(tester_op_sitofp(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<F64>());
}

TEST_F(FpySequencerTester, fptoui) {
    F64 src = 123.123;
    U64 expected = static_cast<U64>(src);

    tester_push<F64>(src);
    ASSERT_EQ(tester_op_fptosi(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<U64>());
}

TEST_F(FpySequencerTester, uitofp) {
    U64 src = 123;
    F64 expected = static_cast<F64>(src);

    tester_push<U64>(src);
    ASSERT_EQ(tester_op_sitofp(), DirectiveError::NO_ERROR);
    ASSERT_EQ(expected, tester_pop<F64>());
}

TEST_F(FpySequencerTester, iadd) {
    tester_push<I64>(100);
    tester_push<I64>(23);
    ASSERT_EQ(tester_op_iadd(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, isub) {
    tester_push<I64>(150);
    tester_push<I64>(27);
    ASSERT_EQ(tester_op_isub(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, imul) {
    tester_push<I64>(41);
    tester_push<I64>(3);
    ASSERT_EQ(tester_op_imul(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, udiv) {
    tester_push<U64>(246);
    tester_push<U64>(2);
    ASSERT_EQ(tester_op_udiv(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<U64>(), 123);
}

TEST_F(FpySequencerTester, sdiv) {
    tester_push<I64>(-246);
    tester_push<I64>(-2);
    ASSERT_EQ(tester_op_sdiv(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, umod) {
    tester_push<U64>(123);
    tester_push<U64>(10);
    ASSERT_EQ(tester_op_umod(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<U64>(), 3);
}

TEST_F(FpySequencerTester, smod) {
    tester_push<I64>(-7);
    tester_push<I64>(-3);
    ASSERT_EQ(tester_op_smod(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), -1);
}

TEST_F(FpySequencerTester, fadd) {
    tester_push<F64>(100.5);
    tester_push<F64>(22.5);
    ASSERT_EQ(tester_op_fadd(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 123.0);
}

TEST_F(FpySequencerTester, fsub) {
    tester_push<F64>(150.5);
    tester_push<F64>(27.5);
    ASSERT_EQ(tester_op_fsub(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 123.0);
}

TEST_F(FpySequencerTester, fmul) {
    tester_push<F64>(41.0);
    tester_push<F64>(3.0);
    ASSERT_EQ(tester_op_fmul(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 123.0);
}

TEST_F(FpySequencerTester, fdiv) {
    tester_push<F64>(246.0);
    tester_push<F64>(2.0);
    ASSERT_EQ(tester_op_fdiv(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 123.0);
}

TEST_F(FpySequencerTester, float_floor_div) {
    tester_push<F64>(246.8);
    tester_push<F64>(2.0);
    ASSERT_EQ(tester_op_float_floor_div(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 123.0);
}

TEST_F(FpySequencerTester, fpow) {
    tester_push<F64>(3.0);
    tester_push<F64>(2.0);
    ASSERT_EQ(tester_op_fpow(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 9.0);
}

TEST_F(FpySequencerTester, flog) {
    tester_push<F64>(2.718281828459045);  // e
    ASSERT_EQ(tester_op_flog(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), 1.0);
}
TEST_F(FpySequencerTester, fmod) {
    tester_push<F64>(-8.5);
    tester_push<F64>(-2.5);
    ASSERT_EQ(tester_op_fmod(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<F64>(), -1.0);
}
TEST_F(FpySequencerTester, exit) {
    FpySequencer_ExitDirective directive;
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(123);
    tester_push<U8>(true);
    Signal result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_header().get_statementCount(), 123);

    tester_push<U8>(false);
    result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::DELIBERATE_FAILURE);
}

TEST_F(FpySequencerTester, discard) {
    // Test discarding 4 bytes
    tester_push<U32>(0x12345678);
    FpySequencer_DiscardDirective directive(4);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_discard_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 0);

    // Test error case - try to discard more bytes than available
    tester_push<U8>(0x12);
    directive.set_size(2);
    result = tester_discard_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, stackCmd) {
    // Test valid command
    tester_push<U32>(0x12345678);    // Args
    tester_push<FwOpcodeType>(123);  // Opcode

    FpySequencer_StackCmdDirective directive(4);  // 4 bytes of args
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_stackCmd_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_from_cmdOut_SIZE(1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stackSize, 0);

    // Test error case - not enough bytes on stack
    tester_push<U8>(0x12);
    result = tester_stackCmd_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, memCmp) {
    // Test equal memory blocks
    tester_push<U32>(0x12345678);
    tester_push<U32>(0x12345678);

    FpySequencer_MemCmpDirective directive(4);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_memCmp_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<U8>(), 1);  // Should be true

    // Test unequal memory blocks
    tester_push<U32>(0x12345678);
    tester_push<U32>(0x87654321);
    result = tester_memCmp_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<U8>(), 0);  // Should be false

    // test not enough bytes on stack
    tester_push<U32>(0x12345678);
    tester_push<U8>(0x11);
    directive.set_size(3);
    result = tester_memCmp_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, checkShouldWakeMismatchBase) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    tester_get_m_runtime_ptr()->wakeupTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 100, 100);
    Signal result = tester_checkShouldWake();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, checkShouldWakeMismatchContext) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    tester_get_m_runtime_ptr()->wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 100, 100);
    Signal result = tester_checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_wakeup);
}

TEST_F(FpySequencerTester, checkShouldWake) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);

    // wake up at 200, currently 100
    tester_get_m_runtime_ptr()->wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    Signal result = tester_checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_keepSleeping);

    // wake up at 50, currently 100
    tester_get_m_runtime_ptr()->wakeupTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 50, 100);
    result = tester_checkShouldWake();
    ASSERT_EQ(result, Signal::result_checkShouldWake_wakeup);
}

TEST_F(FpySequencerTester, checkStatementTimeout) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    // no timeout
    F32 timeout = 0;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    Signal result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);

    // dispatched at 200, currently 300 (should time out)
    tester_get_m_runtime_ptr()->currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 200, 100);
    result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);

    // dispatched at 295, currently 300 (should not time out)
    tester_get_m_runtime_ptr()->currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 295, 100);
    result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_noTimeout);

    // dispatched at 290, currently 300 (should time out)
    tester_get_m_runtime_ptr()->currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 290, 100);
    result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);
}

TEST_F(FpySequencerTester, checkStatementTimeoutMismatchBase) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    F32 timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    tester_get_m_runtime_ptr()->currentStatementDispatchTime = Fw::Time(TimeBase::TB_DONT_CARE, 0, 200, 100);
    Signal result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_timeOpFailed);
}

TEST_F(FpySequencerTester, checkStatementTimeoutMismatchContext) {
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 300, 100);
    setTestTime(testTime);

    F32 timeout = 10;
    paramSet_STATEMENT_TIMEOUT_SECS(timeout, Fw::ParamValid::VALID);
    paramSend_STATEMENT_TIMEOUT_SECS(0, 0);
    tester_get_m_runtime_ptr()->currentStatementDispatchTime = Fw::Time(TimeBase::TB_WORKSTATION_TIME, 10, 200, 100);
    Signal result = tester_checkStatementTimeout();
    ASSERT_EQ(result, Signal::result_checkStatementTimeout_statementTimeout);
}

TEST_F(FpySequencerTester, cmd_RUN) {
    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::VALIDATING);
    ASSERT_EQ(tester_get_m_sequencesStarted(), 0);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 0);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    ASSERT_from_seqStartOut_SIZE(1);
    ASSERT_from_seqStartOut(0, Fw::String("test.bin"));
    ASSERT_EQ(tester_get_m_sequencesStarted(), 1);
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
    ASSERT_from_seqDoneOut_SIZE(1);
    ASSERT_from_seqDoneOut(0, 0, 0, Fw::CmdResponse::OK);
    this->clearHistory();

    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::NO_BLOCK);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
    ASSERT_from_seqDoneOut_SIZE(0);
    dispatchUntilState(State::VALIDATING);
    ASSERT_from_seqStartOut_SIZE(1);
    ASSERT_from_seqStartOut(0, Fw::String("test.bin"));
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_from_seqDoneOut_SIZE(1);
    ASSERT_from_seqDoneOut(0, 0, 0, Fw::CmdResponse::OK);

    this->clearHistory();

    // blocking will take some queue emptying to respond
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::BLOCK);

    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);

    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_from_seqDoneOut_SIZE(1);
    ASSERT_from_seqDoneOut(0, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);

    this->clearHistory();

    // try running while already running
    this->tester_setState(State::RUNNING_DISPATCH_STATEMENT);
    sendCmd_RUN(0, 0, Fw::String("invalid seq"), FpySequencer_BlockState::BLOCK);
    // dispatch cmd
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    removeFile("test.bin");
}

TEST_F(FpySequencerTester, cmd_VALIDATE) {
    sendCmd_VALIDATE(0, 0, Fw::String("invalid seq"));
    // should try validating, then go to idle cuz it failed
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_VALIDATE(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::VALIDATING);
    ASSERT_EQ(tester_get_m_sequencesStarted(), 0);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 0);
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_VALIDATE(), 0, Fw::CmdResponse::OK);
    this->clearHistory();

    this->tester_setState(State::VALIDATING);
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_VALIDATE(), 0, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(FpySequencerTester, cmd_RUN_VALIDATED) {
    // should fail because in idle
    this->tester_setState(State::IDLE);
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN_VALIDATED(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    allocMem();
    add_NO_OP();
    writeToFile("test.bin");
    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    this->clearHistory();
    // should succeed immediately
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::NO_BLOCK);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN_VALIDATED(), 0, Fw::CmdResponse::OK);
    // should go back to IDLE because sequence is bad
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    clearHistory();

    sendCmd_VALIDATE(0, 0, Fw::String("test.bin"));
    dispatchUntilState(State::AWAITING_CMD_RUN_VALIDATED);
    this->clearHistory();
    // should succeed immediately
    sendCmd_RUN_VALIDATED(0, 0, FpySequencer_BlockState::BLOCK);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(0);
    // should go back to IDLE because sequence is bad
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN_VALIDATED(), 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, cmd_CANCEL) {
    this->tester_setState(State::IDLE);
    sendCmd_CANCEL(0, 0);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail if we're in IDLE
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_CANCEL(), 0, Fw::CmdResponse::EXECUTION_ERROR);

    dispatchCurrentMessages(cmp);
    ASSERT_EQ(this->tester_getState(), State::IDLE);

    this->clearHistory();
    this->tester_setState(State::RUNNING_SLEEPING);
    sendCmd_CANCEL(0, 0);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should succeed instantly
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_CANCEL(), 0, Fw::CmdResponse::OK);
    // should go back to idle
    dispatchUntilState(State::IDLE);
}

TEST_F(FpySequencerTester, cmd_DEBUG_CLEAR_BREAKPOINT) {
    tester_get_m_debug_ptr()->breakOnBreakpoint = true;
    sendCmd_DEBUG_CLEAR_BREAKPOINT(0, 0);
    // dispatch cmd
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_CLEAR_BREAKPOINT(), 0, Fw::CmdResponse::OK);
    // dispatch signal
    this->tester_doDispatch();
    ASSERT_FALSE(tester_get_m_debug_ptr()->breakOnBreakpoint);
}

TEST_F(FpySequencerTester, cmd_DEBUG_SET_BREAKPOINT) {
    sendCmd_DEBUG_SET_BREAKPOINT(0, 0, 123, true);
    // dispatch cmd handler
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_SET_BREAKPOINT(), 0, Fw::CmdResponse::OK);
    // dispatch signal
    this->tester_doDispatch();
    ASSERT_TRUE(tester_get_m_debug_ptr()->breakOnBreakpoint);
    ASSERT_TRUE(tester_get_m_debug_ptr()->breakOnlyOnceOnBreakpoint);
    ASSERT_EQ(tester_get_m_debug_ptr()->breakpointIndex, 123);
}

TEST_F(FpySequencerTester, cmd_DEBUG_BREAK) {
    this->tester_setState(State::IDLE);
    tester_get_m_debug_ptr()->breakOnBreakpoint = false;
    tester_get_m_debug_ptr()->breakOnlyOnceOnBreakpoint = false;
    sendCmd_DEBUG_BREAK(0, 0, true);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in idle
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_BREAK(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FALSE(tester_get_m_debug_ptr()->breakOnBreakpoint);

    // now try while running
    this->clearHistory();
    this->tester_setState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    sendCmd_DEBUG_BREAK(0, 0, true);
    // dispatch cmd handler
    dispatchCurrentMessages(cmp);
    // dispatch signal handler
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in running
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_BREAK(), 0, Fw::CmdResponse::OK);
    ASSERT_TRUE(tester_get_m_debug_ptr()->breakOnBreakpoint);
    ASSERT_TRUE(tester_get_m_debug_ptr()->breakOnlyOnceOnBreakpoint);
}

TEST_F(FpySequencerTester, cmd_DEBUG_CONTINUE) {
    this->tester_setState(State::IDLE);
    sendCmd_DEBUG_CONTINUE(0, 0);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in IDLE
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_CONTINUE(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    this->tester_setState(State::RUNNING_DEBUG_BROKEN);
    sendCmd_DEBUG_CONTINUE(0, 0);
    // dispatch cmd handler
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in debug_broken
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DEBUG_CONTINUE(), 0, Fw::CmdResponse::OK);
    // dispatch signal handler
    this->tester_doDispatch();
    // should have gone to dispatch stmt
    ASSERT_EQ(this->tester_getState(), State::RUNNING_DISPATCH_STATEMENT);
}

TEST_F(FpySequencerTester, readHeader) {
    U8 seqBuf[Fpy::Header::SERIALIZED_SIZE] = {0};
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(seqBuf, sizeof(seqBuf));
    Fpy::Header header;
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    header.set_bodySize(50);
    header.set_schemaVersion(Fpy::SCHEMA_VERSION);
    header.set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(tester_readHeader(), Fw::Success::SUCCESS);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_header(), header);

    // check not enough bytes
    tester_get_m_sequenceBuffer_ptr()->resetDeser();
    tester_get_m_sequenceBuffer_ptr()->setBuffLen(tester_get_m_sequenceBuffer_ptr()->getBuffLength() - 1);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileReadDeserializeError_SIZE(1);

    // check wrong schema version
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_schemaVersion(Fpy::SCHEMA_VERSION + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    header.set_schemaVersion(Fpy::SCHEMA_VERSION);
    clearHistory();

    // check too many args
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceArgs_SIZE(1);
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);

    // check too many stmts
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceStatements_SIZE(1);
}

TEST_F(FpySequencerTester, readBody) {
    U8 data[Fpy::MAX_SEQUENCE_ARG_COUNT + Fpy::MAX_SEQUENCE_STATEMENT_COUNT * Fpy::Statement::SERIALIZED_SIZE];

    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));
    // write some args mappings
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        // map arg idx ii to serReg pos 123
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(static_cast<U8>(123)),
                  Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // write some statements
    Fpy::Statement stmt(Fpy::DirectiveId::NO_OP, Fw::StatementArgBuffer());
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    tester_get_m_sequenceObj_ptr()->get_header().set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(tester_readBody(), Fw::Success::SUCCESS);

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_args()[ii], 123);
    }

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_statements()[ii], stmt);
    }

    tester_get_m_sequenceBuffer_ptr()->resetSer();
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(0);
    // now see what happens if we don't write enough args
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT - 1; ii++) {
        // map arg idx ii to serReg pos 123
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(static_cast<U8>(123)),
                  Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // don't write any stmts otherwise their bytes will be interpreted as arg mappings and it will trigger
    // the wrong branch
    ASSERT_EQ(tester_readBody(), Fw::Success::FAILURE);

    // now see what happens if we don't write enough stmts
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    tester_get_m_sequenceObj_ptr()->get_header().set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        // map arg idx ii to serReg pos 123
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(static_cast<U8>(123)),
                  Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // the -1 here is the intended mistake
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT - 1; ii++) {
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    ASSERT_EQ(tester_readBody(), Fw::Success::FAILURE);
}

TEST_F(FpySequencerTester, readFooter) {
    U8 data[Fpy::Footer::SERIALIZED_SIZE];

    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));

    tester_set_m_computedCRC(0x12345678);
    Fpy::Footer footer(static_cast<U32>(~0x12345678));
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(footer), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(tester_readFooter(), Fw::Success::SUCCESS);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_footer(), footer);

    tester_get_m_sequenceBuffer_ptr()->resetDeser();
    // try wrong crc
    tester_set_m_computedCRC(0x44444444);
    ASSERT_EQ(tester_readFooter(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongCRC_SIZE(1);

    // try not enough remaining
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->setBuffLen(tester_get_m_sequenceBuffer_ptr()->getBuffLength() - 1),
              Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readFooter(), Fw::Success::FAILURE);
}

TEST_F(FpySequencerTester, readBytes) {
    // no statements, just header and footer
    writeToFile("test.bin");
    U8 data[Fpy::Sequence::SERIALIZED_SIZE] = {0};
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));
    Os::File seqFile;
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE, FpySequencer_FileReadStage::HEADER, true),
              Fw::Success::SUCCESS);
    seqFile.close();

    // check capacity too low
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, Fpy::Header::SERIALIZED_SIZE - 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE, FpySequencer_FileReadStage::HEADER, true),
              Fw::Success::FAILURE);
    seqFile.close();

    // check not enough bytes
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data,
                                                    Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1,
                               FpySequencer_FileReadStage::HEADER, true),
              Fw::Success::FAILURE);

    seqFile.close();
    removeFile("test.bin");

    // read after close
    ASSERT_DEATH_IF_SUPPORTED(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE,
                                               FpySequencer_FileReadStage::HEADER, true),
                              "Assert: ");
}

TEST_F(FpySequencerTester, validate) {
    // nominal
    add_NO_OP();
    writeToFile("test.bin");
    U8 data[Fpy::Sequence::SERIALIZED_SIZE] = {0};
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));
    tester_set_m_sequenceFilePath("test.bin");
    ASSERT_EQ(tester_validate(), Fw::Success::SUCCESS);

    // cause validation failure to open
    removeFile("test.bin");
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    this->clearHistory();

    // cause not enough bytes for header
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    writeToFile("test.bin", Fpy::Header::SERIALIZED_SIZE - 1);
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_EndOfFileError_SIZE(1);
    this->clearHistory();
    removeFile("test.bin");

    tester_get_m_sequenceBuffer_ptr()->resetSer();
    // cause fail to validate header
    seq.get_header().set_schemaVersion(Fpy::SCHEMA_VERSION + 1);
    writeToFile("test.bin");
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    seq.get_header().set_schemaVersion(Fpy::SCHEMA_VERSION);
    this->clearHistory();
    removeFile("test.bin");

    // cause not enough bytes for body
    writeToFile("test.bin", Fpy::Header::SERIALIZED_SIZE + 1);
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_EndOfFileError_SIZE(1);
    this->clearHistory();
    removeFile("test.bin");

    // cause body deser error
    // TODO cannot figure out how to cause this
    // // bad statement arg buf len
    // seq.get_statements()[0].get_argBuf().m_serLoc = Fpy::Sequence::SERIALIZED_SIZE + 1;
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
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_ExtraBytesInSequence_SIZE(1);
}

TEST_F(FpySequencerTester, allocateBuffer) {
    Fw::MallocAllocator alloc;
    cmp.allocateBuffer(0, alloc, 100);
    ASSERT_NE(tester_get_m_sequenceBuffer_ptr()->getBuffAddr(), nullptr);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getBuffCapacity(), 100);
    cmp.deallocateBuffer(alloc);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getBuffAddr(), nullptr);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getBuffCapacity(), 0);
}

// caught a bug
TEST_F(FpySequencerTester, dispatchStatement) {
    Fw::Time time(123, 123);
    setTestTime(time);

    add_NO_OP();
    *(tester_get_m_sequenceObj_ptr()) = seq;
    Signal result = tester_dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementOpcode, Fpy::DirectiveId::NO_OP);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementDispatchTime, time);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 1);
    // try dispatching again, should fail cuz no more stmts
    result = tester_dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_noMoreStatements);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 1);
    // reset counter, try dispatching a bad statement
    // TODO can't figure out how to do this without triggering compiler warning
    // cmp.m_runtime.nextStatementIndex = 0;
    // cmp.m_sequenceObj.get_statements()[0].set_opCode(reinterpret_cast<Svc::Fpy::DirectiveId::T>(200));
    // result = cmp.dispatchStatement();
    // ASSERT_EQ(result, Signal::result_dispatchStatement_failure);

    clearSeq();
    time = Fw::Time(456, 123);
    setTestTime(time);
    // okay try adding a command
    add_CONST_CMD(123);
    *(tester_get_m_sequenceObj_ptr()) = seq;
    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    result = tester_dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementOpcode, Fpy::DirectiveId::CONST_CMD);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentCmdOpcode, 123);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementDispatchTime, time);

    tester_get_m_runtime_ptr()->nextStatementIndex =
        tester_get_m_sequenceObj_ptr()->get_header().get_statementCount() + 1;
    ASSERT_DEATH_IF_SUPPORTED(tester_dispatchStatement(), "Assert: ");
}

// caught one bug
TEST_F(FpySequencerTester, deserialize_waitRel) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitRelDirective waitRel;
    add_WAIT_REL();
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitRel, waitRel);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it succeeds
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(0);
}
TEST_F(FpySequencerTester, deserialize_waitAbs) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitAbsDirective waitAbs;
    add_WAIT_ABS(waitAbs);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitAbs, waitAbs);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(0);
}

TEST_F(FpySequencerTester, deserialize_goto) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GotoDirective gotoDir(123);
    add_GOTO(gotoDir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.gotoDirective, gotoDir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_if) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_IfDirective ifDir(123);
    add_IF(ifDir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.ifDirective, ifDir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_noOp) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_NoOpDirective noOp;
    add_NO_OP();
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.noOp, noOp);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, should succeed
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
}

TEST_F(FpySequencerTester, deserialize_storeTlmVal) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_StoreTlmValDirective dir(123, 0);
    add_STORE_TLM_VAL(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.storeTlmVal, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_storePrm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_StorePrmDirective dir(123, 456);
    add_STORE_PRM(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.storePrm, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_stackOp) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_StackOpDirective dir(Fpy::DirectiveId::AND);
    add_STACK_OP(Fpy::DirectiveId::AND);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.stackOp, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    // caught two bugs (one here, and it reminded me of this somewhere else)
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it succeeds
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(0);
}

TEST_F(FpySequencerTester, deserialize_exit) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_ExitDirective dir;
    add_EXIT();
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.exit, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it succeeds
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(0);
}

TEST_F(FpySequencerTester, deserialize_discard) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_DiscardDirective dir(123);
    add_DISCARD(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.discard, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_stackCmd) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_StackCmdDirective dir(123);
    add_STACK_CMD(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.stackCmd, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_memCmp) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_MemCmpDirective dir(123);
    add_MEMCMP(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.memCmp, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serialize(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}
// caught a bug
TEST_F(FpySequencerTester, checkTimers) {
    allocMem();
    add_PUSH_VAL<U32>(10);
    add_PUSH_VAL<U32>(0);
    add_WAIT_REL();
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
    ASSERT_EQ(this->tester_getState(), State::RUNNING_SLEEPING);

    time = Fw::Time(10, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    // should leave sleep
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
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
    ASSERT_EQ(this->tester_getState(), State::RUNNING_SLEEPING);
    time = Fw::Time(15, 0);
    setTestTime(time);
    invoke_to_checkTimers(0, 0);
    dispatchUntilState(State::IDLE);
    // timed out, should give exec error
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(FpySequencerTester, ping) {
    invoke_to_pingIn(0, 0);
    this->tester_doDispatch();
    ASSERT_from_pingOut_SIZE(1);
}

TEST_F(FpySequencerTester, cmdResponse) {
    invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    this->tester_doDispatch();
    ASSERT_EVENTS_CmdResponseWhileNotRunningSequence_SIZE(1);
    clearHistory();

    allocMem();
    add_CONST_CMD(123);
    writeToFile("test.bin");
    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    // once we're here, we should have just added the cmd dir to the queue
    this->tester_doDispatch();
    // dispatch once more to execute the cmd dir, sending out the command

    // should be 256 for seq idx and 256 for cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x01000100, Fw::CmdResponse::OK);
    // should be successful
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
    clearHistory();
    // let's try that again but with a command that fails
    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);

    invoke_to_cmdResponseIn(0, 123, 0x010000FF, Fw::CmdResponse::EXECUTION_ERROR);
    dispatchUntilState(State::IDLE);
    // should fail seq
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    // send wrong cmd uid
    // should be 256 for seq idx and 256 for cmd idx
    // but we're gonna send 255 for seq idx and 256 for cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x00FF0100, Fw::CmdResponse::OK);
    // should fail on seq idx, but should stay in running
    dispatchCurrentMessages(cmp);
    ASSERT_EQ(this->tester_getState(), State::RUNNING_AWAITING_STATEMENT_RESPONSE);

    // okay now send right seq idx but wrong cmd idx
    invoke_to_cmdResponseIn(0, 123, 0x01000101, Fw::CmdResponse::OK);
    // should fail on cmd idx and go back to IDLE
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    // okay now have a command response come in from this seq
    // while sleeping (coding err)
    clearSeq();
    add_PUSH_VAL<U32>(10);
    add_PUSH_VAL<U32>(0);
    add_WAIT_REL();
    add_CONST_CMD(123);
    writeToFile("test.bin");
    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_SLEEPING);
    invoke_to_cmdResponseIn(0, 123, 0x01000100, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();

    // okay now have the wrong opcode come in
    clearSeq();
    add_CONST_CMD(123);
    writeToFile("test.bin");
    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    invoke_to_cmdResponseIn(0, 456, 0x01000100, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(FpySequencerTester, tlmWrite) {
    invoke_to_tlmWrite(0, 0);
    this->tester_doDispatch();
    // make sure that all tlm is written every call
    ASSERT_TLM_SIZE(10);
}

TEST_F(FpySequencerTester, seqRunIn) {
    allocMem();
    add_NO_OP();
    writeToFile("test.bin");

    invoke_to_seqRunIn(0, Fw::String("test.bin"));
    this->tester_doDispatch();
    dispatchUntilState(State::VALIDATING);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    dispatchUntilState(State::IDLE);

    ASSERT_from_seqStartOut_SIZE(1);
    ASSERT_from_seqStartOut(0, Fw::String("test.bin"));
    ASSERT_from_seqDoneOut_SIZE(1);
    ASSERT_from_seqDoneOut(0, 0, 0, Fw::CmdResponse::OK);

    this->clearHistory();

    // try running while already running
    this->tester_setState(State::RUNNING_DISPATCH_STATEMENT);
    invoke_to_seqRunIn(0, Fw::String("test.bin"));
    // dispatch cmd
    this->tester_doDispatch();
    ASSERT_EVENTS_InvalidSeqRunCall_SIZE(1);
    removeFile("test.bin");
}

}  // namespace Svc
