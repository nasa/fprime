// ======================================================================
// \title  ChoiceToChoice.cpp
// \author bocchino
// \brief  cpp file for ChoiceToChoice component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/ChoiceToChoice.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ChoiceToChoice ::ChoiceToChoice(const char* const compName)
    : ChoiceToChoiceComponentBase(compName),
      m_smChoiceChoiceToChoice_actionHistory(),
      m_smChoiceChoiceToChoice_guard_g1(),
      m_smChoiceChoiceToChoice_guard_g2() {}

ChoiceToChoice ::~ChoiceToChoice() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void ChoiceToChoice ::FppTest_SmChoice_ChoiceToChoice_action_exitS1(SmId smId,
                                                                    FppTest_SmChoice_ChoiceToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToChoice);
    this->m_smChoiceChoiceToChoice_actionHistory.push(signal, ActionId::EXIT_S1);
}

void ChoiceToChoice ::FppTest_SmChoice_ChoiceToChoice_action_a(SmId smId,
                                                               FppTest_SmChoice_ChoiceToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToChoice);
    this->m_smChoiceChoiceToChoice_actionHistory.push(signal, ActionId::A);
}

void ChoiceToChoice ::FppTest_SmChoice_ChoiceToChoice_action_enterS2(SmId smId,
                                                                     FppTest_SmChoice_ChoiceToChoice::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceChoiceToChoice);
    this->m_smChoiceChoiceToChoice_actionHistory.push(signal, ActionId::ENTER_S2);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool ChoiceToChoice ::FppTest_SmChoice_ChoiceToChoice_guard_g1(SmId smId,
                                                               FppTest_SmChoice_ChoiceToChoice::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceChoiceToChoice, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceChoiceToChoice_guard_g1.call(signal);
}

bool ChoiceToChoice ::FppTest_SmChoice_ChoiceToChoice_guard_g2(SmId smId,
                                                               FppTest_SmChoice_ChoiceToChoice::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceChoiceToChoice, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceChoiceToChoice_guard_g2.call(signal);
}

// ----------------------------------------------------------------------
// Overflow hook implementations for internal state machines
// ----------------------------------------------------------------------

void ChoiceToChoice ::smChoiceChoiceToChoice_stateMachineOverflowHook(SmId smId,
                                                                      FwEnumStoreType signal,
                                                                      Fw::SerializeBufferBase& buffer) {
    this->m_hookCalled = true;
    ASSERT_EQ(smId, SmId::smChoiceChoiceToChoice);
    ASSERT_EQ(static_cast<SmChoice_ChoiceToChoice::Signal>(signal), SmChoice_ChoiceToChoice::Signal::s);
    ASSERT_EQ(buffer.getBuffLeft(), 0);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ChoiceToChoice::testG1True() {
    this->m_smChoiceChoiceToChoice_actionHistory.clear();
    this->m_smChoiceChoiceToChoice_guard_g1.reset();
    this->m_smChoiceChoiceToChoice_guard_g2.reset();
    this->m_smChoiceChoiceToChoice_guard_g1.setReturnValue(true);
    this->m_hookCalled = false;
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), 0);
    this->smChoiceChoiceToChoice_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_FALSE(this->m_hookCalled);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getItemAt(0),
              SmChoice_ChoiceToChoice::Signal::s);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 0);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smChoiceChoiceToChoice_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmChoice_ChoiceToChoice::Signal::s);
    }
    const auto& values = this->m_smChoiceChoiceToChoice_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S2_S3);
}

void ChoiceToChoice::testG1FalseG2True() {
    this->m_smChoiceChoiceToChoice_actionHistory.clear();
    this->m_smChoiceChoiceToChoice_guard_g1.reset();
    this->m_smChoiceChoiceToChoice_guard_g2.reset();
    this->m_smChoiceChoiceToChoice_guard_g2.setReturnValue(true);
    this->m_hookCalled = false;
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), 0);
    this->smChoiceChoiceToChoice_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_FALSE(this->m_hookCalled);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getItemAt(0),
              SmChoice_ChoiceToChoice::Signal::s);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getItemAt(0),
              SmChoice_ChoiceToChoice::Signal::s);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smChoiceChoiceToChoice_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmChoice_ChoiceToChoice::Signal::s);
    }
    const auto& values = this->m_smChoiceChoiceToChoice_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S2_S3);
}

void ChoiceToChoice::testG1FalseG2False() {
    this->m_smChoiceChoiceToChoice_actionHistory.clear();
    this->m_smChoiceChoiceToChoice_guard_g1.reset();
    this->m_smChoiceChoiceToChoice_guard_g2.reset();
    this->m_hookCalled = false;
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), 0);
    this->smChoiceChoiceToChoice_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_FALSE(this->m_hookCalled);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getItemAt(0),
              SmChoice_ChoiceToChoice::Signal::s);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_guard_g1.getCallHistory().getItemAt(0),
              SmChoice_ChoiceToChoice::Signal::s);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_smChoiceChoiceToChoice_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_smChoiceChoiceToChoice_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), SmChoice_ChoiceToChoice::Signal::s);
    }
    const auto& values = this->m_smChoiceChoiceToChoice_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->smChoiceChoiceToChoice_getState(), SmChoice_ChoiceToChoice::State::S2_S4);
}

void ChoiceToChoice::testOverflow() {
    this->m_hookCalled = false;
    this->init(queueDepth, instanceId);
    for (FwSizeType i = 0; i < queueDepth; i++) {
        this->smChoiceChoiceToChoice_sendSignal_s();
        ASSERT_FALSE(this->m_hookCalled);
    }
    this->smChoiceChoiceToChoice_sendSignal_s();
    ASSERT_TRUE(this->m_hookCalled);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
