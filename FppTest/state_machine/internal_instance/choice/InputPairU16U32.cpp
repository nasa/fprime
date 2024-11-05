// ======================================================================
// \title  InputPairU16U32.cpp
// \author bocchino
// \brief  cpp file for InputPairU16U32 component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/InputPairU16U32.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

InputPairU16U32 ::InputPairU16U32(const char* const compName)
    : InputPairU16U32ComponentBase(compName),
      m_smChoiceInputPairU16U32_action_a_history(),
      m_smChoiceInputPairU16U32_guard_g() {}

InputPairU16U32 ::~InputPairU16U32() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void InputPairU16U32 ::FppTest_SmChoice_InputPairU16U32_action_a(SmId smId,
                                                                 FppTest_SmChoice_InputPairU16U32::Signal signal,
                                                                 U32 value) {
    ASSERT_EQ(smId, SmId::smChoiceInputPairU16U32);
    this->m_smChoiceInputPairU16U32_action_a_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool InputPairU16U32 ::FppTest_SmChoice_InputPairU16U32_guard_g(SmId smId,
                                                                FppTest_SmChoice_InputPairU16U32::Signal signal,
                                                                U32 value) const {
    FW_ASSERT(smId == SmId::smChoiceInputPairU16U32, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceInputPairU16U32_guard_g.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void InputPairU16U32::testS1True() {
    this->m_smChoiceInputPairU16U32_action_a_history.clear();
    this->m_smChoiceInputPairU16U32_guard_g.reset();
    this->m_smChoiceInputPairU16U32_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 0);
    const U16 value = static_cast<U16>(STest::Pick::any());
    this->smChoiceInputPairU16U32_sendSignal_s1(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S2);
}

void InputPairU16U32::testS1False() {
    this->m_smChoiceInputPairU16U32_action_a_history.clear();
    this->m_smChoiceInputPairU16U32_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 0);
    const U16 value = static_cast<U16>(STest::Pick::any());
    this->smChoiceInputPairU16U32_sendSignal_s1(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S3);
}

void InputPairU16U32::testS2True() {
    this->m_smChoiceInputPairU16U32_action_a_history.clear();
    this->m_smChoiceInputPairU16U32_guard_g.reset();
    this->m_smChoiceInputPairU16U32_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smChoiceInputPairU16U32_sendSignal_s2(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s2);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s2);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S2);
}

void InputPairU16U32::testS2False() {
    this->m_smChoiceInputPairU16U32_action_a_history.clear();
    this->m_smChoiceInputPairU16U32_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smChoiceInputPairU16U32_sendSignal_s2(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s2);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getSignals().getItemAt(0),
              SmChoice_InputPairU16U32::Signal::s2);
    ASSERT_EQ(this->m_smChoiceInputPairU16U32_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->smChoiceInputPairU16U32_getState(), SmChoice_InputPairU16U32::State::S3);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
