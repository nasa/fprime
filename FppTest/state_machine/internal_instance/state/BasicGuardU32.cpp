// ======================================================================
// \title  BasicGuardU32.cpp
// \author bocchino
// \brief  cpp file for BasicGuardU32 component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicGuardU32.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicGuardU32::BasicGuardU32(const char* const compName)
    : BasicGuardU32ComponentBase(compName),
      m_smStateBasicGuardU32_action_a_history(),
      m_smStateBasicGuardU32_guard_g() {}

BasicGuardU32::~BasicGuardU32() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicGuardU32::FppTest_SmState_BasicGuardU32_action_a(SmId smId,
                                                           FppTest_SmState_BasicGuardU32::Signal signal,
                                                           U32 value) {
    ASSERT_EQ(smId, SmId::smStateBasicGuardU32);
    this->m_smStateBasicGuardU32_action_a_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool BasicGuardU32::FppTest_SmState_BasicGuardU32_guard_g(SmId smId,
                                                          FppTest_SmState_BasicGuardU32::Signal signal,
                                                          U32 value) const {
    FW_ASSERT(smId == SmId::smStateBasicGuardU32, static_cast<FwAssertArgType>(smId));
    return this->m_smStateBasicGuardU32_guard_g.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicGuardU32::testFalse() {
    this->m_smStateBasicGuardU32_action_a_history.clear();
    this->m_smStateBasicGuardU32_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardU32_getState(), SmState_BasicGuardU32::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smStateBasicGuardU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardU32_getState(), SmState_BasicGuardU32::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardU32::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getSize(), 0);
}

void BasicGuardU32::testTrue() {
    this->m_smStateBasicGuardU32_action_a_history.clear();
    this->m_smStateBasicGuardU32_guard_g.reset();
    this->m_smStateBasicGuardU32_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardU32_getState(), SmState_BasicGuardU32::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smStateBasicGuardU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardU32_getState(), SmState_BasicGuardU32::State::T);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardU32::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardU32_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getSignals().getItemAt(0),
              SmState_BasicGuardU32::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardU32_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
