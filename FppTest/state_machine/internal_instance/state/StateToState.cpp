// ======================================================================
// \title  StateToState.cpp
// \author bocchino
// \brief  cpp file for StateToState component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/StateToState.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

StateToState::StateToState(const char* const compName)
    : StateToStateComponentBase(compName), m_smStateStateToState_actionHistory() {}

StateToState::~StateToState() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void StateToState::FppTest_SmState_StateToState_action_exitS1(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::EXIT_S1);
}

void StateToState::FppTest_SmState_StateToState_action_exitS2(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToState::FppTest_SmState_StateToState_action_exitS3(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToState::FppTest_SmState_StateToState_action_a(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::A);
}

void StateToState::FppTest_SmState_StateToState_action_enterS1(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::ENTER_S1);
}

void StateToState::FppTest_SmState_StateToState_action_enterS2(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToState::FppTest_SmState_StateToState_action_enterS3(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::ENTER_S3);
}

void StateToState::FppTest_SmState_StateToState_action_enterS4(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::ENTER_S4);
}

void StateToState::FppTest_SmState_StateToState_action_enterS5(SmId smId, FppTest_SmState_StateToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToState);
    this->m_smStateStateToState_actionHistory.push(signal, ActionId::ENTER_S5);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void StateToState::testInit() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S1_S2);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S2);
}

void StateToState::testS2_to_S3() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToState_actionHistory.clear();
    this->smStateStateToState_sendSignal_S2_to_S3();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S1_S3);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::S2_to_S3);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S3);
}

void StateToState::testS2_to_S4() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToState_actionHistory.clear();
    this->smStateStateToState_sendSignal_S1_to_S4();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS2_to_S5() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToState_actionHistory.clear();
    this->smStateStateToState_sendSignal_S1_to_S5();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::S1_to_S5);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS3_to_S4() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToState_actionHistory.clear();
        this->smStateStateToState_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    {
        this->m_smStateStateToState_actionHistory.clear();
        this->smStateStateToState_sendSignal_S1_to_S4();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS3_to_S5() {
    this->m_smStateStateToState_actionHistory.clear();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToState_actionHistory.clear();
        this->smStateStateToState_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    {
        this->m_smStateStateToState_actionHistory.clear();
        this->smStateStateToState_sendSignal_S1_to_S5();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->smStateStateToState_getState(), SmState_StateToState::State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToState_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToState_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToState::Signal::S1_to_S5);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

}  // namespace SmInstanceState

}  // namespace FppTest
