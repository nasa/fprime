// ======================================================================
// \title  BasicTestEnum.cpp
// \author bocchino
// \brief  cpp file for BasicTestEnum component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicTestEnum.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicTestEnum::BasicTestEnum(const char* const compName)
    : BasicTestEnumComponentBase(compName),
      m_smStateBasicTestEnum_action_a_history(),
      m_smStateBasicTestEnum_action_b_history() {}

BasicTestEnum::~BasicTestEnum() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicTestEnum::FppTest_SmState_BasicTestEnum_action_a(SmId smId, FppTest_SmState_BasicTestEnum::Signal signal) {
    this->m_smStateBasicTestEnum_action_a_history.push(signal);
}

void BasicTestEnum::FppTest_SmState_BasicTestEnum_action_b(SmId smId,
                                                           FppTest_SmState_BasicTestEnum::Signal signal,
                                                           const SmHarness::TestEnum& value) {
    this->m_smStateBasicTestEnum_action_b_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicTestEnum::test() {
    this->m_smStateBasicTestEnum_action_a_history.clear();
    this->m_smStateBasicTestEnum_action_b_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicTestEnum_getState(), SmState_BasicTestEnum::State::S);
    ASSERT_EQ(this->m_smStateBasicTestEnum_action_a_history.getSize(), 0);
    const SmHarness::TestEnum value = SmHarness::Pick::testEnum();
    this->smStateBasicTestEnum_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicTestEnum_getState(), SmState_BasicTestEnum::State::T);
    const FwSizeType expectedASize = 5;
    ASSERT_EQ(this->m_smStateBasicTestEnum_action_a_history.getSize(), expectedASize);
    for (FwSizeType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_smStateBasicTestEnum_action_a_history.getItemAt(i), SmState_BasicTestEnum::Signal::s);
    }
    ASSERT_EQ(this->m_smStateBasicTestEnum_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicTestEnum_action_b_history.getSignals().getItemAt(0),
              SmState_BasicTestEnum::Signal::s);
    ASSERT_EQ(this->m_smStateBasicTestEnum_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
