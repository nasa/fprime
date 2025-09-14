// ======================================================================
// \title  BasicGuardTestArrayTester.cpp
// \author bocchino
// \brief  cpp file for BasicGuardTestArrayTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicGuardTestArrayTester.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicGuardTestArrayTester::BasicGuardTestArrayTester(const char* const compName)
    : BasicGuardTestArrayComponentBase(compName),
      m_smStateBasicGuardTestArray_action_a_history(),
      m_smStateBasicGuardTestArray_guard_g() {}

BasicGuardTestArrayTester::~BasicGuardTestArrayTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicGuardTestArrayTester::FppTest_SmState_BasicGuardTestArray_action_a(
    SmId smId,
    FppTest_SmState_BasicGuardTestArray::Signal signal,
    const SmHarness::TestArray& value) {
    ASSERT_EQ(smId, SmId::smStateBasicGuardTestArray);
    this->m_smStateBasicGuardTestArray_action_a_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool BasicGuardTestArrayTester::FppTest_SmState_BasicGuardTestArray_guard_g(
    SmId smId,
    FppTest_SmState_BasicGuardTestArray::Signal signal,
    const SmHarness::TestArray& value) const {
    FW_ASSERT(smId == SmId::smStateBasicGuardTestArray, static_cast<FwAssertArgType>(smId));
    return this->m_smStateBasicGuardTestArray_guard_g.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicGuardTestArrayTester::testFalse() {
    this->m_smStateBasicGuardTestArray_action_a_history.clear();
    this->m_smStateBasicGuardTestArray_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestArray_getState(), SmState_BasicGuardTestArray::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testArray();
    this->smStateBasicGuardTestArray_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestArray_getState(), SmState_BasicGuardTestArray::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestArray::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getSize(), 0);
}

void BasicGuardTestArrayTester::testTrue() {
    this->m_smStateBasicGuardTestArray_action_a_history.clear();
    this->m_smStateBasicGuardTestArray_guard_g.reset();
    this->m_smStateBasicGuardTestArray_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestArray_getState(), SmState_BasicGuardTestArray::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testArray();
    this->smStateBasicGuardTestArray_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestArray_getState(), SmState_BasicGuardTestArray::State::T);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestArray::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getSignals().getItemAt(0),
              SmState_BasicGuardTestArray::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestArray_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest