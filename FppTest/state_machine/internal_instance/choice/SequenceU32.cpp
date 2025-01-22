// ======================================================================
// \title  SequenceU32.cpp
// \author bocchino
// \brief  cpp file for SequenceU32 component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/SequenceU32.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SequenceU32::SequenceU32(const char* const compName)
    : SequenceU32ComponentBase(compName),
      m_smChoiceSequenceU32_action_a_history(),
      m_smChoiceSequenceU32_action_b_history(),
      m_smChoiceSequenceU32_guard_g1(),
      m_smChoiceSequenceU32_guard_g2() {}

SequenceU32::~SequenceU32() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void SequenceU32::FppTest_SmChoice_SequenceU32_action_a(SmId smId,
                                                        FppTest_SmChoice_SequenceU32::Signal signal,
                                                        U32 value) {
    ASSERT_EQ(smId, SmId::smChoiceSequenceU32);
    this->m_smChoiceSequenceU32_action_a_history.push(signal, value);
}

void SequenceU32::FppTest_SmChoice_SequenceU32_action_b(SmId smId, FppTest_SmChoice_SequenceU32::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceSequenceU32);
    this->m_smChoiceSequenceU32_action_b_history.push(signal);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool SequenceU32::FppTest_SmChoice_SequenceU32_guard_g1(SmId smId, FppTest_SmChoice_SequenceU32::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceSequenceU32, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceSequenceU32_guard_g1.call(signal);
}

bool SequenceU32::FppTest_SmChoice_SequenceU32_guard_g2(SmId smId,
                                                        FppTest_SmChoice_SequenceU32::Signal signal,
                                                        U32 value) const {
    FW_ASSERT(smId == SmId::smChoiceSequenceU32, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceSequenceU32_guard_g2.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SequenceU32::testG1True() {
    this->m_smChoiceSequenceU32_action_a_history.clear();
    this->m_smChoiceSequenceU32_action_b_history.clear();
    this->m_smChoiceSequenceU32_guard_g1.reset();
    this->m_smChoiceSequenceU32_guard_g1.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smChoiceSequenceU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S2);
}

void SequenceU32::testG1FalseG2True() {
    this->m_smChoiceSequenceU32_action_a_history.clear();
    this->m_smChoiceSequenceU32_action_b_history.clear();
    this->m_smChoiceSequenceU32_guard_g1.reset();
    this->m_smChoiceSequenceU32_guard_g2.reset();
    this->m_smChoiceSequenceU32_guard_g2.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smChoiceSequenceU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSignals().getItemAt(0),
              SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSignals().getItemAt(0), SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S3);
}

void SequenceU32::testG1FalseG2False() {
    this->m_smChoiceSequenceU32_action_a_history.clear();
    this->m_smChoiceSequenceU32_action_b_history.clear();
    this->m_smChoiceSequenceU32_guard_g1.reset();
    this->m_smChoiceSequenceU32_guard_g2.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smChoiceSequenceU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getSignals().getItemAt(0),
              SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequenceU32_guard_g2.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequenceU32_action_b_history.getItemAt(0), SmChoice_SequenceU32::Signal::s);
    ASSERT_EQ(this->smChoiceSequenceU32_getState(), SmChoice_SequenceU32::State::S4);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
