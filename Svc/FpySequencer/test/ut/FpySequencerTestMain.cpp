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
using DirectiveError = Fpy::DirectiveErrorCode;

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
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
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

TEST_F(FpySequencerTester, pushTlmVal) {
    FpySequencer_PushTlmValDirective directive(456);
    nextTlmId = 456;
    nextTlmValue.setBuffLen(2);
    nextTlmValue.getBuffAddr()[0] = 200;
    nextTlmValue.getBuffAddr()[1] = 100;
    nextTlmTime.set(888, 777);
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->stack.size = 1;
    Signal result = tester_pushTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_from_getTlmChan(0, 456, Fw::Time(), Fw::TlmBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], nextTlmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], nextTlmValue.getBuffAddr()[1]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, nextTlmValue.getSize() + 1);
    clearHistory();

    // try getting a nonexistent chan
    directive.set_chanId(111);
    result = tester_pushTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::TLM_CHAN_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.set_chanId(456);

    // try overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 1;
    result = tester_pushTlmVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
    err = DirectiveError::NO_ERROR;
}

TEST_F(FpySequencerTester, pushTlmValAndTime) {
    FpySequencer_PushTlmValAndTimeDirective directive(456);
    nextTlmId = 456;
    nextTlmValue.setBuffLen(1);
    nextTlmValue.getBuffAddr()[0] = 200;
    nextTlmTime.set(888, 777);
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->stack.size = 0;
    Signal result = tester_pushTlmValAndTime_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_from_getTlmChan(0, 456, Fw::Time(), Fw::TlmBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], nextTlmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, nextTlmValue.getSize() + Fw::Time::SERIALIZED_SIZE);
    Fw::Time deserTime;
    Fw::ExternalSerializeBuffer esb(tester_get_m_runtime_ptr()->stack.bytes + 1, Fw::Time::SERIALIZED_SIZE);
    esb.setBuffLen(Fw::Time::SERIALIZED_SIZE);
    ASSERT_EQ(esb.deserializeTo(deserTime), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(deserTime, nextTlmTime);
    clearHistory();

    // try getting a nonexistent chan
    directive.set_chanId(111);
    result = tester_pushTlmValAndTime_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::TLM_CHAN_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.set_chanId(456);

    // try overflow stack
    // should be one byte over
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - Fw::Time::SERIALIZED_SIZE;
    result = tester_pushTlmValAndTime_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
    err = DirectiveError::NO_ERROR;
}

TEST_F(FpySequencerTester, pushPrm) {
    FpySequencer_PushPrmDirective directive(456);
    nextPrmId = 456;
    nextPrmValue.setBuffLen(2);
    nextPrmValue.getBuffAddr()[0] = 200;
    nextPrmValue.getBuffAddr()[1] = 100;
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->stack.size = 1;
    Signal result = tester_pushPrm_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_from_getParam(0, 456, Fw::ParamBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], nextPrmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], nextPrmValue.getBuffAddr()[1]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, nextPrmValue.getSize() + 1);
    clearHistory();

    // try getting a nonexistent param
    directive.set_prmId(111);
    result = tester_pushPrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::PRM_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.set_prmId(456);

    // try stack overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 1;
    result = tester_pushPrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
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
    ASSERT_EQ(expected.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND)),
              Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serializeFrom(directive.get_opCode()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serializeFrom(data, sizeof(data), Fw::Serialization::OMIT_LENGTH),
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
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test NE (not equal)
    FpySequencer_StackOpDirective directiveNE(Fpy::DirectiveId::INE);
    tester_push<I64>(10);
    tester_push<I64>(20);
    result = tester_stackOp_directiveHandler(directiveNE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test OR (bitwise OR)
    FpySequencer_StackOpDirective directiveOR(Fpy::DirectiveId::OR);
    tester_push<U8>(5);   // 0b0101;
    tester_push<U8>(10);  // 0b1010;
    result = tester_stackOp_directiveHandler(directiveOR, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 15);  // 0b1111
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);       // 0b1111
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test AND (bitwise AND)
    FpySequencer_StackOpDirective directiveAND(Fpy::DirectiveId::AND);
    tester_push<U8>(12);  // 0b1100;
    tester_push<U8>(10);  // 0b1010;
    result = tester_stackOp_directiveHandler(directiveAND, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 8);  // 0b1111
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);      // 0b1111
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test signed comparison (SLT - signed less than)
    FpySequencer_StackOpDirective directiveSLT(Fpy::DirectiveId::SLT);
    tester_push<I64>(-5);
    tester_push<I64>(10);
    result = tester_stackOp_directiveHandler(directiveSLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test unsigned comparison (ULT - unsigned less than)
    FpySequencer_StackOpDirective directiveULT(Fpy::DirectiveId::ULT);
    tester_push<I64>(5);
    tester_push<I64>(10);
    result = tester_stackOp_directiveHandler(directiveULT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test unsigned comparison (UGT - unsigned greater than)
    FpySequencer_StackOpDirective directiveUGT(Fpy::DirectiveId::UGT);
    tester_push<I64>(10);
    tester_push<I64>(5);
    result = tester_stackOp_directiveHandler(directiveUGT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test floating-point comparison (FLT - floating-point less than)
    FpySequencer_StackOpDirective directiveFLT(Fpy::DirectiveId::FLT);
    tester_push<F64>(5.5);
    tester_push<F64>(10.1);
    result = tester_stackOp_directiveHandler(directiveFLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test floating-point comparison (FGE - floating-point greater or equal)
    FpySequencer_StackOpDirective directiveFGE(Fpy::DirectiveId::FGE);
    tester_push<F64>(10.1);
    tester_push<F64>(10.1);
    result = tester_stackOp_directiveHandler(directiveFGE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test out-of-bounds stack access
    FpySequencer_StackOpDirective directiveOOB(Fpy::DirectiveId::IEQ);
    result = tester_stackOp_directiveHandler(directiveOOB, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test invalid operation
    FpySequencer_StackOpDirective directiveInvalid(Fpy::DirectiveId::NO_OP);
    ASSERT_DEATH_IF_SUPPORTED(tester_stackOp_directiveHandler(directiveInvalid, err), "Assert: ");
}

TEST_F(FpySequencerTester, ieq) {
    tester_push<I64>(-1);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_ieq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<I64>(-1);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_ieq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, ine) {
    tester_push<I64>(-1);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_ine(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<I64>(-1);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_ine(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, or) {
    tester_push<U8>(true);
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(true);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(false);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_or(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, and) {
    tester_push<U8>(false);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(true);
    tester_push<U8>(false);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(true);
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_and(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, ult) {
    tester_push<U64>(0);
    tester_push<U64>(std::numeric_limits<U64>::max());
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ult(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, ule) {
    tester_push<U64>(0);
    tester_push<U64>(std::numeric_limits<U64>::max());
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(2);
    tester_push<U64>(1);
    ASSERT_EQ(tester_op_ule(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, ugt) {
    tester_push<U64>(std::numeric_limits<U64>::max());
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(1);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_ugt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, uge) {
    tester_push<U64>(std::numeric_limits<U64>::max());
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(0);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(1);
    tester_push<U64>(0);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<U64>(1);
    tester_push<U64>(2);
    ASSERT_EQ(tester_op_uge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, slt) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(1);
    ASSERT_EQ(tester_op_slt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, sle) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, sgt) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, sge) {
    tester_push<I64>(0);
    tester_push<I64>(std::numeric_limits<I64>::max());
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(0);
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<I64>(0);
    tester_push<I64>(-1);
    ASSERT_EQ(tester_op_sge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, flt) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_flt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, fle) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fle(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, fgt) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fgt(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, fge) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fge(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, feq) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_feq(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
}

TEST_F(FpySequencerTester, fne) {
    tester_push<F64>(0.0);
    tester_push<F64>(std::numeric_limits<F64>::max());
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(0.0);
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(0.0);
    tester_push<F64>(-1.0);
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
    tester_get_m_runtime_ptr()->stack.size = 0;

    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    tester_push<F64>(std::numeric_limits<F64>::quiet_NaN());
    ASSERT_EQ(tester_op_fne(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);
}

TEST_F(FpySequencerTester, not) {
    tester_push<U8>(true);
    ASSERT_EQ(tester_op_not(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);
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

TEST_F(FpySequencerTester, add) {
    tester_push<I64>(100);
    tester_push<I64>(23);
    ASSERT_EQ(tester_op_add(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, sub) {
    tester_push<I64>(150);
    tester_push<I64>(27);
    ASSERT_EQ(tester_op_sub(), DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_pop<I64>(), 123);
}

TEST_F(FpySequencerTester, mul) {
    tester_push<I64>(41);
    tester_push<I64>(3);
    ASSERT_EQ(tester_op_mul(), DirectiveError::NO_ERROR);
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
    tester_push<U8>(0);
    Signal result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_header().get_statementCount(), 123);

    tester_push<U8>(123);
    result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::EXIT_WITH_ERROR);
    ASSERT_EVENTS_SequenceExitedWithError_SIZE(1);
    ASSERT_EVENTS_SequenceExitedWithError(0, tester_get_m_sequenceFilePath().toChar(), 123);
}

TEST_F(FpySequencerTester, discard) {
    // Test discarding 4 bytes
    tester_push<U32>(0x12345678);
    FpySequencer_DiscardDirective directive(4);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_discard_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 0);

    // Test error case - try to discard more bytes than available
    tester_push<U8>(0x12);
    directive.set_size(2);
    result = tester_discard_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
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
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 0);

    // Test error case - not enough bytes on stack
    tester_push<U8>(0x12);
    result = tester_stackCmd_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: argsSize near max U32
    // If we use naive check (stack.size < argsSize + sizeof(FwOpcodeType)),
    // argsSize = 0xFFFFFFFC would cause addition with 4 to overflow to 0,
    // making the check (0 < 0) pass incorrectly.
    directive = FpySequencer_StackCmdDirective(static_cast<Fpy::StackSizeType>(0xFFFFFFFC));
    tester_get_m_runtime_ptr()->stack.size = 0;
    result = tester_stackCmd_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
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
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: size * 2 would overflow U32
    // With size = 0x80000001, size * 2 = 0x100000002 which truncates to 0x2 in U32
    // This would make the check (stack.size < 2) pass when it shouldn't
    directive.set_size(static_cast<Fpy::StackSizeType>(0x80000001));
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE;
    result = tester_memCmp_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, setFlag) {
    FpySequencer_SetFlagDirective directive(0);
    tester_push<U8>(1);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_setFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_TRUE(tester_get_m_runtime_ptr()->flags[static_cast<Fpy::FlagId::T>(0)]);

    // Test setting flag to false
    tester_push<U8>(0);
    result = tester_setFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_FALSE(tester_get_m_runtime_ptr()->flags[static_cast<Fpy::FlagId::T>(0)]);

    // Test invalid flag index
    directive.set_flagIdx(Fpy::FLAG_COUNT);
    tester_push<U8>(1);
    result = tester_setFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::FLAG_IDX_OUT_OF_BOUNDS);
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Test stack underflow
    directive.set_flagIdx(0);
    result = tester_setFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, getFlag) {
    tester_get_m_runtime_ptr()->flags[0] = true;
    FpySequencer_GetFlagDirective directive(0);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_getFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 1);

    // reset stack
    tester_get_m_runtime_ptr()->stack.size = 0;
    // test getting false flag
    tester_get_m_runtime_ptr()->flags[0] = false;
    result = tester_getFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 1);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0);

    // Test invalid flag index
    directive.set_flagIdx(Fpy::FLAG_COUNT);
    result = tester_getFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::FLAG_IDX_OUT_OF_BOUNDS);

    // Test stack overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE;
    directive.set_flagIdx(0);
    result = tester_getFlag_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

TEST_F(FpySequencerTester, pushTime) {
    FpySequencer_PushTimeDirective directive;
    DirectiveError err = DirectiveError::NO_ERROR;
    Fw::Time testTime(TimeBase::TB_WORKSTATION_TIME, 0, 100, 100);
    setTestTime(testTime);
    tester_get_m_runtime_ptr()->stack.size = 0;
    Signal result = tester_pushTime_directiveHandler(directive, err);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // make sure deser'd time is same as input time
    Fw::Time deserTime;
    Fw::ExternalSerializeBuffer esb(tester_get_m_runtime_ptr()->stack.bytes, Fw::Time::SERIALIZED_SIZE);
    esb.setBuffLen(Fw::Time::SERIALIZED_SIZE);
    ASSERT_EQ(esb.deserializeTo(deserTime), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(deserTime, testTime);
    clearHistory();

    // check almost overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - Fw::Time::SERIALIZED_SIZE;
    result = tester_pushTime_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, Fpy::MAX_STACK_SIZE);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // check overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - Fw::Time::SERIALIZED_SIZE + 1;
    result = tester_pushTime_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

TEST_F(FpySequencerTester, getField) {
    FpySequencer_GetFieldDirective directive(4, 2);  // parent size 3, member size 2
    tester_push<U8>(123);
    tester_push<U8>(45);
    tester_push<U8>(67);
    tester_push<U8>(99);
    tester_push<Fpy::StackSizeType>(1);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 2);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 45);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], 67);

    // Test parent out of bounds
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U64>(0);
    tester_push<Fpy::StackSizeType>(0);
    directive = FpySequencer_GetFieldDirective(20, 1);
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test member out of bounds
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U64>(0);
    tester_push<Fpy::StackSizeType>(0);
    directive = FpySequencer_GetFieldDirective(1, 6);
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // test not enough bytes
    tester_get_m_runtime_ptr()->stack.size = 0;
    directive = FpySequencer_GetFieldDirective(0, 0);
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: offset + memberSize > parentSize
    directive = FpySequencer_GetFieldDirective(100, 50);  // parentSize=100, memberSize=50
    tester_get_m_runtime_ptr()->stack.size = 0;
    for (int i = 0; i < 100; i++) {
        tester_push<U8>(static_cast<U8>(i));
    }
    tester_push<Fpy::StackSizeType>(60);  // 60 + 50 = 110 > 100
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test U32 overflow protection: offset + memberSize could overflow
    // With offset = 0xFFFFFFFF and memberSize = 10, offset + memberSize would wrap to 9
    // making the check (9 > parentSize) potentially pass incorrectly
    directive = FpySequencer_GetFieldDirective(100, 10);  // parentSize=100, memberSize=10
    tester_get_m_runtime_ptr()->stack.size = 0;
    for (int i = 0; i < 100; i++) {
        tester_push<U8>(static_cast<U8>(i));
    }
    tester_push<Fpy::StackSizeType>(0xFFFFFFFF);  // offset that would overflow with memberSize
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test insufficient stack after popping offset
    directive = FpySequencer_GetFieldDirective(100, 10);  // parentSize=100, memberSize=10
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<Fpy::StackSizeType>(0);  // Only 4 bytes, not enough for parentSize=100
    result = tester_getField_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, storeRel) {
    // Test storing 2 bytes at offset 4 from stack_frame_start
    FpySequencer_StoreRelDirective directive(2);  // size = 2
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;  // frame starts at 0
    tester_push<U8>(123);                                     // value byte 1
    tester_push<U8>(100);                                     // value byte 2
    tester_push<Fpy::SignedStackSizeType>(4);                 // lvar_offset = 4 (signed)
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);  // size after pop of offset and value
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 123);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 100);

    // Test with negative offset (accessing function arguments)
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;  // frame starts at 4
    tester_push<U8>(55);                                      // value byte 1
    tester_push<U8>(66);                                      // value byte 2
    tester_push<Fpy::SignedStackSizeType>(-4);                // lvar_offset = -4 (negative, points to bytes 0-1)
    err = DirectiveError::NO_ERROR;
    result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 55);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], 66);

    // Test out of bounds storage (address too high)
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    tester_push<U8>(123);
    tester_push<U8>(100);
    tester_push<Fpy::SignedStackSizeType>(100);  // offset too high
    directive.set_size(2);
    result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test negative offset going below zero
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 2;  // frame at 2
    tester_push<U8>(123);
    tester_push<U8>(100);
    tester_push<Fpy::SignedStackSizeType>(-10);  // offset would result in negative address
    directive.set_size(2);
    result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test stack underflow (not enough bytes for value and offset)
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    directive.set_size(100);  // request more bytes than available
    result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: size near max
    directive.set_size(Fpy::MAX_STACK_SIZE);
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    result = tester_storeRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, peek) {
    FpySequencer_PeekDirective directive(0);
    // start by testing without pushing args to stack
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
    // okay now try duplicating two bytes on the top of the stack
    tester_push<U8>(12);
    tester_push<U8>(34);
    // push size
    tester_push<Fpy::StackSizeType>(2);
    // push offset
    tester_push<Fpy::StackSizeType>(0);
    err = DirectiveError::NO_ERROR;
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 4);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], 34);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[3], 34);

    // okay now try duplicating two bytes at the bottom of the stack
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(12);
    tester_push<U8>(34);
    tester_push<U8>(0);
    tester_push<U8>(0);
    // push size
    tester_push<Fpy::StackSizeType>(2);
    // push offset
    tester_push<Fpy::StackSizeType>(2);
    err = DirectiveError::NO_ERROR;
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 6);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], 34);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], 0);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[3], 0);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 34);

    // test offset > stack size
    tester_get_m_runtime_ptr()->stack.size = 10;
    // push size
    tester_push<Fpy::StackSizeType>(0);
    // push offset
    tester_push<Fpy::StackSizeType>(11);
    err = DirectiveError::NO_ERROR;
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // test offset == stack size and byte count == 0
    tester_get_m_runtime_ptr()->stack.size = 10;
    // push size
    tester_push<Fpy::StackSizeType>(0);
    // push offset
    tester_push<Fpy::StackSizeType>(10);
    err = DirectiveError::NO_ERROR;
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);

    // Test stack overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 1 - sizeof(Fpy::StackSizeType) * 2;
    // push size
    tester_push<Fpy::StackSizeType>(2 + sizeof(Fpy::StackSizeType) * 2);
    // push offset
    tester_push<Fpy::StackSizeType>(0);
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);

    // Test stack underflow
    // push size
    tester_push<Fpy::StackSizeType>(2);
    // push offset
    tester_push<Fpy::StackSizeType>(0);
    tester_get_m_runtime_ptr()->stack.size = 0;
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: byteCount exceeds available stack space
    tester_get_m_runtime_ptr()->stack.size = 100;
    tester_push<Fpy::StackSizeType>(Fpy::MAX_STACK_SIZE);  // byteCount - way too large
    tester_push<Fpy::StackSizeType>(100);                  // offset = stack.size after pops (allowed)
    result = tester_peek_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

TEST_F(FpySequencerTester, call) {
    // Set up a basic call scenario
    FpySequencer_CallDirective directive(0);  // empty U8 placeholder, target is popped from stack
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;  // existing frame at offset 4

    // Set up a sequence with 20 statements for bounds checking
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(20);

    // Set current statement index
    tester_get_m_runtime_ptr()->nextStatementIndex = 5;

    // Push target address (U32) - call to statement 10
    tester_push<U32>(10);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_call_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // Check that return address (5) and old frame pointer (4) were pushed
    // Stack should now have: [old_frame_ptr(4 bytes), return_addr(4 bytes)]
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 8);

    // Check nextStatementIndex was set to target
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 10);

    // Check new frame start is at old stack size (0 + 8 = 8)
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.currentFrameStart, 8);

    // Test stack underflow (no target address)
    tester_get_m_runtime_ptr()->stack.size = 0;
    err = DirectiveError::NO_ERROR;
    result = tester_call_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test target out of bounds
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(5);
    tester_push<U32>(10);  // target 10 is out of bounds for 5 statements
    result = tester_call_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STMT_OUT_OF_BOUNDS);

    // Test overflow protection: stack near max when pushing frame ptr + return addr
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(100);
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U32>(10);                                          // target address at position 0-3
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE;  // artificially set to max
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    result = tester_call_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

TEST_F(FpySequencerTester, returnDirective) {
    // Set up a return scenario
    FpySequencer_ReturnDirective directive(4, 8);  // return_val_size = 4, call_args_size = 8

    // Set up a sequence with enough statements for return address
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(20);

    // Initialize stack with:
    // [args (8 bytes)] [return_addr (4 bytes)] [old_frame_ptr (4 bytes)] [locals...] [return_val (4 bytes)]
    // Note: CALL pushes return address first, then frame pointer
    tester_get_m_runtime_ptr()->stack.size = 0;

    // Push 8 bytes of args
    tester_push<U64>(0x1122334455667788);

    // Push return address (statement 15) - pushed first by CALL
    tester_push<U32>(15);

    // Push old frame pointer (offset 2) - pushed second by CALL
    tester_push<U32>(2);

    // Stack frame starts here (after saved frame ptr)
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 16;  // 8 + 4 + 4 = 16

    // Push some locals (let's say 4 bytes)
    tester_push<U32>(0xDEADBEEF);

    // Push return value (4 bytes) - this is the value we want to keep
    tester_push<U32>(0x12345678);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // After return:
    // - Stack should have just the return value (4 bytes)
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 4);

    // Check return value bytes are at the bottom (stack stores big-endian)
    // 0x12345678 stored big-endian is: 0x12, 0x34, 0x56, 0x78
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[0], 0x12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[1], 0x34);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], 0x56);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[3], 0x78);

    // Check frame pointer was restored
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.currentFrameStart, 2);

    // Check next statement index was set to return address
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 15);

    // Test that return fails gracefully if return address is invalid (out of bounds)
    directive = FpySequencer_ReturnDirective(0, 0);  // return_val_size = 0, call_args_size = 0

    // Set up a sequence with only 10 statements
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(10);

    tester_get_m_runtime_ptr()->stack.size = 0;

    // Push an invalid return address (statement 100, but only 10 statements exist)
    tester_push<U32>(100);

    // Push old frame pointer
    tester_push<U32>(0);

    // Stack frame starts here
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 8;

    err = DirectiveError::NO_ERROR;
    result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STMT_OUT_OF_BOUNDS);

    // Test that return fails gracefully if saved frame pointer is invalid
    directive = FpySequencer_ReturnDirective(0, 0);  // return_val_size = 0, call_args_size = 0

    // Set up a sequence with enough statements
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(100);

    tester_get_m_runtime_ptr()->stack.size = 0;

    // Push a valid return address
    tester_push<U32>(5);

    // Push an invalid saved frame pointer (points beyond where stack will be after popping)
    // After popping return addr and frame ptr, stack size will be 0
    // So a saved frame ptr of 100 is invalid
    tester_push<U32>(100);

    // Stack frame starts here
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 8;

    err = DirectiveError::NO_ERROR;
    result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::FRAME_START_OUT_OF_BOUNDS);

    // Test that return fails gracefully if currentFrameStart is invalid (beyond stack size)
    directive = FpySequencer_ReturnDirective(0, 0);  // return_val_size = 0, call_args_size = 0

    tester_get_m_runtime_ptr()->stack.size = 8;

    // Push return address and frame pointer
    tester_push<U32>(5);
    tester_push<U32>(0);

    // Set currentFrameStart beyond current stack size (invalid)
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 100;

    err = DirectiveError::NO_ERROR;
    result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::FRAME_START_OUT_OF_BOUNDS);

    // Test overflow protection: returnValSize > MAX_STACK_SIZE
    Fpy::StackSizeType oversizedReturnVal = Fpy::MAX_STACK_SIZE + 1;
    directive = FpySequencer_ReturnDirective(oversizedReturnVal, 0);
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: discarding more args than available
    tester_get_m_sequenceObj_ptr()->get_header().set_statementCount(100);
    tester_get_m_runtime_ptr()->stack.size = 0;
    for (int i = 0; i < 20; i++) {
        tester_push<U8>(static_cast<U8>(i));
    }
    tester_push<U32>(5);                 // return address
    tester_push<Fpy::StackSizeType>(0);  // saved frame pointer
    tester_get_m_runtime_ptr()->stack.currentFrameStart = tester_get_m_runtime_ptr()->stack.size;
    for (int i = 0; i < 10; i++) {
        tester_push<U8>(static_cast<U8>(i));
    }
    directive = FpySequencer_ReturnDirective(10, 50);  // try to discard 50 args but only 20 available
    result = tester_return_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, loadAbs) {
    // Set up a load abs scenario
    FpySequencer_LoadAbsDirective directive(4, 2);  // globalOffset = 4, size = 2

    tester_get_m_runtime_ptr()->stack.size = 10;
    // Put some values in the stack at offset 4
    tester_get_m_runtime_ptr()->stack.bytes[4] = 0xAB;
    tester_get_m_runtime_ptr()->stack.bytes[5] = 0xCD;

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_loadAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // Check bytes were pushed
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[10], 0xAB);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[11], 0xCD);

    // Test out of bounds (offset beyond stack)
    tester_get_m_runtime_ptr()->stack.size = 3;
    directive = FpySequencer_LoadAbsDirective(4, 2);  // offset 4 is beyond size 3
    result = tester_loadAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test stack overflow
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 1;
    directive = FpySequencer_LoadAbsDirective(0, 2);  // would overflow
    result = tester_loadAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);

    // Test overflow protection: globalOffset + size overflow
    directive = FpySequencer_LoadAbsDirective(Fpy::MAX_STACK_SIZE, 100);  // offset near max
    tester_get_m_runtime_ptr()->stack.size = 1000;
    result = tester_loadAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, storeAbs) {
    // Set up a store abs scenario
    FpySequencer_StoreAbsDirective directive(2);  // size = 2

    tester_get_m_runtime_ptr()->stack.size = 10;

    // Push value to store
    tester_push<U8>(0xEE);
    tester_push<U8>(0xFF);

    // Push global offset
    tester_push<Fpy::StackSizeType>(4);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_storeAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // Check bytes were stored at offset 4
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 0xEE);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 0xFF);

    // Stack size should be back to what it was before pushes
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);

    // Test out of bounds (offset too high)
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(0x11);
    tester_push<U8>(0x22);
    tester_push<Fpy::StackSizeType>(100);  // offset way too high
    result = tester_storeAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test stack underflow (not enough bytes)
    tester_get_m_runtime_ptr()->stack.size = 0;
    directive.set_size(10);
    result = tester_storeAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: size near max
    directive.set_size(Fpy::MAX_STACK_SIZE);
    tester_get_m_runtime_ptr()->stack.size = 10;
    result = tester_storeAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);
}

TEST_F(FpySequencerTester, storeAbsConstOffset) {
    // Set up a store abs const offset scenario
    FpySequencer_StoreAbsConstOffsetDirective directive(4, 2);  // globalOffset = 4, size = 2

    tester_get_m_runtime_ptr()->stack.size = 10;

    // Push value to store
    tester_push<U8>(0xAA);
    tester_push<U8>(0xBB);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_storeAbsConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);

    // Check bytes were stored at offset 4
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 0xAA);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 0xBB);

    // Stack size should be back to what it was before push
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);

    // Test out of bounds (offset + size > stack size after pop)
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(0x11);
    tester_push<U8>(0x22);
    directive = FpySequencer_StoreAbsConstOffsetDirective(100, 2);  // offset too high
    result = tester_storeAbsConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test stack underflow
    tester_get_m_runtime_ptr()->stack.size = 0;
    directive = FpySequencer_StoreAbsConstOffsetDirective(0, 10);
    result = tester_storeAbsConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test overflow protection: globalOffset near max
    directive = FpySequencer_StoreAbsConstOffsetDirective(Fpy::MAX_STACK_SIZE, 100);
    tester_get_m_runtime_ptr()->stack.size = 100;
    result = tester_storeAbsConstOffset_directiveHandler(directive, err);
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

TEST_F(FpySequencerTester, cmd_CLEAR_BREAKPOINT) {
    tester_get_m_breakpoint_ptr()->breakpointInUse = true;
    sendCmd_CLEAR_BREAKPOINT(0, 0);
    // dispatch cmd
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_CLEAR_BREAKPOINT(), 0, Fw::CmdResponse::OK);
    // dispatch signal
    this->tester_doDispatch();
    ASSERT_FALSE(tester_get_m_breakpoint_ptr()->breakpointInUse);
}

TEST_F(FpySequencerTester, cmd_SET_BREAKPOINT) {
    sendCmd_SET_BREAKPOINT(0, 0, 123, true);
    // dispatch cmd handler
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should always work
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_SET_BREAKPOINT(), 0, Fw::CmdResponse::OK);
    // dispatch signal
    this->tester_doDispatch();
    ASSERT_TRUE(tester_get_m_breakpoint_ptr()->breakpointInUse);
    ASSERT_TRUE(tester_get_m_breakpoint_ptr()->breakOnlyOnceOnBreakpoint);
    ASSERT_EQ(tester_get_m_breakpoint_ptr()->breakpointIndex, 123);
}

TEST_F(FpySequencerTester, cmd_BREAK) {
    // Test BREAK command in IDLE state (should fail)
    this->tester_setState(State::IDLE);
    tester_get_m_breakpoint_ptr()->breakBeforeNextLine = false;
    sendCmd_BREAK(0, 0);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should fail in IDLE state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_BREAK(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FALSE(tester_get_m_breakpoint_ptr()->breakBeforeNextLine);

    // Test BREAK command in RUNNING state (should succeed)
    this->clearHistory();
    this->tester_setState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    sendCmd_BREAK(0, 0);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should succeed in RUNNING state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_BREAK(), 0, Fw::CmdResponse::OK);
    // now dispatch the signal
    dispatchCurrentMessages(cmp);
    ASSERT_TRUE(tester_get_m_breakpoint_ptr()->breakBeforeNextLine);

    tester_get_m_breakpoint_ptr()->breakBeforeNextLine = false;
    // Test BREAK command in RUNNING_PAUSED state (should fail)
    this->clearHistory();
    this->tester_setState(State::RUNNING_PAUSED);
    sendCmd_BREAK(0, 0);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should fail in RUNNING_PAUSED state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_BREAK(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FALSE(tester_get_m_breakpoint_ptr()->breakBeforeNextLine);
}

TEST_F(FpySequencerTester, cmd_CONTINUE) {
    this->tester_setState(State::IDLE);
    sendCmd_CONTINUE(0, 0);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in IDLE
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_CONTINUE(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    this->tester_setState(State::RUNNING_PAUSED);
    sendCmd_CONTINUE(0, 0);
    // dispatch cmd handler
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in debug_broken
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_CONTINUE(), 0, Fw::CmdResponse::OK);
    // dispatch signal handler
    this->tester_doDispatch();
    // should have gone to dispatch stmt
    ASSERT_EQ(this->tester_getState(), State::RUNNING_DISPATCH_STATEMENT);
}

TEST_F(FpySequencerTester, cmd_SET_FLAG) {
    this->tester_setState(State::IDLE);
    sendCmd_SET_FLAG(0, 0, Svc::Fpy::FlagId::EXIT_ON_CMD_FAIL, false);
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should fail in IDLE
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_SET_FLAG(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->clearHistory();

    tester_get_m_runtime_ptr()->flags[Fpy::FlagId::EXIT_ON_CMD_FAIL] = false;

    // okay try setting in await stmt response
    this->tester_setState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    sendCmd_SET_FLAG(0, 0, Fpy::FlagId::EXIT_ON_CMD_FAIL, true);
    // dispatch cmd handler
    this->tester_doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    // should work in await stmt response
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_SET_FLAG(), 0, Fw::CmdResponse::OK);
    ASSERT_TRUE(tester_get_m_runtime_ptr()->flags[Fpy::FlagId::EXIT_ON_CMD_FAIL]);
}

TEST_F(FpySequencerTester, cmd_STEP) {
    // Test STEP command in IDLE state (should fail)
    this->tester_setState(State::IDLE);
    sendCmd_STEP(0, 0);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should fail in IDLE state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_STEP(), 0, Fw::CmdResponse::EXECUTION_ERROR);

    // Test STEP command in RUNNING state (should fail - must be paused)
    this->clearHistory();
    this->tester_setState(State::RUNNING_DISPATCH_STATEMENT);
    sendCmd_STEP(0, 0);
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should fail in RUNNING state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_STEP(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->tester_setState(State::IDLE);

    // Test STEP command in RUNNING_PAUSED state (should succeed)
    this->clearHistory();
    // Setup test sequence
    allocMem();
    add_NO_OP();  // Statement 0
    add_NO_OP();  // Statement 1
    add_NO_OP();  // Statement 2
    writeAndRun();

    // tell it to break before stmt 0
    tester_get_m_breakpoint_ptr()->breakpointInUse = true;
    tester_get_m_breakpoint_ptr()->breakpointIndex = 0;

    // run until we get to paused
    dispatchUntilState(State::RUNNING_PAUSED);

    // Send STEP command
    sendCmd_STEP(0, 0);
    // should go to dispatch stmt
    dispatchUntilState(State::RUNNING_DISPATCH_STATEMENT);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should succeed in RUNNING_PAUSED state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_STEP(), 0, Fw::CmdResponse::OK);

    // and then back to paused
    dispatchUntilState(State::RUNNING_PAUSED);
    // Should be ready to execute statement 1 next
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 1);
    ASSERT_FALSE(tester_get_m_breakpoint_ptr()->breakBeforeNextLine);

    this->clearHistory();
    // okay try stepping to end of seq
    tester_get_m_runtime_ptr()->nextStatementIndex = 3;
    // Send STEP command
    sendCmd_STEP(0, 0);
    // should go to dispatch stmt
    dispatchUntilState(State::RUNNING_DISPATCH_STATEMENT);
    // once it gets past here should end seq
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should succeed in RUNNING_PAUSED state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_STEP(), 0, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_EVENTS_SequenceDone_SIZE(1);
}

TEST_F(FpySequencerTester, cmd_DUMP_STACK_TO_FILE) {
    // Test command in IDLE state (should fail)
    this->tester_setState(State::IDLE);
    sendCmd_DUMP_STACK_TO_FILE(0, 0, Fw::String("test"));
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should fail in IDLE state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DUMP_STACK_TO_FILE(), 0,
                        Fw::CmdResponse::EXECUTION_ERROR);

    // Test STEP command in RUNNING_PAUSED state (should succeed)
    this->clearHistory();
    // Setup test sequence
    allocMem();
    add_PUSH_VAL<U8>(0x00);
    add_PUSH_VAL<U8>(0x11);
    add_PUSH_VAL<U8>(0x22);
    writeAndRun();

    // tell it to break before the end
    tester_get_m_breakpoint_ptr()->breakpointInUse = true;
    tester_get_m_breakpoint_ptr()->breakpointIndex = 3;

    // run until we get to paused
    dispatchUntilState(State::RUNNING_PAUSED);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 3);

    sendCmd_DUMP_STACK_TO_FILE(0, 0, Fw::String("output.bin"));
    dispatchCurrentMessages(cmp);
    ASSERT_CMD_RESPONSE_SIZE(1);
    // Should succeed in RUNNING_PAUSED state
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_DUMP_STACK_TO_FILE(), 0, Fw::CmdResponse::OK);

    // assert that the file was created and has the right data
    Os::File outputFile;
    ASSERT_EQ(outputFile.open("output.bin", Os::FileInterface::Mode::OPEN_READ), Os::File::Status::OP_OK);
    U8 bytes[3] = {};
    FwSizeType size = sizeof(bytes);
    ASSERT_EQ(outputFile.read(bytes, size), Os::File::Status::OP_OK);
    ASSERT_EQ(size, sizeof(bytes));
    ASSERT_EQ(bytes[0], 0x00);
    ASSERT_EQ(bytes[1], 0x11);
    ASSERT_EQ(bytes[2], 0x22);

    removeFile("output.bin");
}

TEST_F(FpySequencerTester, readHeader) {
    U8 seqBuf[Fpy::Header::SERIALIZED_SIZE] = {0};
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(seqBuf, sizeof(seqBuf));
    Fpy::Header header;
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    header.set_bodySize(50);
    header.set_schemaVersion(Fpy::SCHEMA_VERSION);
    header.set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(header), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(tester_readHeader(), Fw::Success::SUCCESS);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_header(), header);

    // check not enough bytes
    tester_get_m_sequenceBuffer_ptr()->resetDeser();
    tester_get_m_sequenceBuffer_ptr()->setBuffLen(tester_get_m_sequenceBuffer_ptr()->getSize() - 1);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileReadDeserializeError_SIZE(1);

    // check wrong schema version
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_schemaVersion(Fpy::SCHEMA_VERSION + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    header.set_schemaVersion(Fpy::SCHEMA_VERSION);
    clearHistory();

    // check too many args
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceArgs_SIZE(1);
    header.set_argumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);

    // check too many stmts
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.set_statementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceDirectives_SIZE(1);
}

TEST_F(FpySequencerTester, readBody) {
    U8 data[Fpy::MAX_SEQUENCE_ARG_COUNT + Fpy::MAX_SEQUENCE_STATEMENT_COUNT * Fpy::Statement::SERIALIZED_SIZE];

    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));
    // write some args mappings
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        // map arg idx ii to serReg pos 123
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(static_cast<U8>(123)),
                  Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // write some statements
    Fpy::Statement stmt(Fpy::DirectiveId::NO_OP, Fw::StatementArgBuffer());
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
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
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(static_cast<U8>(123)),
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
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(static_cast<U8>(123)),
                  Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    // the -1 here is the intended mistake
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT - 1; ii++) {
        ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(stmt), Fw::SerializeStatus::FW_SERIALIZE_OK);
    }
    ASSERT_EQ(tester_readBody(), Fw::Success::FAILURE);
}

TEST_F(FpySequencerTester, readFooter) {
    U8 data[Fpy::Footer::SERIALIZED_SIZE];

    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, sizeof(data));

    tester_set_m_computedCRC(0x12345678);
    Fpy::Footer footer(static_cast<U32>(~0x12345678));
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serializeFrom(footer), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(tester_readFooter(), Fw::Success::SUCCESS);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->get_footer(), footer);

    tester_get_m_sequenceBuffer_ptr()->resetDeser();
    // try wrong crc
    tester_set_m_computedCRC(0x44444444);
    ASSERT_EQ(tester_readFooter(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongCRC_SIZE(1);

    // try not enough remaining
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->setBuffLen(tester_get_m_sequenceBuffer_ptr()->getSize() - 1),
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
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getCapacity(), 100);
    cmp.deallocateBuffer(alloc);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getBuffAddr(), nullptr);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->getCapacity(), 0);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, should succeed
    seq.get_statements()[0].get_argBuf().resetSer();
    result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
}

TEST_F(FpySequencerTester, deserialize_pushTlmVal) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_PushTlmValDirective dir(123);
    add_PUSH_TLM_VAL(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.pushTlmVal, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_pushTlmValAndTime) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_PushTlmValAndTimeDirective dir(123);
    add_PUSH_TLM_VAL_AND_TIME(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.pushTlmValAndTime, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_pushPrm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_PushPrmDirective dir(123);
    add_PUSH_PRM(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.pushPrm, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_setFlag) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_SetFlagDirective dir(123);
    add_SET_FLAG(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.setFlag, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_getFlag) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetFlagDirective dir(123);
    add_GET_FLAG(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getFlag, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_getField) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetFieldDirective dir(123, 123);
    add_GET_FIELD(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getField, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_storeRel) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_StoreRelDirective dir(123);
    add_STORE_REL(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.storeRel, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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

TEST_F(FpySequencerTester, deserialize_peek) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_PeekDirective dir(0);
    add_PEEK(dir);
    Fw::Success result = tester_deserializeDirective(seq.get_statements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.peek, dir);
    // write some junk after buf, make sure it fails
    seq.get_statements()[0].get_argBuf().serializeFrom(123);
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
    ASSERT_TLM_SIZE(19);
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

TEST_F(FpySequencerTester, flag_EXIT_ON_CMD_FAIL) {
    // test a simple seq that fails because a cmd fails
    this->paramSet_FLAG_DEFAULT_EXIT_ON_CMD_FAIL(true, Fw::ParamValid::VALID);
    this->paramSend_FLAG_DEFAULT_EXIT_ON_CMD_FAIL(0, 0);
    this->clearHistory();
    allocMem();
    add_CONST_CMD(123);
    writeAndRun();
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    // okay now send in a failure
    invoke_to_cmdResponseIn(0, 123, 0x00010001, Fw::CmdResponse::EXECUTION_ERROR);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, 0, get_OPCODE_RUN(), Fw::CmdResponse::EXECUTION_ERROR);

    // now test that it doesn't fail if we set flag to false
    this->paramSet_FLAG_DEFAULT_EXIT_ON_CMD_FAIL(false, Fw::ParamValid::VALID);
    this->paramSend_FLAG_DEFAULT_EXIT_ON_CMD_FAIL(0, 0);
    this->clearHistory();
    // cmd is already in seq, can just rerun
    writeAndRun();
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    // okay now send in a failure
    invoke_to_cmdResponseIn(0, 123, 0x00020002, Fw::CmdResponse::EXECUTION_ERROR);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, 0, get_OPCODE_RUN(), Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Stack Unit Tests
// ----------------------------------------------------------------------

// Test push/pop template methods for various types
TEST_F(FpySequencerTester, StackPushPopU8) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U8 val = 0xAB;
    tester_push<U8>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(U8));

    U8 popped = tester_pop<U8>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopU16) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U16 val = 0xABCD;
    tester_push<U16>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(U16));

    U16 popped = tester_pop<U16>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopU32) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U32 val = 0xDEADBEEF;
    tester_push<U32>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(U32));

    U32 popped = tester_pop<U32>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopU64) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U64 val = 0xDEADBEEFCAFEBABE;
    tester_push<U64>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(U64));

    U64 popped = tester_pop<U64>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopI8) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    I8 val = -42;
    tester_push<I8>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(I8));

    I8 popped = tester_pop<I8>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopI16) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    I16 val = -12345;
    tester_push<I16>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(I16));

    I16 popped = tester_pop<I16>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopI32) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    I32 val = -123456789;
    tester_push<I32>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(I32));

    I32 popped = tester_pop<I32>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopI64) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    I64 val = -1234567890123456789LL;
    tester_push<I64>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(I64));

    I64 popped = tester_pop<I64>();
    ASSERT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopF32) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    F32 val = 3.14159f;
    tester_push<F32>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(F32));

    F32 popped = tester_pop<F32>();
    ASSERT_FLOAT_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

TEST_F(FpySequencerTester, StackPushPopF64) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    F64 val = 2.718281828459045;
    tester_push<F64>(val);
    ASSERT_EQ(runtime->stack.size, sizeof(F64));

    F64 popped = tester_pop<F64>();
    ASSERT_DOUBLE_EQ(popped, val);
    ASSERT_EQ(runtime->stack.size, 0);
}

// Test pushing/popping multiple values
TEST_F(FpySequencerTester, StackPushPopMultiple) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U32 val1 = 0x11111111;
    U32 val2 = 0x22222222;
    U32 val3 = 0x33333333;

    tester_push<U32>(val1);
    tester_push<U32>(val2);
    tester_push<U32>(val3);
    ASSERT_EQ(runtime->stack.size, 3 * sizeof(U32));

    // Pop in LIFO order
    ASSERT_EQ(tester_pop<U32>(), val3);
    ASSERT_EQ(tester_pop<U32>(), val2);
    ASSERT_EQ(tester_pop<U32>(), val1);
    ASSERT_EQ(runtime->stack.size, 0);
}

// Test byte array push/pop
TEST_F(FpySequencerTester, StackPushPopByteArray) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U8 src[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    runtime->stack.push(src, 10);
    ASSERT_EQ(runtime->stack.size, 10);

    U8 dest[10] = {0};
    runtime->stack.pop(dest, 10);
    ASSERT_EQ(runtime->stack.size, 0);

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(dest[i], src[i]);
    }
}

// Test pushZeroes
TEST_F(FpySequencerTester, StackPushZeroes) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    // Put some non-zero data first
    U8 nonZero[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(runtime->stack.bytes, nonZero, 4);

    runtime->stack.pushZeroes(4);
    ASSERT_EQ(runtime->stack.size, 4);

    // Verify the bytes are zero
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(runtime->stack.bytes[i], 0);
    }
}

// Test top() pointer
TEST_F(FpySequencerTester, StackTop) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    ASSERT_EQ(runtime->stack.top(), runtime->stack.bytes);

    runtime->stack.size = 10;
    ASSERT_EQ(runtime->stack.top(), runtime->stack.bytes + 10);

    runtime->stack.size = 100;
    ASSERT_EQ(runtime->stack.top(), runtime->stack.bytes + 100);
}

// Test copy with non-overlapping regions
TEST_F(FpySequencerTester, StackCopyNonOverlapping) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    // Set up source data at offset 0
    U8 srcData[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22};
    memcpy(runtime->stack.bytes, srcData, 8);
    runtime->stack.size = 16;  // 8 bytes source + 8 bytes dest space

    // Copy from offset 0 to offset 8
    runtime->stack.copy(8, 0, 8);

    // Verify both regions have the same data
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(runtime->stack.bytes[i], srcData[i]);
        ASSERT_EQ(runtime->stack.bytes[8 + i], srcData[i]);
    }
}

// Test copy with zero size (should be no-op)
TEST_F(FpySequencerTester, StackCopyZeroSize) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 16;

    // This should not crash or modify anything
    runtime->stack.copy(0, 8, 0);
}

// Test move with overlapping regions (dest < src)
TEST_F(FpySequencerTester, StackMoveOverlappingDestBeforeSrc) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 16;

    // Set up data: [0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, ...]
    memset(runtime->stack.bytes, 0, 16);
    runtime->stack.bytes[4] = 0xAA;
    runtime->stack.bytes[5] = 0xBB;
    runtime->stack.bytes[6] = 0xCC;
    runtime->stack.bytes[7] = 0xDD;

    // Move 4 bytes from offset 4 to offset 2 (overlapping)
    runtime->stack.move(2, 4, 4);

    // Verify the moved data
    ASSERT_EQ(runtime->stack.bytes[2], 0xAA);
    ASSERT_EQ(runtime->stack.bytes[3], 0xBB);
    ASSERT_EQ(runtime->stack.bytes[4], 0xCC);
    ASSERT_EQ(runtime->stack.bytes[5], 0xDD);
}

// Test move with overlapping regions (dest > src)
TEST_F(FpySequencerTester, StackMoveOverlappingDestAfterSrc) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 16;

    // Set up data: [0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, ...]
    memset(runtime->stack.bytes, 0, 16);
    runtime->stack.bytes[0] = 0xAA;
    runtime->stack.bytes[1] = 0xBB;
    runtime->stack.bytes[2] = 0xCC;
    runtime->stack.bytes[3] = 0xDD;

    // Move 4 bytes from offset 0 to offset 2 (overlapping)
    runtime->stack.move(2, 0, 4);

    // Verify the moved data
    ASSERT_EQ(runtime->stack.bytes[2], 0xAA);
    ASSERT_EQ(runtime->stack.bytes[3], 0xBB);
    ASSERT_EQ(runtime->stack.bytes[4], 0xCC);
    ASSERT_EQ(runtime->stack.bytes[5], 0xDD);
}

// Test move with zero size (should be no-op)
TEST_F(FpySequencerTester, StackMoveZeroSize) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 16;

    // This should not crash or modify anything
    runtime->stack.move(0, 8, 0);
}

// Test move non-overlapping (move should work like copy here)
TEST_F(FpySequencerTester, StackMoveNonOverlapping) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 16;

    // Set up source data at offset 0
    U8 srcData[4] = {0x11, 0x22, 0x33, 0x44};
    memcpy(runtime->stack.bytes, srcData, 4);
    memset(runtime->stack.bytes + 8, 0, 4);

    // Move from offset 0 to offset 8 (non-overlapping)
    runtime->stack.move(8, 0, 4);

    // Verify both source and dest
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(runtime->stack.bytes[i], srcData[i]);      // source unchanged
        ASSERT_EQ(runtime->stack.bytes[8 + i], srcData[i]);  // dest has copy
    }
}

// Test big-endian byte order in push/pop
TEST_F(FpySequencerTester, StackBigEndianByteOrder) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    // Push a known U32 value
    U32 val = 0x12345678;
    tester_push<U32>(val);

    // Verify big-endian byte order in stack bytes
    ASSERT_EQ(runtime->stack.bytes[0], 0x12);  // MSB first
    ASSERT_EQ(runtime->stack.bytes[1], 0x34);
    ASSERT_EQ(runtime->stack.bytes[2], 0x56);
    ASSERT_EQ(runtime->stack.bytes[3], 0x78);  // LSB last
}

// Test stack with mixed type operations
TEST_F(FpySequencerTester, StackMixedTypes) {
    auto* runtime = tester_get_m_runtime_ptr();
    runtime->stack.size = 0;

    U8 val8 = 0xAB;
    U16 val16 = 0xCDEF;
    U32 val32 = 0x12345678;

    tester_push<U8>(val8);
    tester_push<U16>(val16);
    tester_push<U32>(val32);

    ASSERT_EQ(runtime->stack.size, sizeof(U8) + sizeof(U16) + sizeof(U32));

    // Pop in reverse order
    ASSERT_EQ(tester_pop<U32>(), val32);
    ASSERT_EQ(tester_pop<U16>(), val16);
    ASSERT_EQ(tester_pop<U8>(), val8);
    ASSERT_EQ(runtime->stack.size, 0);
}

// Test allocate directive - allocates zeroed space on stack
TEST_F(FpySequencerTester, allocate) {
    FpySequencer_AllocateDirective directive(8);

    tester_get_m_runtime_ptr()->stack.size = 0;
    // Put some non-zero data in the stack buffer to verify zeroing
    memset(tester_get_m_runtime_ptr()->stack.bytes, 0xFF, 20);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_allocate_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 8);

    // Verify the allocated space is zeroed
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[i], 0);
    }

    // Allocate more on top
    directive.set_size(4);
    result = tester_allocate_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 12);

    // Verify newly allocated space is zeroed
    for (int i = 8; i < 12; i++) {
        ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[i], 0);
    }

    // Test overflow protection
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 50;
    directive.set_size(100);
    result = tester_allocate_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

// Test loadRel directive - loads from frame-relative offset to stack top
TEST_F(FpySequencerTester, loadRel) {
    // Set up stack with some data
    tester_get_m_runtime_ptr()->stack.size = 0;
    tester_push<U8>(0xAA);
    tester_push<U8>(0xBB);
    tester_push<U8>(0xCC);
    tester_push<U8>(0xDD);
    // Stack: [0xAA, 0xBB, 0xCC, 0xDD], size=4

    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;

    // Load 2 bytes from offset 1 (should get 0xBB, 0xCC)
    FpySequencer_LoadRelDirective directive(1, 2);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 6);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 0xBB);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 0xCC);

    // Test with frame start offset
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.bytes[4] = 0x11;
    tester_get_m_runtime_ptr()->stack.bytes[5] = 0x22;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;
    directive = FpySequencer_LoadRelDirective(0, 2);  // offset 0 from frame start = absolute 4
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[10], 0x11);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[11], 0x22);

    // Test negative offset (accessing call args before frame)
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.bytes[2] = 0x33;
    tester_get_m_runtime_ptr()->stack.bytes[3] = 0x44;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;
    directive = FpySequencer_LoadRelDirective(-2, 2);  // offset -2 from frame start = absolute 2
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 12);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[10], 0x33);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[11], 0x44);

    // Test negative offset going below zero
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 2;
    directive = FpySequencer_LoadRelDirective(-10, 2);  // would result in negative address
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test out of bounds (offset + size > stack.size)
    tester_get_m_runtime_ptr()->stack.size = 50;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    directive = FpySequencer_LoadRelDirective(0, 100);
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test overflow protection
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 50;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    directive = FpySequencer_LoadRelDirective(0, 100);
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);

    // Test addr exceeding MAX_STACK_SIZE (currentFrameStart + lvarOffset > MAX_STACK_SIZE)
    tester_get_m_runtime_ptr()->stack.size = 100;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = Fpy::MAX_STACK_SIZE;
    directive = FpySequencer_LoadRelDirective(1, 1);  // addr = MAX_STACK_SIZE + 1
    result = tester_loadRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

// Test storeRelConstOffset directive - stores stack top to frame-relative offset
TEST_F(FpySequencerTester, storeRelConstOffset) {
    // Set up stack with space and a value to store
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;

    // Push value to store
    tester_push<U8>(0xEE);
    tester_push<U8>(0xFF);
    // Stack size is now 12

    // Store 2 bytes at offset 4 from frame start
    FpySequencer_StoreRelConstOffsetDirective directive(4, 2);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);  // Size back to before push
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[4], 0xEE);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[5], 0xFF);

    // Test with frame start offset
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;
    tester_push<U8>(0x11);
    tester_push<U8>(0x22);
    directive = FpySequencer_StoreRelConstOffsetDirective(2, 2);  // offset 2 from frame start = absolute 6
    result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[6], 0x11);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[7], 0x22);

    // Test negative offset (storing to call args before frame)
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 4;
    tester_push<U8>(0x33);
    tester_push<U8>(0x44);
    directive = FpySequencer_StoreRelConstOffsetDirective(-2, 2);  // offset -2 from frame = absolute 2
    result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[2], 0x33);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[3], 0x44);

    // Test negative offset going below zero
    tester_get_m_runtime_ptr()->stack.size = 10;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 2;
    tester_push<U8>(0x55);
    tester_push<U8>(0x66);
    directive = FpySequencer_StoreRelConstOffsetDirective(-10, 2);  // would result in negative address
    result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);

    // Test out of bounds (dest + size > stack.size after pop)
    tester_get_m_runtime_ptr()->stack.size = 50;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = 0;
    directive = FpySequencer_StoreRelConstOffsetDirective(0, 100);
    result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_UNDERFLOW);

    // Test addr exceeding MAX_STACK_SIZE (currentFrameStart + lvarOffset > MAX_STACK_SIZE)
    tester_get_m_runtime_ptr()->stack.size = 100;
    tester_get_m_runtime_ptr()->stack.currentFrameStart = Fpy::MAX_STACK_SIZE;
    tester_push<U8>(0x77);
    directive = FpySequencer_StoreRelConstOffsetDirective(1, 1);  // addr = MAX_STACK_SIZE + 1
    result = tester_storeRelConstOffset_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_ACCESS_OUT_OF_BOUNDS);
}

// Test pushVal directive - pushes constant value onto stack
TEST_F(FpySequencerTester, pushVal) {
    U8 testData[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};
    FpySequencer_PushValDirective directive(0, 10);
    memcpy(directive.get_val(), testData, 10);

    tester_get_m_runtime_ptr()->stack.size = 0;

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_pushVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 10);

    // Verify the pushed data
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[i], testData[i]);
    }

    // Push more data on top
    U8 moreData[4] = {0xBB, 0xCC, 0xDD, 0xEE};
    FpySequencer_PushValDirective directive2(0, 4);
    memcpy(directive2.get_val(), moreData, 4);
    result = tester_pushVal_directiveHandler(directive2, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->stack.size, 14);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(tester_get_m_runtime_ptr()->stack.bytes[10 + i], moreData[i]);
    }

    // Test overflow protection
    tester_get_m_runtime_ptr()->stack.size = Fpy::MAX_STACK_SIZE - 5;
    directive.set__valSize(10);
    result = tester_pushVal_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STACK_OVERFLOW);
}

}  // namespace Svc
