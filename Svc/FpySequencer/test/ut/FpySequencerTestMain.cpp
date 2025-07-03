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
    FpySequencer_WaitRelDirective directive(5, 123);
    Fw::Time testTime(100, 100);
    setTestTime(testTime);

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_waitRel_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->wakeupTime, Fw::Time(105, 223));
}

TEST_F(FpySequencerTester, waitAbs) {
    FpySequencer_WaitAbsDirective directive(Fw::Time(5, 123));

    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_waitAbs_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_beginSleep);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->wakeupTime, Fw::Time(5, 123));
}

TEST_F(FpySequencerTester, goto) {
    FpySequencer_GotoDirective directive(123);
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(456);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 123);

    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    // out of bounds
    directive.setstatementIndex(111111);
    result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::STMT_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 0);

    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    // just inside bounds
    directive.setstatementIndex(456);
    result = tester_goto_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 456);
}

TEST_F(FpySequencerTester, setSerReg) {
    U8 buf[Fpy::MAX_SERIALIZABLE_REGISTER_SIZE];
    memset(buf, 1, sizeof(buf));
    FpySequencer_SetSerRegDirective directive(static_cast<U8>(0), 1, static_cast<FwSizeType>(sizeof(buf)));
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_setSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->serRegs[0].valueSize, sizeof(buf));
    ASSERT_EQ(memcmp(buf, tester_get_m_runtime_ptr()->serRegs[0].value, sizeof(buf)), 0);

    // outside of serReg range
    directive.setindex(Fpy::NUM_SERIALIZABLE_REGISTERS);
    result = tester_setSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_OUT_OF_BOUNDS);

    // check what happens if buf too big
    directive = FpySequencer_SetSerRegDirective(static_cast<U8>(0), 1, Fpy::MAX_SERIALIZABLE_REGISTER_SIZE + 1);

    ASSERT_DEATH_IF_SUPPORTED(tester_setSerReg_directiveHandler(directive, err), "Assert: ");
}

TEST_F(FpySequencerTester, if) {
    tester_get_m_runtime_ptr()->nextStatementIndex = 100;
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(123);
    tester_get_m_runtime_ptr()->regs[0] = 1;
    FpySequencer_IfDirective directive(0, 111);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    // should not have changed stmtidx
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 100);

    tester_get_m_runtime_ptr()->regs[0] = 0;  // set it to false
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    // should have changed stmtidx
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex, 111);

    tester_get_m_runtime_ptr()->nextStatementIndex = 100;

    directive.setfalseGotoStmtIndex(tester_get_m_sequenceObj_ptr()->getheader().getstatementCount());
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    // should have succeeded
    ASSERT_EQ(tester_get_m_runtime_ptr()->nextStatementIndex,
              tester_get_m_sequenceObj_ptr()->getheader().getstatementCount());

    tester_get_m_runtime_ptr()->nextStatementIndex = 100;

    // check reg out of bounds
    directive.setconditionalReg(Fpy::NUM_REGISTERS);
    result = tester_if_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    // should not have changed stmtidx
    ASSERT_NE(tester_get_m_runtime_ptr()->nextStatementIndex, 111);

    directive.setconditionalReg(0);
    directive.setfalseGotoStmtIndex(tester_get_m_sequenceObj_ptr()->getheader().getstatementCount() + 1);
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

TEST_F(FpySequencerTester, getTlm) {
    FpySequencer_GetTlmDirective directive(0, 1, 456);
    nextTlmId = 456;
    nextTlmValue.setBuffLen(1);
    nextTlmValue.getBuffAddr()[0] = 200;
    nextTlmTime.set(888, 777);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_getTlm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_from_getTlmChan(0, 456, Fw::Time(), Fw::TlmBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->serRegs[0].value[0], nextTlmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->serRegs[0].valueSize, nextTlmValue.getBuffLength());
    Fw::ExternalSerializeBuffer timeBuf(tester_get_m_runtime_ptr()->serRegs[1].value,
                                        tester_get_m_runtime_ptr()->serRegs[1].valueSize);
    timeBuf.setBuffLen(tester_get_m_runtime_ptr()->serRegs[1].valueSize);
    Fw::Time resultTime;
    ASSERT_EQ(timeBuf.deserialize(resultTime), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(resultTime, nextTlmTime);
    clearHistory();

    // try getting a nonexistent chan
    directive.setchanId(111);
    result = tester_getTlm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::TLM_CHAN_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.setchanId(456);

    // try setting bad value serReg
    directive.setvalueDestSerReg(Fpy::NUM_SERIALIZABLE_REGISTERS);
    result = tester_getTlm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    directive.setvalueDestSerReg(0);

    // try setting bad time serReg
    directive.settimeDestSerReg(Fpy::NUM_SERIALIZABLE_REGISTERS);
    result = tester_getTlm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    directive.settimeDestSerReg(0);
}

TEST_F(FpySequencerTester, getPrm) {
    FpySequencer_GetPrmDirective directive(0, 456);
    nextPrmId = 456;
    nextPrmValue.setBuffLen(1);
    nextPrmValue.getBuffAddr()[0] = 200;
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_getPrm_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_from_getParam(0, 456, Fw::ParamBuffer());
    ASSERT_EQ(tester_get_m_runtime_ptr()->serRegs[0].value[0], nextPrmValue.getBuffAddr()[0]);
    ASSERT_EQ(tester_get_m_runtime_ptr()->serRegs[0].valueSize, nextPrmValue.getBuffLength());
    clearHistory();

    // try getting a nonexistent param
    directive.setprmId(111);
    result = tester_getPrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::PRM_NOT_FOUND);
    err = DirectiveError::NO_ERROR;
    directive.setprmId(456);

    // try setting bad serReg
    directive.setdestSerRegIndex(Fpy::NUM_SERIALIZABLE_REGISTERS);
    result = tester_getPrm_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_OUT_OF_BOUNDS);
    err = DirectiveError::NO_ERROR;
    directive.setdestSerRegIndex(0);
}

TEST_F(FpySequencerTester, cmd) {
    U8 data[4] = {0x12, 0x23, 0x34, 0x45};
    FpySequencer_CmdDirective directive(123, 0, sizeof(data));
    memcpy(directive.getargBuf(), data, sizeof(data));
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_cmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);

    Fw::ComBuffer expected;
    ASSERT_EQ(expected.serialize(Fw::ComPacketType::FW_PACKET_COMMAND), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(directive.getopCode()), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serialize(data, sizeof(data), Fw::Serialization::OMIT_LENGTH), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_from_cmdOut_SIZE(1);
    ASSERT_from_cmdOut(0, expected, 0);
    this->clearHistory();

    // try dispatching again, make sure cmd uid is right
    tester_set_m_statementsDispatched(123);
    result = tester_cmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);
    ASSERT_from_cmdOut(0, expected, tester_get_m_statementsDispatched());
    this->clearHistory();

    // modify sequences started, make sure correct
    tester_set_m_sequencesStarted(456);
    result = tester_cmd_directiveHandler(directive, err);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(result, Signal::stmtResponse_keepWaiting);
    ASSERT_from_cmdOut(
        0, expected,
        (((tester_get_m_sequencesStarted() & 0xFFFF) << 16) | (tester_get_m_statementsDispatched() & 0xFFFF)));
}

TEST_F(FpySequencerTester, deserSerReg) {
    // Nominal case: Deserialize 1-byte value
    FpySequencer_DeserSerRegDirective directive(0, 0, 0, 1);
    tester_get_m_runtime_ptr()->serRegs[0].value[0] = 0x12;
    tester_get_m_runtime_ptr()->serRegs[0].valueSize = 1;
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[0], 0x12);

    // Deserialize 2-byte value
    directive = FpySequencer_DeserSerRegDirective(0, 0, 0, 2);
    tester_get_m_runtime_ptr()->serRegs[0].value[0] = 0x34;
    tester_get_m_runtime_ptr()->serRegs[0].value[1] = 0x56;
    tester_get_m_runtime_ptr()->serRegs[0].valueSize = 2;
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[0], 0x3456);

    // Deserialize 4-byte value
    directive = FpySequencer_DeserSerRegDirective(0, 0, 0, 4);
    tester_get_m_runtime_ptr()->serRegs[0].value[0] = 0x78;
    tester_get_m_runtime_ptr()->serRegs[0].value[1] = 0x9A;
    tester_get_m_runtime_ptr()->serRegs[0].value[2] = 0xBC;
    tester_get_m_runtime_ptr()->serRegs[0].value[3] = 0xDE;
    tester_get_m_runtime_ptr()->serRegs[0].valueSize = 4;
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[0], 0x789ABCDE);

    // Deserialize 8-byte value
    directive = FpySequencer_DeserSerRegDirective(0, 0, 0, 8);
    memcpy(tester_get_m_runtime_ptr()->serRegs[0].value, "\x01\x23\x45\x67\x89\xAB\xCD\xEF", 8);
    tester_get_m_runtime_ptr()->serRegs[0].valueSize = 8;
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[0], 0x0123456789ABCDEF);

    // Error case: Out-of-bounds serializable register index
    directive = FpySequencer_DeserSerRegDirective(Fpy::NUM_SERIALIZABLE_REGISTERS, 0, 0, 1);
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_OUT_OF_BOUNDS);

    // Error case: Out-of-bounds register index
    directive = FpySequencer_DeserSerRegDirective(0, 0, Fpy::NUM_REGISTERS, 1);
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);

    // Error case: Offset + size exceeds serializable register size
    directive = FpySequencer_DeserSerRegDirective(0, 1, 0, 1);
    tester_get_m_runtime_ptr()->serRegs[0].valueSize = 1;
    result = tester_deserSerReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::SER_REG_ACCESS_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, binaryCmp) {
    // Test EQ (equal)
    FpySequencer_BinaryCmpDirective directiveEQ(0, 1, 2, Fpy::DirectiveId::IEQ);
    tester_get_m_runtime_ptr()->regs[0] = 10;
    tester_get_m_runtime_ptr()->regs[1] = 10;
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_binaryCmp_directiveHandler(directiveEQ, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test NE (not equal)
    FpySequencer_BinaryCmpDirective directiveNE(0, 1, 2, Fpy::DirectiveId::INE);
    tester_get_m_runtime_ptr()->regs[1] = 20;
    result = tester_binaryCmp_directiveHandler(directiveNE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test OR (bitwise OR)
    FpySequencer_BinaryCmpDirective directiveOR(0, 1, 2, Fpy::DirectiveId::OR);
    tester_get_m_runtime_ptr()->regs[0] = 10;  // 0b1010;
    tester_get_m_runtime_ptr()->regs[1] = 5;   // 0b0101;
    result = tester_binaryCmp_directiveHandler(directiveOR, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 15);  // 0b1111

    // Test AND (bitwise AND)
    FpySequencer_BinaryCmpDirective directiveAND(0, 1, 2, Fpy::DirectiveId::AND);
    tester_get_m_runtime_ptr()->regs[0] = 10;  // 0b1010;
    tester_get_m_runtime_ptr()->regs[1] = 12;  // 0b1100;
    result = tester_binaryCmp_directiveHandler(directiveAND, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 8);  // 0b1000

    // Test signed comparison (SLT - signed less than)
    FpySequencer_BinaryCmpDirective directiveSLT(0, 1, 2, Fpy::DirectiveId::SLT);
    tester_get_m_runtime_ptr()->regs[0] = -5;
    tester_get_m_runtime_ptr()->regs[1] = 10;
    result = tester_binaryCmp_directiveHandler(directiveSLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test unsigned comparison (ULT - unsigned less than)
    FpySequencer_BinaryCmpDirective directiveULT(0, 1, 2, Fpy::DirectiveId::ULT);
    tester_get_m_runtime_ptr()->regs[0] = 5;
    tester_get_m_runtime_ptr()->regs[1] = 10;
    result = tester_binaryCmp_directiveHandler(directiveULT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test unsigned comparison (UGT - unsigned greater than)
    FpySequencer_BinaryCmpDirective directiveUGT(0, 1, 2, Fpy::DirectiveId::UGT);
    tester_get_m_runtime_ptr()->regs[0] = 10;
    tester_get_m_runtime_ptr()->regs[1] = 5;
    result = tester_binaryCmp_directiveHandler(directiveUGT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test floating-point comparison (FLT - floating-point less than)
    FpySequencer_BinaryCmpDirective directiveFLT(0, 1, 2, Fpy::DirectiveId::FLT);
    double lhsValueFLT = 5.5;
    double rhsValueFLT = 10.1;
    tester_get_m_runtime_ptr()->regs[0] = *reinterpret_cast<I64*>(&lhsValueFLT);
    tester_get_m_runtime_ptr()->regs[1] = *reinterpret_cast<I64*>(&rhsValueFLT);
    result = tester_binaryCmp_directiveHandler(directiveFLT, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test floating-point comparison (FGE - floating-point greater or equal)
    FpySequencer_BinaryCmpDirective directiveFGE(0, 1, 2, Fpy::DirectiveId::FGE);
    double lhsValueFGE = 10.1;
    double rhsValueFGE = 10.1;
    tester_get_m_runtime_ptr()->regs[0] = *reinterpret_cast<I64*>(&lhsValueFGE);
    tester_get_m_runtime_ptr()->regs[1] = *reinterpret_cast<I64*>(&rhsValueFGE);
    result = tester_binaryCmp_directiveHandler(directiveFGE, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[2], 1);

    // Test out-of-bounds register index
    FpySequencer_BinaryCmpDirective directiveOOB(Fpy::NUM_REGISTERS, 1, 2, Fpy::DirectiveId::IEQ);
    result = tester_binaryCmp_directiveHandler(directiveOOB, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);

    // Test invalid operation
    FpySequencer_BinaryCmpDirective directiveInvalid(0, 1, 2, Fpy::DirectiveId::NO_OP);
    ASSERT_DEATH_IF_SUPPORTED(tester_binaryCmp_directiveHandler(directiveInvalid, err), "Assert: ");
}

TEST_F(FpySequencerTester, ieq) {
    I64 lhs = -1;
    I64 rhs = -1;
    ASSERT_EQ(tester_binaryCmp_ieq(lhs, rhs), true);
    rhs = 1;
    ASSERT_EQ(tester_binaryCmp_ieq(lhs, rhs), false);
}

TEST_F(FpySequencerTester, ine) {
    I64 lhs = -1;
    I64 rhs = -1;
    ASSERT_EQ(tester_binaryCmp_ine(lhs, rhs), false);
    rhs = 1;
    ASSERT_EQ(tester_binaryCmp_ine(lhs, rhs), true);
}

TEST_F(FpySequencerTester, or) {
    I64 lhs = static_cast<I64>(true);
    I64 rhs = static_cast<I64>(true);
    ASSERT_EQ(tester_binaryCmp_or(lhs, rhs), true);
    rhs = static_cast<I64>(false);
    ASSERT_EQ(tester_binaryCmp_or(lhs, rhs), true);
    lhs = static_cast<I64>(false);
    ASSERT_EQ(tester_binaryCmp_or(lhs, rhs), false);
}

TEST_F(FpySequencerTester, and) {
    I64 lhs = static_cast<I64>(false);
    I64 rhs = static_cast<I64>(false);
    ASSERT_EQ(tester_binaryCmp_and(lhs, rhs), false);
    rhs = static_cast<I64>(true);
    ASSERT_EQ(tester_binaryCmp_and(lhs, rhs), false);
    lhs = static_cast<I64>(true);
    ASSERT_EQ(tester_binaryCmp_and(lhs, rhs), true);
}

TEST_F(FpySequencerTester, ult) {
    U64 lhs = 0;
    U64 rhs = std::numeric_limits<U64>::max();
    ASSERT_EQ(tester_binaryCmp_ult(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_ult(static_cast<I64>(lhs), static_cast<I64>(rhs)), false);
    rhs = 1;
    ASSERT_EQ(tester_binaryCmp_ult(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
}

TEST_F(FpySequencerTester, ule) {
    U64 lhs = 0;
    U64 rhs = std::numeric_limits<U64>::max();
    ASSERT_EQ(tester_binaryCmp_ule(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_ule(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    rhs = 1;
    ASSERT_EQ(tester_binaryCmp_ule(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    lhs = 2;
    ASSERT_EQ(tester_binaryCmp_ule(static_cast<I64>(lhs), static_cast<I64>(rhs)), false);
}

TEST_F(FpySequencerTester, ugt) {
    U64 rhs = 0;
    U64 lhs = std::numeric_limits<U64>::max();
    ASSERT_EQ(tester_binaryCmp_ugt(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    lhs = 0;
    ASSERT_EQ(tester_binaryCmp_ugt(static_cast<I64>(lhs), static_cast<I64>(rhs)), false);
    lhs = 1;
    ASSERT_EQ(tester_binaryCmp_ugt(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
}

TEST_F(FpySequencerTester, uge) {
    U64 rhs = 0;
    U64 lhs = std::numeric_limits<U64>::max();
    ASSERT_EQ(tester_binaryCmp_uge(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    lhs = 0;
    ASSERT_EQ(tester_binaryCmp_uge(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    lhs = 1;
    ASSERT_EQ(tester_binaryCmp_uge(static_cast<I64>(lhs), static_cast<I64>(rhs)), true);
    rhs = 2;
    ASSERT_EQ(tester_binaryCmp_uge(static_cast<I64>(lhs), static_cast<I64>(rhs)), false);
}

TEST_F(FpySequencerTester, slt) {
    I64 lhs = 0;
    I64 rhs = std::numeric_limits<I64>::max();
    ASSERT_EQ(tester_binaryCmp_slt(lhs, rhs), true);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_slt(lhs, rhs), false);
    rhs = 1;
    ASSERT_EQ(tester_binaryCmp_slt(lhs, rhs), true);
}

TEST_F(FpySequencerTester, sle) {
    I64 lhs = 0;
    I64 rhs = std::numeric_limits<I64>::max();
    ASSERT_EQ(tester_binaryCmp_sle(lhs, rhs), true);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_sle(lhs, rhs), true);
    rhs = -1;
    ASSERT_EQ(tester_binaryCmp_sle(lhs, rhs), false);
}

TEST_F(FpySequencerTester, sgt) {
    I64 lhs = 0;
    I64 rhs = std::numeric_limits<I64>::max();
    ASSERT_EQ(tester_binaryCmp_sgt(lhs, rhs), false);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_sgt(lhs, rhs), false);
    rhs = -1;
    ASSERT_EQ(tester_binaryCmp_sgt(lhs, rhs), true);
}

TEST_F(FpySequencerTester, sge) {
    I64 lhs = 0;
    I64 rhs = std::numeric_limits<I64>::max();
    ASSERT_EQ(tester_binaryCmp_sge(lhs, rhs), false);
    rhs = 0;
    ASSERT_EQ(tester_binaryCmp_sge(lhs, rhs), true);
    rhs = -1;
    ASSERT_EQ(tester_binaryCmp_sge(lhs, rhs), true);
}

TEST_F(FpySequencerTester, setReg) {
    FpySequencer_SetRegDirective directive(0, -123);
    DirectiveError err = DirectiveError::NO_ERROR;
    Signal result = tester_setReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[0], -123);

    directive.setdest(Fpy::NUM_REGISTERS);
    result = tester_setReg_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, not) {
    FpySequencer_NotDirective directive(0, 1);
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_runtime_ptr()->regs[0] = 0x123;
    Signal result = tester_not_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_runtime_ptr()->regs[1], ~0x123);

    directive.setsrc(Fpy::NUM_REGISTERS);
    result = tester_not_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);

    directive.setsrc(0);
    directive.setres(Fpy::NUM_REGISTERS);
    result = tester_not_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::REGISTER_OUT_OF_BOUNDS);
}

TEST_F(FpySequencerTester, exit) {
    FpySequencer_ExitDirective directive(true);
    DirectiveError err = DirectiveError::NO_ERROR;
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(123);
    Signal result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_success);
    ASSERT_EQ(err, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->getheader().getstatementCount(), 123);

    directive.setsuccess(false);
    result = tester_exit_directiveHandler(directive, err);
    ASSERT_EQ(result, Signal::stmtResponse_failure);
    ASSERT_EQ(err, DirectiveError::DELIBERATE_FAILURE);
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
    ASSERT_EQ(result, Signal::result_timeOpFailed);
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
    ASSERT_EQ(result, Signal::result_timeOpFailed);
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
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    header.setbodySize(50);
    header.setschemaVersion(Fpy::SCHEMA_VERSION);
    header.setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);

    ASSERT_EQ(tester_readHeader(), Fw::Success::SUCCESS);
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->getheader(), header);

    // check not enough bytes
    tester_get_m_sequenceBuffer_ptr()->resetDeser();
    tester_get_m_sequenceBuffer_ptr()->setBuffLen(tester_get_m_sequenceBuffer_ptr()->getBuffLength() - 1);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_FileReadDeserializeError_SIZE(1);

    // check wrong schema version
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.setschemaVersion(Fpy::SCHEMA_VERSION + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    header.setschemaVersion(Fpy::SCHEMA_VERSION);
    clearHistory();

    // check too many args
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT + 1);
    ASSERT_EQ(tester_get_m_sequenceBuffer_ptr()->serialize(header), Fw::SerializeStatus::FW_SERIALIZE_OK);
    ASSERT_EQ(tester_readHeader(), Fw::Success::FAILURE);
    ASSERT_EVENTS_TooManySequenceArgs_SIZE(1);
    header.setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);

    // check too many stmts
    tester_get_m_sequenceBuffer_ptr()->resetSer();
    header.setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT + 1);
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
    tester_get_m_sequenceObj_ptr()->getheader().setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);

    ASSERT_EQ(tester_readBody(), Fw::Success::SUCCESS);

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_ARG_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceObj_ptr()->getargs()[ii], 123);
    }

    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        ASSERT_EQ(tester_get_m_sequenceObj_ptr()->getstatements()[ii], stmt);
    }

    tester_get_m_sequenceBuffer_ptr()->resetSer();
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(0);
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
    tester_get_m_sequenceObj_ptr()->getheader().setargumentCount(Fpy::MAX_SEQUENCE_ARG_COUNT);
    tester_get_m_sequenceObj_ptr()->getheader().setstatementCount(Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
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
    ASSERT_EQ(tester_get_m_sequenceObj_ptr()->getfooter(), footer);

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
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE, true), Fw::Success::SUCCESS);
    seqFile.close();

    // check capacity too low
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data, Fpy::Header::SERIALIZED_SIZE - 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE, true), Fw::Success::FAILURE);
    seqFile.close();

    // check not enough bytes
    tester_get_m_sequenceBuffer_ptr()->setExtBuffer(data,
                                                    Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1);
    ASSERT_EQ(seqFile.open("test.bin", Os::File::OPEN_READ), Os::File::OP_OK);
    ASSERT_EQ(tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE + 1, true),
              Fw::Success::FAILURE);

    seqFile.close();
    removeFile("test.bin");

    // read after close
    ASSERT_DEATH_IF_SUPPORTED(
        tester_readBytes(seqFile, Fpy::Header::SERIALIZED_SIZE + Fpy::Footer::SERIALIZED_SIZE, true), "Assert: ");
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
    seq.getheader().setschemaVersion(Fpy::SCHEMA_VERSION + 1);
    writeToFile("test.bin");
    ASSERT_EQ(tester_validate(), Fw::Success::FAILURE);
    ASSERT_EVENTS_WrongSchemaVersion_SIZE(1);
    seq.getheader().setschemaVersion(Fpy::SCHEMA_VERSION);
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
    // cmp.m_sequenceObj.getstatements()[0].setopCode(reinterpret_cast<Svc::Fpy::DirectiveId::T>(200));
    // result = cmp.dispatchStatement();
    // ASSERT_EQ(result, Signal::result_dispatchStatement_failure);

    clearSeq();
    time = Fw::Time(456, 123);
    setTestTime(time);
    // okay try adding a command
    add_CMD(123);
    *(tester_get_m_sequenceObj_ptr()) = seq;
    tester_get_m_runtime_ptr()->nextStatementIndex = 0;
    result = tester_dispatchStatement();
    ASSERT_EQ(result, Signal::result_dispatchStatement_success);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementOpcode, Fpy::DirectiveId::CMD);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentCmdOpcode, 123);
    ASSERT_EQ(tester_get_m_runtime_ptr()->currentStatementDispatchTime, time);

    tester_get_m_runtime_ptr()->nextStatementIndex =
        tester_get_m_sequenceObj_ptr()->getheader().getstatementCount() + 1;
    ASSERT_DEATH_IF_SUPPORTED(tester_dispatchStatement(), "Assert: ");
}

TEST_F(FpySequencerTester, deserialize_waitRel) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitRelDirective waitRel(123, 123);
    add_WAIT_REL(waitRel);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitRel, waitRel);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}
TEST_F(FpySequencerTester, deserialize_waitAbs) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_WaitAbsDirective waitAbs(Fw::Time(123, 123));
    add_WAIT_ABS(waitAbs);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.waitAbs, waitAbs);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_setSerReg) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_SetSerRegDirective setSerReg(0, 123, 10);
    add_SET_SER_REG(setSerReg);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    // for some reason, can't just use the equality method here... i get a huge asan err
    ASSERT_EQ(actual.setSerReg.get_valueSize(), setSerReg.get_valueSize());
    ASSERT_EQ(actual.setSerReg.getindex(), setSerReg.getindex());
    ASSERT_EQ(memcmp(actual.setSerReg.getvalue(), setSerReg.getvalue(), setSerReg.get_valueSize()), 0);
    // write some junk after buf, setserReg should eat it up and succeed
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    this->clearHistory();
    // clear buf, should fail cuz no valueSize or whatever
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);

    this->clearHistory();
    clearSeq();
    // run with valueSize too big
    if (Fpy::MAX_SERIALIZABLE_REGISTER_SIZE + 6 <= FW_STATEMENT_ARG_BUFFER_MAX_SIZE) {
        // we can test this
        setSerReg = FpySequencer_SetSerRegDirective(0, 123, Fpy::MAX_SERIALIZABLE_REGISTER_SIZE + 1);
        add_SET_SER_REG(setSerReg);
        result = tester_deserializeDirective(seq.getstatements()[0], actual);
        ASSERT_EQ(result, Fw::Success::FAILURE);
        ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    }
}

TEST_F(FpySequencerTester, deserialize_goto) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GotoDirective gotoDir(123);
    add_GOTO(gotoDir);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.gotoDirective, gotoDir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_if) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_IfDirective ifDir(123, 9999);
    add_IF(ifDir);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.ifDirective, ifDir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_noOp) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_NoOpDirective noOp;
    add_NO_OP();
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.noOp, noOp);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, should succeed
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
}

TEST_F(FpySequencerTester, deserialize_getTlm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetTlmDirective dir(123, 234, 456);
    add_GET_TLM(dir);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getTlm, dir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_getPrm) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_GetPrmDirective dir(123, 456);
    add_GET_PRM(dir);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.getPrm, dir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

TEST_F(FpySequencerTester, deserialize_exit) {
    FpySequencer::DirectiveUnion actual;
    FpySequencer_ExitDirective dir(false);
    add_EXIT(dir);
    Fw::Success result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::SUCCESS);
    ASSERT_EQ(actual.exit, dir);
    // write some junk after buf, make sure it fails
    seq.getstatements()[0].getargBuf().serialize(123);
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
    this->clearHistory();
    // clear args, make sure it fails
    seq.getstatements()[0].getargBuf().resetSer();
    result = tester_deserializeDirective(seq.getstatements()[0], actual);
    ASSERT_EQ(result, Fw::Success::FAILURE);
    ASSERT_EVENTS_DirectiveDeserializeError_SIZE(1);
}

// caught a bug
TEST_F(FpySequencerTester, checkTimers) {
    allocMem();
    add_WAIT_REL(FpySequencer_WaitRelDirective(10, 0));
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
    add_CMD(123);
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
    add_WAIT_REL(FpySequencer_WaitRelDirective(10, 0));
    add_CMD(123);
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
    add_CMD(123);
    writeToFile("test.bin");
    tester_set_m_sequencesStarted(255);
    tester_set_m_statementsDispatched(255);
    sendCmd_RUN(0, 0, Fw::String("test.bin"), FpySequencer_BlockState::BLOCK);
    dispatchUntilState(State::RUNNING_AWAITING_STATEMENT_RESPONSE);
    invoke_to_cmdResponseIn(0, 456, 0x01000100, Fw::CmdResponse::OK);
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::EXECUTION_ERROR);
    clearHistory();
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
