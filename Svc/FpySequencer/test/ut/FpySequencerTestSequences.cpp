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
    add_GET_TLM(0, 1, 123);
    add_DESER_SER_REG(0, 0, 0, 1);
    add_IF(0, 5);
    // if true
    add_NO_OP();
    add_GOTO(8); // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();


    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 5);
    nextTlmValue.resetSer();
    nextTlmValue.serialize(false);
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 6);
}

TEST_F(FpySequencerTester, OrOfTlmAndReg) {
    allocMem();

    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(true), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_GET_TLM(0, 1, 123);
    add_DESER_SER_REG(0, 0, 0, 1);
    add_SET_REG(1, 0);
    // or between the stored const and the tlm val
    add_BINARY_CMP(0, 1, 2, Fpy::DirectiveId::OR);
    add_IF(2, 7);
    // if true
    add_NO_OP();
    add_GOTO(10); // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 7);
    nextTlmValue.resetSer();
    nextTlmValue.serialize(false);
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 8);
}

TEST_F(FpySequencerTester, CmpIntTlm) {
    allocMem();

    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(999), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_GET_TLM(0, 1, 123);
    add_DESER_SER_REG(0, 0, 0, 4);
    add_SET_REG(1, 999);
    // unsigned >= between tlm and reg
    add_BINARY_CMP(0, 1, 2, Fpy::DirectiveId::UGE);
    add_IF(2, 7);
    // if true
    add_NO_OP();
    add_GOTO(10); // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();

    writeAndRun();
    dispatchUntilState(State::IDLE);
    // should be equal on first try
    ASSERT_EQ(tester_get_m_tlm_ptr()->lastDirectiveError, DirectiveError::NO_ERROR);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 7);
    nextTlmValue.resetSer();
    // should fail if tlm is 998
    nextTlmValue.serialize(998);
    tester_set_m_statementsDispatched(0);
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(tester_get_m_statementsDispatched(), 8);
}

}