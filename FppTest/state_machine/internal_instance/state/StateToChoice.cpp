// ======================================================================
// \title  StateToChoice.cpp
// \author bocchino
// \brief  cpp file for StateToChoice component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/StateToChoice.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

StateToChoice::StateToChoice(const char* const compName)
    : StateToChoiceComponentBase(compName), m_smStateStateToChoice_actionHistory(), m_smStateStateToChoice_guard_g() {}

StateToChoice::~StateToChoice() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void StateToChoice::FppTest_SmState_StateToChoice_action_exitS1(SmId smId,
                                                                FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::EXIT_S1);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_exitS2(SmId smId,
                                                                FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_exitS3(SmId smId,
                                                                FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_a(SmId smId, FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::A);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_enterS1(SmId smId,
                                                                 FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::ENTER_S1);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_enterS2(SmId smId,
                                                                 FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_enterS3(SmId smId,
                                                                 FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::ENTER_S3);
}

void StateToChoice::FppTest_SmState_StateToChoice_action_enterS4(SmId smId,
                                                                 FppTest_SmState_StateToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateStateToChoice);
    this->m_smStateStateToChoice_actionHistory.push(signal, ActionId::ENTER_S4);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool StateToChoice ::FppTest_SmState_StateToChoice_guard_g(SmId smId,
                                                           FppTest_SmState_StateToChoice::Signal signal) const {
    FW_ASSERT(smId == SmId::smStateStateToChoice, static_cast<FwAssertArgType>(smId));
    return this->m_smStateStateToChoice_guard_g.call(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void StateToChoice::testInit() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S1_S2);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S2);
}

void StateToChoice::testS2_to_C() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->m_smStateStateToChoice_guard_g.reset();
    this->m_smStateStateToChoice_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    this->m_smStateStateToChoice_actionHistory.clear();
    this->smStateStateToChoice_sendSignal_S1_to_C();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S4_S5);
    const FwSizeType expectedSize = 4;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::S1_to_C);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
}

void StateToChoice::testS2_to_S3() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->init(queueDepth, instanceId);
    this->m_smStateStateToChoice_actionHistory.clear();
    this->smStateStateToChoice_sendSignal_S2_to_S3();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S1_S3);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::S2_to_S3);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S3);
}

void StateToChoice::testS2_to_S4() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->m_smStateStateToChoice_guard_g.reset();
    this->m_smStateStateToChoice_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    this->m_smStateStateToChoice_actionHistory.clear();
    this->smStateStateToChoice_sendSignal_S1_to_S4();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S4_S5);
    const FwSizeType expectedSize = 4;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
}

void StateToChoice::testS3_to_C() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->m_smStateStateToChoice_guard_g.reset();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToChoice_actionHistory.clear();
        this->smStateStateToChoice_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    {
        this->m_smStateStateToChoice_actionHistory.clear();
        this->smStateStateToChoice_sendSignal_S1_to_C();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S4_S6);
    const FwSizeType expectedSize = 4;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::S1_to_C);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
}

void StateToChoice::testS3_to_S4() {
    this->m_smStateStateToChoice_actionHistory.clear();
    this->m_smStateStateToChoice_guard_g.reset();
    this->init(queueDepth, instanceId);
    {
        this->m_smStateStateToChoice_actionHistory.clear();
        this->smStateStateToChoice_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    {
        this->m_smStateStateToChoice_actionHistory.clear();
        this->smStateStateToChoice_sendSignal_S1_to_S4();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->smStateStateToChoice_getState(), SmState_StateToChoice::State::S4_S6);
    const FwSizeType expectedSize = 4;
    ASSERT_EQ(this->m_smStateStateToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smStateStateToChoice_actionHistory.getSignals();
    const auto& actions = this->m_smStateStateToChoice_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmState_StateToChoice::Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
}

}  // namespace SmInstanceState

}  // namespace FppTest
