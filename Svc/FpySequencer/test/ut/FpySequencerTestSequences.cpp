#include "FpySequencerTester.hpp"

namespace Svc {

TEST_F(FpySequencerTester, Empty) {
    allocMem();
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, Full) {
    allocMem();
    for (U32 ii = 0; ii < Fpy::MAX_SEQUENCE_STATEMENT_COUNT; ii++) {
        add_NO_OP();
    }
    writeAndRun();
    dispatchUntilState(State::IDLE, Fpy::MAX_SEQUENCE_STATEMENT_COUNT * 4);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, FpySequencer::OPCODE_RUN, 0, Fw::CmdResponse::OK);
}

TEST_F(FpySequencerTester, ComplexControlFlow) {
    allocMem();
    
    nextTlmId = 123;
    ASSERT_EQ(nextTlmValue.serialize(true), Fw::SerializeStatus::FW_SERIALIZE_OK);
    add_GET_TLM(0, 1, 123);
    add_IF(0, 4);
    // if true
    add_NO_OP();
    add_GOTO(7); // goto end
    // else
    add_NO_OP();
    add_NO_OP();
    add_NO_OP();


    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(cmp.m_statementsDispatched, 4);
    nextTlmValue.resetSer();
    nextTlmValue.serialize(false);
    cmp.m_statementsDispatched = 0;
    writeAndRun();
    dispatchUntilState(State::IDLE);
    ASSERT_EQ(cmp.m_statementsDispatched, 5);
}

}