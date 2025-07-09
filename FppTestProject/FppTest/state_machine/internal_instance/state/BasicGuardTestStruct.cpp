// ======================================================================
// \title  BasicGuardTestStruct.cpp
// \author bocchino
// \brief  cpp file for BasicGuardTestStruct component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicGuardTestStruct.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicGuardTestStruct::BasicGuardTestStruct(const char* const compName)
    : BasicGuardTestStructComponentBase(compName),
      m_smStateBasicGuardTestStruct_action_a_history(),
      m_smStateBasicGuardTestStruct_guard_g() {}

BasicGuardTestStruct::~BasicGuardTestStruct() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicGuardTestStruct::FppTest_SmState_BasicGuardTestStruct_action_a(
    SmId smId,
    FppTest_SmState_BasicGuardTestStruct::Signal signal,
    const SmHarness::TestStruct& value) {
    ASSERT_EQ(smId, SmId::smStateBasicGuardTestStruct);
    this->m_smStateBasicGuardTestStruct_action_a_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool BasicGuardTestStruct::FppTest_SmState_BasicGuardTestStruct_guard_g(
    SmId smId,
    FppTest_SmState_BasicGuardTestStruct::Signal signal,
    const SmHarness::TestStruct& value) const {
    FW_ASSERT(smId == SmId::smStateBasicGuardTestStruct, static_cast<FwAssertArgType>(smId));
    return this->m_smStateBasicGuardTestStruct_guard_g.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicGuardTestStruct::testFalse() {
    this->m_smStateBasicGuardTestStruct_action_a_history.clear();
    this->m_smStateBasicGuardTestStruct_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestStruct_getState(), SmState_BasicGuardTestStruct::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testStruct();
    this->smStateBasicGuardTestStruct_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestStruct_getState(), SmState_BasicGuardTestStruct::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestStruct::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getSize(), 0);
}

void BasicGuardTestStruct::testTrue() {
    this->m_smStateBasicGuardTestStruct_action_a_history.clear();
    this->m_smStateBasicGuardTestStruct_guard_g.reset();
    this->m_smStateBasicGuardTestStruct_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestStruct_getState(), SmState_BasicGuardTestStruct::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testStruct();
    this->smStateBasicGuardTestStruct_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestStruct_getState(), SmState_BasicGuardTestStruct::State::T);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestStruct::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getSignals().getItemAt(0),
              SmState_BasicGuardTestStruct::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestStruct_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
