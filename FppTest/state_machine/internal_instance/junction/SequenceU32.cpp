// ======================================================================
// \title  SequenceU32.cpp
// \author bocchino
// \brief  cpp file for SequenceU32 component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/junction/SequenceU32.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceJunction {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SequenceU32::SequenceU32(const char* const compName)
    : SequenceU32ComponentBase(compName),
      m_smJunctionSequenceU32_action_a_history(),
      m_smJunctionSequenceU32_action_b_history(),
      m_smJunctionSequenceU32_guard_g1(),
      m_smJunctionSequenceU32_guard_g2() {}

SequenceU32::~SequenceU32() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void SequenceU32::FppTest_SmJunction_SequenceU32_action_a(SmId smId,
                                                          FppTest_SmJunction_SequenceU32::Signal signal,
                                                          U32 value) {
    ASSERT_EQ(smId, SmId::smJunctionSequenceU32);
    this->m_smJunctionSequenceU32_action_a_history.push(signal, value);
}

void SequenceU32::FppTest_SmJunction_SequenceU32_action_b(SmId smId, FppTest_SmJunction_SequenceU32::Signal signal) {
    ASSERT_EQ(smId, SmId::smJunctionSequenceU32);
    this->m_smJunctionSequenceU32_action_b_history.push(signal);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool SequenceU32::FppTest_SmJunction_SequenceU32_guard_g1(SmId smId,
                                                          FppTest_SmJunction_SequenceU32::Signal signal) const {
    FW_ASSERT(smId == SmId::smJunctionSequenceU32, static_cast<FwAssertArgType>(smId));
    return this->m_smJunctionSequenceU32_guard_g1.call(signal);
}

bool SequenceU32::FppTest_SmJunction_SequenceU32_guard_g2(SmId smId,
                                                          FppTest_SmJunction_SequenceU32::Signal signal,
                                                          U32 value) const {
    FW_ASSERT(smId == SmId::smJunctionSequenceU32, static_cast<FwAssertArgType>(smId));
    return this->m_smJunctionSequenceU32_guard_g2.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SequenceU32::testG1True() {
#if 0
    this->m_smJunctionSequenceU32_action_a_history.clear();
    this->m_smJunctionSequenceU32_action_b_history.clear();
    this->m_smJunctionSequenceU32_guard_g1.reset();
    this->m_smJunctionSequenceU32_guard_g1.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 0);
    this->smJunctionSequenceU32_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S2);
#endif
}

void SequenceU32::testG1FalseG2True() {
#if 0
    this->m_smJunctionSequenceU32_action_a_history.clear();
    this->m_smJunctionSequenceU32_action_b_history.clear();
    this->m_smJunctionSequenceU32_guard_g1.reset();
    this->m_smJunctionSequenceU32_guard_g2.reset();
    this->m_smJunctionSequenceU32_guard_g2.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 0);
    this->smJunctionSequenceU32_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 0);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S3);
#endif
}

void SequenceU32::testG1FalseG2False() {
#if 0
    this->m_smJunctionSequenceU32_action_a_history.clear();
    this->m_smJunctionSequenceU32_action_b_history.clear();
    this->m_smJunctionSequenceU32_guard_g1.reset();
    this->m_smJunctionSequenceU32_guard_g2.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 0);
    this->smJunctionSequenceU32_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g1.getCallHistory().getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_guard_g2.getCallHistory().getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smJunctionSequenceU32_action_b_history.getItemAt(0), SmJunction_SequenceU32::Signal::s);
    ASSERT_EQ(this->smJunctionSequenceU32_getState(), SmJunction_SequenceU32::State::S4);
#endif
}

}  // namespace SmInstanceJunction

}  // namespace FppTest
