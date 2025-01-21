// ======================================================================
// \title  Sequence.cpp
// \author bocchino
// \brief  cpp file for Sequence component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/Sequence.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Sequence ::Sequence(const char* const compName)
    : SequenceComponentBase(compName),
      m_smChoiceSequence_action_a_history(),
      m_smChoiceSequence_action_b_history(),
      m_smChoiceSequence_guard_g1(),
      m_smChoiceSequence_guard_g2() {}

Sequence ::~Sequence() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void Sequence ::FppTest_SmChoice_Sequence_action_a(SmId smId, FppTest_SmChoice_Sequence::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceSequence);
    this->m_smChoiceSequence_action_a_history.push(signal);
}

void Sequence ::FppTest_SmChoice_Sequence_action_b(SmId smId, FppTest_SmChoice_Sequence::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceSequence);
    this->m_smChoiceSequence_action_b_history.push(signal);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool Sequence ::FppTest_SmChoice_Sequence_guard_g1(SmId smId, FppTest_SmChoice_Sequence::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceSequence, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceSequence_guard_g1.call(signal);
}

bool Sequence ::FppTest_SmChoice_Sequence_guard_g2(SmId smId, FppTest_SmChoice_Sequence::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceSequence, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceSequence_guard_g2.call(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Sequence::testG1True() {
    this->m_smChoiceSequence_action_a_history.clear();
    this->m_smChoiceSequence_action_b_history.clear();
    this->m_smChoiceSequence_guard_g1.reset();
    this->m_smChoiceSequence_guard_g1.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 0);
    this->smChoiceSequence_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S2);
}

void Sequence::testG1FalseG2True() {
    this->m_smChoiceSequence_action_a_history.clear();
    this->m_smChoiceSequence_action_b_history.clear();
    this->m_smChoiceSequence_guard_g1.reset();
    this->m_smChoiceSequence_guard_g2.reset();
    this->m_smChoiceSequence_guard_g2.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 0);
    this->smChoiceSequence_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S3);
}

void Sequence::testG1FalseG2False() {
    this->m_smChoiceSequence_action_a_history.clear();
    this->m_smChoiceSequence_action_b_history.clear();
    this->m_smChoiceSequence_guard_g1.reset();
    this->m_smChoiceSequence_guard_g2.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 0);
    this->smChoiceSequence_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g1.getCallHistory().getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_guard_g2.getCallHistory().getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->m_smChoiceSequence_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceSequence_action_b_history.getItemAt(0), SmChoice_Sequence::Signal::s);
    ASSERT_EQ(this->smChoiceSequence_getState(), SmChoice_Sequence::State::S4);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
