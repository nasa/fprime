// ======================================================================
// \title  StateToSelf.cpp
// \author bocchino
// \brief  cpp file for StateToSelf component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/StateToSelf.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

StateToSelf::StateToSelf(const char* const compName)
    : StateToSelfComponentBase(compName), m_smStateStateToSelf_actionHistory() {}

StateToSelf::~StateToSelf() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void StateToSelf::FppTest_SmState_StateToSelf_action_exitS1(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::EXIT_S1);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_exitS2(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_exitS3(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_a(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::A);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_enterS1(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::ENTER_S1);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_enterS2(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToSelf::FppTest_SmState_StateToSelf_action_enterS3(SmId smId, FppTest_SmState_StateToSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToSelf);
    this->m_smStateStateToSelf_actionHistory.push(signal, ActionId::ENTER_S3);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void StateToSelf::testInit() {
    this->m_smStateStateToSelf_actionHistory.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateStateToSelf_getState(), SmState_StateToSelf::State::S1_S2);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToSelf_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToSelf_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToSelf_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToSelf::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S2);
}

void StateToSelf::testS2_to_S1() {
    this->m_smStateStateToSelf_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToSelf_actionHistory.clear();
    this->smStateStateToSelf_sendSignal_S1_to_S1();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToSelf_getState(), SmState_StateToSelf::State::S1_S2);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToSelf_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToSelf_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToSelf_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToSelf::Signal::S1_to_S1);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S2);
}

void StateToSelf::testS2_to_S3() {
    this->m_smStateStateToSelf_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToSelf_actionHistory.clear();
    this->smStateStateToSelf_sendSignal_S2_to_S3();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToSelf_getState(), SmState_StateToSelf::State::S1_S3);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToSelf_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToSelf_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToSelf_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToSelf::Signal::S2_to_S3);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S3);
}

void StateToSelf::testS3_to_S1() {
    this->m_smStateStateToSelf_actionHistory.clear();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToSelf_actionHistory.clear();
        this->smStateStateToSelf_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    {
        this->m_smStateStateToSelf_actionHistory.clear();
        this->smStateStateToSelf_sendSignal_S1_to_S1();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->smStateStateToSelf_getState(), SmState_StateToSelf::State::S1_S2);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_smStateStateToSelf_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToSelf_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToSelf_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToSelf::Signal::S1_to_S1);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S2);
}

}  // namespace SmInstanceState

}  // namespace FppTest
