// ======================================================================
// \title  StateToChildTester.cpp
// \author bocchino
// \brief  cpp file for StateToChildTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/StateToChildTester.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

StateToChildTester::StateToChildTester(const char* const compName)
    : StateToChildComponentBase(compName), m_smStateStateToChild_actionHistory() {}

StateToChildTester::~StateToChildTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void StateToChildTester::FppTest_SmState_StateToChild_action_exitS2(SmId smId,
                                                                    FppTest_SmState_StateToChild::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChild);
    this->m_smStateStateToChild_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToChildTester::FppTest_SmState_StateToChild_action_exitS3(SmId smId,
                                                                    FppTest_SmState_StateToChild::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChild);
    this->m_smStateStateToChild_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToChildTester::FppTest_SmState_StateToChild_action_a(SmId smId, FppTest_SmState_StateToChild::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChild);
    this->m_smStateStateToChild_actionHistory.push(signal, ActionId::A);
}

void StateToChildTester::FppTest_SmState_StateToChild_action_enterS2(SmId smId,
                                                                     FppTest_SmState_StateToChild::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChild);
    this->m_smStateStateToChild_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToChildTester::FppTest_SmState_StateToChild_action_enterS3(SmId smId,
                                                                     FppTest_SmState_StateToChild::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChild);
    this->m_smStateStateToChild_actionHistory.push(signal, ActionId::ENTER_S3);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void StateToChildTester::testInit() {
    this->m_smStateStateToChild_actionHistory.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateStateToChild_getState(), SmState_StateToChild::State::S1_S2);
    ASSERT_EQ(this->m_smStateStateToChild_actionHistory.getSize(), 1);
    const auto& signals = this->m_smStateStateToChild_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChild_actionHistory.getValues();
    ASSERT_EQ(signals.getItemAt(0), SmState_StateToChild::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S2);
}

void StateToChildTester::testS2_to_S2() {
    this->m_smStateStateToChild_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToChild_actionHistory.clear();
    this->smStateStateToChild_sendSignal_S1_to_S2();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToChild_getState(), SmState_StateToChild::State::S1_S2);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_smStateStateToChild_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChild_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChild_actionHistory.getValues();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChild::Signal::S1_to_S2);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::A);
    ASSERT_EQ(actions.getItemAt(2), ActionId::ENTER_S2);
}

void StateToChildTester::testS2_to_S3() {
    this->m_smStateStateToChild_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToChild_actionHistory.clear();
    this->smStateStateToChild_sendSignal_S2_to_S3();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToChild_getState(), SmState_StateToChild::State::S1_S3);
    const FwIndexType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToChild_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChild_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChild_actionHistory.getValues();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChild::Signal::S2_to_S3);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S3);
}

void StateToChildTester::testS3_to_S2() {
    this->m_smStateStateToChild_actionHistory.clear();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToChild_actionHistory.clear();
        this->smStateStateToChild_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStateStateToChild_getState(), SmState_StateToChild::State::S1_S3);
    }
    {
        this->m_smStateStateToChild_actionHistory.clear();
        this->smStateStateToChild_sendSignal_S1_to_S2();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStateStateToChild_getState(), SmState_StateToChild::State::S1_S2);
    }
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_smStateStateToChild_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChild_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChild_actionHistory.getValues();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChild::Signal::S1_to_S2);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::A);
    ASSERT_EQ(actions.getItemAt(2), ActionId::ENTER_S2);
}

}  // namespace SmInstanceState

}  // namespace FppTest
