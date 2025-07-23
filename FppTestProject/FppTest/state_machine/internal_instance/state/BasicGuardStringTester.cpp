// ======================================================================
// \title  BasicGuardStringTester.cpp
// \author bocchino
// \brief  cpp file for BasicGuardStringTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/FppConstantsAc.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardStringTester.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicGuardStringTester ::BasicGuardStringTester(const char* const compName)
    : BasicGuardStringComponentBase(compName),
      m_smStateBasicGuardString_action_a_history(),
      m_smStateBasicGuardString_guard_g() {}

BasicGuardStringTester ::~BasicGuardStringTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicGuardStringTester ::FppTest_SmState_BasicGuardString_action_a(SmId smId,
                                                                        FppTest_SmState_BasicGuardString::Signal signal,
                                                                        const Fw::StringBase& value) {
    ASSERT_EQ(smId, SmId::smStateBasicGuardString);
    this->m_smStateBasicGuardString_action_a_history.push(signal, Fw::String(value));
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool BasicGuardStringTester ::FppTest_SmState_BasicGuardString_guard_g(SmId smId,
                                                                       FppTest_SmState_BasicGuardString::Signal signal,
                                                                       const Fw::StringBase& value) const {
    FW_ASSERT(smId == SmId::smStateBasicGuardString, static_cast<FwAssertArgType>(smId));
    return this->m_smStateBasicGuardString_guard_g.call(signal, Fw::String(value));
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicGuardStringTester::testFalse() {
    this->m_smStateBasicGuardString_action_a_history.clear();
    this->m_smStateBasicGuardString_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardString_getState(), SmState_BasicGuardString::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSize(), 0);
    Fw::String value;
    SmHarness::Pick::string(value, SmState::basicGuardStringSize);
    this->smStateBasicGuardString_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardString_getState(), SmState_BasicGuardString::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardString::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getSize(), 0);
}

void BasicGuardStringTester::testTrue() {
    this->m_smStateBasicGuardString_action_a_history.clear();
    this->m_smStateBasicGuardString_guard_g.reset();
    this->m_smStateBasicGuardString_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardString_getState(), SmState_BasicGuardString::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSize(), 0);
    Fw::String value;
    SmHarness::Pick::string(value, SmState::basicGuardStringSize);
    this->smStateBasicGuardString_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardString_getState(), SmState_BasicGuardString::State::T);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardString::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardString_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getSignals().getItemAt(0),
              SmState_BasicGuardString::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardString_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
