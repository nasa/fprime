// ======================================================================
// \title  BasicTestArrayTester.cpp
// \author bocchino
// \brief  cpp file for BasicTestArrayTester component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicTestArrayTester.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicTestArrayTester::BasicTestArrayTester(const char* const compName)
    : BasicTestArrayComponentBase(compName),
      m_smStateBasicTestArray_action_a_history(),
      m_smStateBasicTestArray_action_b_history() {}

BasicTestArrayTester::~BasicTestArrayTester() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicTestArrayTester::FppTest_SmState_BasicTestArray_action_a(SmId smId,
                                                                   FppTest_SmState_BasicTestArray::Signal signal) {
    this->m_smStateBasicTestArray_action_a_history.push(signal);
}

void BasicTestArrayTester::FppTest_SmState_BasicTestArray_action_b(SmId smId,
                                                                   FppTest_SmState_BasicTestArray::Signal signal,
                                                                   const SmHarness::TestArray& value) {
    this->m_smStateBasicTestArray_action_b_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicTestArrayTester::test() {
    this->m_smStateBasicTestArray_action_a_history.clear();
    this->m_smStateBasicTestArray_action_b_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicTestArray_getState(), SmState_BasicTestArray::State::S);
    ASSERT_EQ(this->m_smStateBasicTestArray_action_a_history.getSize(), 0);
    const SmHarness::TestArray value = SmHarness::Pick::testArray();
    this->smStateBasicTestArray_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicTestArray_getState(), SmState_BasicTestArray::State::T);
    const FwIndexType expectedASize = 5;
    ASSERT_EQ(this->m_smStateBasicTestArray_action_a_history.getSize(), expectedASize);
    for (FwIndexType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_smStateBasicTestArray_action_a_history.getItemAt(i), SmState_BasicTestArray::Signal::s);
    }
    ASSERT_EQ(this->m_smStateBasicTestArray_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicTestArray_action_b_history.getSignals().getItemAt(0),
              SmState_BasicTestArray::Signal::s);
    ASSERT_EQ(this->m_smStateBasicTestArray_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
