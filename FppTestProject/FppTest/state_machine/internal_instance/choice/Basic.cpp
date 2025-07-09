// ======================================================================
// \title  Basic.cpp
// \author bocchino
// \brief  cpp file for Basic component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/choice/Basic.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceChoice {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Basic::Basic(const char* const compName)
    : BasicComponentBase(compName),
      m_basic_action_a_history(),
      m_basic_action_b_history(),
      m_basic_guard_g(),
      m_smChoiceBasic_action_a_history(),
      m_smChoiceBasic_action_b_history(),
      m_smChoiceBasic_guard_g() {}

Basic::~Basic() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Basic::schedIn_handler(FwIndexType portNum, U32 context) {
    // Nothing to do
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void Basic::FppTest_SmInstanceChoice_Basic_Basic_action_a(SmId smId,
                                                          FppTest_SmInstanceChoice_Basic_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::basic);
    this->m_basic_action_a_history.push(signal);
}

void Basic::FppTest_SmInstanceChoice_Basic_Basic_action_b(SmId smId,
                                                          FppTest_SmInstanceChoice_Basic_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::basic);
    this->m_basic_action_b_history.push(signal);
}

void Basic::FppTest_SmChoice_Basic_action_a(SmId smId, FppTest_SmChoice_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceBasic);
    this->m_smChoiceBasic_action_a_history.push(signal);
}

void Basic::FppTest_SmChoice_Basic_action_b(SmId smId, FppTest_SmChoice_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::smChoiceBasic);
    this->m_smChoiceBasic_action_b_history.push(signal);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool Basic::FppTest_SmInstanceChoice_Basic_Basic_guard_g(SmId smId,
                                                         FppTest_SmInstanceChoice_Basic_Basic::Signal signal) const {
    FW_ASSERT(smId == SmId::basic, static_cast<FwAssertArgType>(smId));
    return this->m_basic_guard_g.call(signal);
}

bool Basic::FppTest_SmChoice_Basic_guard_g(SmId smId, FppTest_SmChoice_Basic::Signal signal) const {
    FW_ASSERT(smId == SmId::smChoiceBasic, static_cast<FwAssertArgType>(smId));
    return this->m_smChoiceBasic_guard_g.call(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Basic::testBasicTrue() {
    this->m_basic_action_a_history.clear();
    this->m_basic_action_b_history.clear();
    this->m_basic_guard_g.reset();
    this->m_basic_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->basic_getState(), Basic_Basic::State::S1);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_basic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_basic_action_b_history.getSize(), 0);
    this->basic_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getItemAt(0), Basic_Basic::Signal::s);
    ASSERT_EQ(this->m_basic_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_basic_action_a_history.getItemAt(0), Basic_Basic::Signal::s);
    ASSERT_EQ(this->m_basic_action_b_history.getSize(), 0);
    ASSERT_EQ(this->basic_getState(), Basic_Basic::State::S2);
}

void Basic::testBasicFalse() {
    this->m_basic_action_a_history.clear();
    this->m_basic_action_b_history.clear();
    this->m_basic_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->basic_getState(), Basic_Basic::State::S1);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_basic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_basic_action_b_history.getSize(), 0);
    this->basic_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_basic_guard_g.getCallHistory().getItemAt(0), Basic_Basic::Signal::s);
    ASSERT_EQ(this->m_basic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_basic_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_basic_action_b_history.getItemAt(0), Basic_Basic::Signal::s);
    ASSERT_EQ(this->basic_getState(), Basic_Basic::State::S3);
}

void Basic::testSmChoiceBasicTrue() {
    this->m_smChoiceBasic_action_a_history.clear();
    this->m_smChoiceBasic_action_b_history.clear();
    this->m_smChoiceBasic_guard_g.reset();
    this->m_smChoiceBasic_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceBasic_getState(), SmChoice_Basic::State::S1);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceBasic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceBasic_action_b_history.getSize(), 0);
    this->smChoiceBasic_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getItemAt(0), SmChoice_Basic::Signal::s);
    ASSERT_EQ(this->m_smChoiceBasic_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceBasic_action_a_history.getItemAt(0), SmChoice_Basic::Signal::s);
    ASSERT_EQ(this->m_smChoiceBasic_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smChoiceBasic_getState(), SmChoice_Basic::State::S2);
}

void Basic::testSmChoiceBasicFalse() {
    this->m_smChoiceBasic_action_a_history.clear();
    this->m_smChoiceBasic_action_b_history.clear();
    this->m_smChoiceBasic_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smChoiceBasic_getState(), SmChoice_Basic::State::S1);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smChoiceBasic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceBasic_action_b_history.getSize(), 0);
    this->smChoiceBasic_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smChoiceBasic_guard_g.getCallHistory().getItemAt(0), SmChoice_Basic::Signal::s);
    ASSERT_EQ(this->m_smChoiceBasic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smChoiceBasic_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smChoiceBasic_action_b_history.getItemAt(0), SmChoice_Basic::Signal::s);
    ASSERT_EQ(this->smChoiceBasic_getState(), SmChoice_Basic::State::S3);
}

}  // namespace SmInstanceChoice

}  // namespace FppTest
