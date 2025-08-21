// ======================================================================
// \title  BasicInternalTester.cpp
// \author bocchino
// \brief  cpp file for BasicInternalTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicInternalTester.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicInternalTester::BasicInternalTester(const char* const compName)
    : BasicInternalComponentBase(compName), m_smStateBasicInternal_action_a_history() {}

BasicInternalTester::~BasicInternalTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicInternalTester::FppTest_SmState_BasicInternal_action_a(SmId smId,
                                                                 FppTest_SmState_BasicInternal::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateBasicInternal);
    this->m_smStateBasicInternal_action_a_history.push(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicInternalTester::test() {
    this->m_smStateBasicInternal_action_a_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicInternal_getState(), SmState_BasicInternal::State::S);
    ASSERT_EQ(this->m_smStateBasicInternal_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicInternal_action_a_history.getItemAt(0),
              SmState_BasicInternal::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    this->m_smStateBasicInternal_action_a_history.clear();
    this->smStateBasicInternal_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicInternal_getState(), SmState_BasicInternal::State::S);
    ASSERT_EQ(this->m_smStateBasicInternal_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicInternal_action_a_history.getItemAt(0), SmState_BasicInternal::Signal::s);
}

}  // namespace SmInstanceState

}  // namespace FppTest
