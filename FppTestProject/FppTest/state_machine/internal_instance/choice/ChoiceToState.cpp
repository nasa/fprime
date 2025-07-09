// ======================================================================
// \title  ChoiceToState.cpp
// \author bocchino
// \brief  cpp file for ChoiceToState component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/ChoiceToState.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ChoiceToState::ChoiceToState(const char* const compName)
    : ChoiceToStateComponentBase(compName),
      m_smChoiceChoiceToState_actionHistory(),
      m_smChoiceChoiceToState_guard_g() {}

ChoiceToState::~ChoiceToState() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void ChoiceToState::FppTest_SmChoice_ChoiceToState_action_exitS1(SmId smId,
                                                                 FppTest_SmChoice_ChoiceToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToState);
    this->m_smChoiceChoiceToState_actionHistory.push(signal, ActionId::EXIT_S1);
}

void ChoiceToState::FppTest_SmChoice_ChoiceToState_action_a(SmId smId, FppTest_SmChoice_ChoiceToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToState);
    this->m_smChoiceChoiceToState_actionHistory.push(signal, ActionId::A);
}

void ChoiceToState::FppTest_SmChoice_ChoiceToState_action_enterS2(SmId smId,
                                                                  FppTest_SmChoice_ChoiceToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToState);
    this->m_smChoiceChoiceToState_actionHistory.push(signal, ActionId::ENTER_S2);
}

void ChoiceToState::FppTest_SmChoice_ChoiceToState_action_enterS3(SmId smId,
                                                                  FppTest_SmChoice_ChoiceToState::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToState);
    this->m_smChoiceChoiceToState_actionHistory.push(signal, ActionId::ENTER_S3);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool ChoiceToState::FppTest_SmChoice_ChoiceToState_guard_g(SmId smId,
                                                           FppTest_SmChoice_ChoiceToState::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceChoiceToState, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceChoiceToState_guard_g.call(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ChoiceToState::testTrue() {
    this->m_smChoiceChoiceToState_actionHistory.clear();
    this->m_smChoiceChoiceToState_guard_g.reset();
    this->m_smChoiceChoiceToState_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceChoiceToState_getState(), SmChoice_ChoiceToState::State::S1);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToState_actionHistory.getSize(), 0);
    this->smChoiceChoiceToState_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getItemAt(0), SmChoice_ChoiceToState::Signal::s);
    const FwIndexType expectedSize = 5;
    ASSERT_EQ(this->m_smChoiceChoiceToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smChoiceChoiceToState_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmChoice_ChoiceToState::Signal::s);
    }
    const auto& values = this->m_smChoiceChoiceToState_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(values.getItemAt(3), ActionId::A);
    ASSERT_EQ(values.getItemAt(4), ActionId::ENTER_S3);
    ASSERT_EQ(this->smChoiceChoiceToState_getState(), SmChoice_ChoiceToState::State::S2_S3);
}

void ChoiceToState::testFalse() {
    this->m_smChoiceChoiceToState_actionHistory.clear();
    this->m_smChoiceChoiceToState_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceChoiceToState_getState(), SmChoice_ChoiceToState::State::S1);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToState_actionHistory.getSize(), 0);
    this->smChoiceChoiceToState_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToState_guard_g.getCallHistory().getItemAt(0), SmChoice_ChoiceToState::Signal::s);
    const FwIndexType expectedSize = 4;
    ASSERT_EQ(this->m_smChoiceChoiceToState_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smChoiceChoiceToState_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmChoice_ChoiceToState::Signal::s);
    }
    const auto& values = this->m_smChoiceChoiceToState_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(values.getItemAt(3), ActionId::ENTER_S3);
    ASSERT_EQ(this->smChoiceChoiceToState_getState(), SmChoice_ChoiceToState::State::S2_S3);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
