#include "FpySequencerTester.hpp"

namespace Svc {

TEST_F(FpySequencerTester, Empty) {
    allocMem();
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, Full) {
    allocMem();
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        add_NO_OP();
    }
    writeAndRun();
    dispatchUntilState(State::IDLE, Fpy::MAX_SEQUENCE_STATEMENT_COUNT * 4);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencerTester::get_OPCODE_RUN(), 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, ComplexControlFlow) {
    allocMem();

    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(true), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_ALLOCATE(1);
    add_STORE_TLM_VAL(123, 0);
    add_LOAD(0, 1);
    add_IF(6);
    // if true
    add_NO_OP();
    add_GOTO(9);  // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 6);
    nextTlmValue.resetSer();
    nextTlmValue.serialize(false);
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 7);
}

TEST_F(FpySequencerTester, OrOfTlmAndReg) {
    allocMem();

    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(true), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_ALLOCATE(1);
    add_STORE_TLM_VAL(123, 0);
    add_LOAD(0, 1);
    add_PUSH_VAL<U8>(0);
    // or between the stored const and the tlm val
    add_STACK_OP(Fpy::DirectiveId::OR);
    add_IF(8);
    // if true
    add_NO_OP();
    add_GOTO(11);  // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 8);
    nextTlmValue.resetSer();
    nextTlmValue.serialize(false);
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 9);
}

TEST_F(FpySequencerTester, CmpIntTlm) {
    allocMem();

    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(static_cast<U64>(999)), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_ALLOCATE(8);
    add_STORE_TLM_VAL(123, 0);
    add_LOAD(0, 8);
    add_PUSH_VAL(static_cast<U64>(999));
    // unsigned >= between tlm and reg
    add_STACK_OP(Fpy::DirectiveId::UGE);
    add_IF(8);
    // if true
    add_NO_OP();
    add_GOTO(11);  // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    // should be equal on first try
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 8);
    nextTlmValue.resetSer();
    // should fail if tlm is 998
    nextTlmValue.serialize(static_cast<U64>(998));
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 9);
}
TEST_F(FpySequencerTester, NotTrueSeq) {
    // this sequence caught one bug
    allocMem();

    add_PUSH_VAL(static_cast<U8>(255));
    add_STACK_OP(Fpy::DirectiveId::NOT);
    add_IF(5);
    // should not get here
    add_PUSH_VAL<U8>(false);
    add_EXIT();
    add_PUSH_VAL<U8>(true);
    add_EXIT();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    // not of 255 should be interpreted as false
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
}

}  // namespace Svc