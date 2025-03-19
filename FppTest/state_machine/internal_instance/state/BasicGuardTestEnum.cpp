// ======================================================================
// \title  BasicGuardTestEnum.cpp
// \author bocchino
// \brief  cpp file for BasicGuardTestEnum component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicGuardTestEnum.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicGuardTestEnum::BasicGuardTestEnum(const char* const compName)
    : BasicGuardTestEnumComponentBase(compName),
      m_smStateBasicGuardTestEnum_action_a_history(),
      m_smStateBasicGuardTestEnum_guard_g() {}

BasicGuardTestEnum::~BasicGuardTestEnum() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicGuardTestEnum::FppTest_SmState_BasicGuardTestEnum_action_a(SmId smId,
                                                                     FppTest_SmState_BasicGuardTestEnum::Signal signal,
                                                                     const SmHarness::TestEnum& value) {
    ASSERT_EQ(smId, SmId::smStateBasicGuardTestEnum);
    this->m_smStateBasicGuardTestEnum_action_a_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool BasicGuardTestEnum::FppTest_SmState_BasicGuardTestEnum_guard_g(SmId smId,
                                                                    FppTest_SmState_BasicGuardTestEnum::Signal signal,
                                                                    const SmHarness::TestEnum& value) const {
    FW_ASSERT(smId == SmId::smStateBasicGuardTestEnum, static_cast<FwAssertArgType>(smId));
    return this->m_smStateBasicGuardTestEnum_guard_g.call(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicGuardTestEnum::testFalse() {
    this->m_smStateBasicGuardTestEnum_action_a_history.clear();
    this->m_smStateBasicGuardTestEnum_guard_g.reset();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestEnum_getState(), SmState_BasicGuardTestEnum::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testEnum();
    this->smStateBasicGuardTestEnum_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestEnum_getState(), SmState_BasicGuardTestEnum::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestEnum::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getSize(), 0);
}

void BasicGuardTestEnum::testTrue() {
    this->m_smStateBasicGuardTestEnum_action_a_history.clear();
    this->m_smStateBasicGuardTestEnum_guard_g.reset();
    this->m_smStateBasicGuardTestEnum_guard_g.setReturnValue(true);
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicGuardTestEnum_getState(), SmState_BasicGuardTestEnum::State::S);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSize(), 0);
    const auto value = SmHarness::Pick::testEnum();
    this->smStateBasicGuardTestEnum_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicGuardTestEnum_getState(), SmState_BasicGuardTestEnum::State::T);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getSignals().getItemAt(0),
              SmState_BasicGuardTestEnum::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getSignals().getItemAt(0),
              SmState_BasicGuardTestEnum::Signal::s);
    ASSERT_EQ(this->m_smStateBasicGuardTestEnum_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
