// ======================================================================
// \title  BasicStringTester.cpp
// \author bocchino
// \brief  cpp file for BasicStringTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/FppConstantsAc.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicStringTester.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicStringTester ::BasicStringTester(const char* const compName)
    : BasicStringComponentBase(compName),
      m_smStateBasicString_action_a_history(),
      m_smStateBasicString_action_b_history() {}

BasicStringTester ::~BasicStringTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicStringTester ::FppTest_SmState_BasicString_action_a(SmId smId, FppTest_SmState_BasicString::Signal signal) {
    this->m_smStateBasicString_action_a_history.push(signal);
}

void BasicStringTester ::FppTest_SmState_BasicString_action_b(SmId smId,
                                                              FppTest_SmState_BasicString::Signal signal,
                                                              const Fw::StringBase& value) {
    this->m_smStateBasicString_action_b_history.push(signal, Fw::String(value));
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicStringTester::test() {
    this->m_smStateBasicString_action_a_history.clear();
    this->m_smStateBasicString_action_b_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicString_getState(), SmState_BasicString::State::S);
    ASSERT_EQ(this->m_smStateBasicString_action_a_history.getSize(), 0);
    Fw::String value;
    SmHarness::Pick::string(value, SmState::basicStringSize);
    this->smStateBasicString_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicString_getState(), SmState_BasicString::State::T);
    const FwIndexType expectedASize = 5;
    ASSERT_EQ(this->m_smStateBasicString_action_a_history.getSize(), expectedASize);
    for (FwIndexType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_smStateBasicString_action_a_history.getItemAt(i), SmState_BasicString::Signal::s);
    }
    ASSERT_EQ(this->m_smStateBasicString_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicString_action_b_history.getSignals().getItemAt(0), SmState_BasicString::Signal::s);
    ASSERT_EQ(this->m_smStateBasicString_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
