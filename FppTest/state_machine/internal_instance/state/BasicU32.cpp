// ======================================================================
// \title  BasicU32.cpp
// \author bocchino
// \brief  cpp file for BasicU32 component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicU32.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicU32::BasicU32(const char* const compName)
    : BasicU32ComponentBase(compName), m_smStateBasicU32_action_a_history(), m_smStateBasicU32_action_b_history() {}

BasicU32::~BasicU32() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicU32::FppTest_SmState_BasicU32_action_a(SmId smId, FppTest_SmState_BasicU32::Signal signal) {
    this->m_smStateBasicU32_action_a_history.push(signal);
}

void BasicU32::FppTest_SmState_BasicU32_action_b(SmId smId, FppTest_SmState_BasicU32::Signal signal, U32 value) {
    this->m_smStateBasicU32_action_b_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicU32::test() {
    this->m_smStateBasicU32_action_a_history.clear();
    this->m_smStateBasicU32_action_b_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicU32_getState(), SmState_BasicU32::State::S);
    ASSERT_EQ(this->m_smStateBasicU32_action_a_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->smStateBasicU32_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicU32_getState(), SmState_BasicU32::State::T);
    const FwSizeType expectedASize = 5;
    ASSERT_EQ(this->m_smStateBasicU32_action_a_history.getSize(), expectedASize);
    for (FwSizeType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_smStateBasicU32_action_a_history.getItemAt(i), SmState_BasicU32::Signal::s);
    }
    ASSERT_EQ(this->m_smStateBasicU32_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicU32_action_b_history.getSignals().getItemAt(0), SmState_BasicU32::Signal::s);
    ASSERT_EQ(this->m_smStateBasicU32_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
