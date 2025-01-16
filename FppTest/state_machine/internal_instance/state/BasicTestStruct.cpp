// ======================================================================
// \title  BasicTestStruct.cpp
// \author bocchino
// \brief  cpp file for BasicTestStruct component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicTestStruct.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicTestStruct::BasicTestStruct(const char* const compName)
    : BasicTestStructComponentBase(compName),
      m_smStateBasicTestStruct_action_a_history(),
      m_smStateBasicTestStruct_action_b_history() {}

BasicTestStruct::~BasicTestStruct() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicTestStruct::FppTest_SmState_BasicTestStruct_action_a(SmId smId,
                                                               FppTest_SmState_BasicTestStruct::Signal signal) {
    this->m_smStateBasicTestStruct_action_a_history.push(signal);
}

void BasicTestStruct::FppTest_SmState_BasicTestStruct_action_b(SmId smId,
                                                               FppTest_SmState_BasicTestStruct::Signal signal,
                                                               const SmHarness::TestStruct& value) {
    this->m_smStateBasicTestStruct_action_b_history.push(signal, value);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicTestStruct::test() {
    this->m_smStateBasicTestStruct_action_a_history.clear();
    this->m_smStateBasicTestStruct_action_b_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicTestStruct_getState(), SmState_BasicTestStruct::State::S);
    ASSERT_EQ(this->m_smStateBasicTestStruct_action_a_history.getSize(), 0);
    const SmHarness::TestStruct value = SmHarness::Pick::testStruct();
    this->smStateBasicTestStruct_sendSignal_s(value);
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicTestStruct_getState(), SmState_BasicTestStruct::State::T);
    const FwSizeType expectedASize = 5;
    ASSERT_EQ(this->m_smStateBasicTestStruct_action_a_history.getSize(), expectedASize);
    for (FwSizeType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_smStateBasicTestStruct_action_a_history.getItemAt(i), SmState_BasicTestStruct::Signal::s);
    }
    ASSERT_EQ(this->m_smStateBasicTestStruct_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicTestStruct_action_b_history.getSignals().getItemAt(0),
              SmState_BasicTestStruct::Signal::s);
    ASSERT_EQ(this->m_smStateBasicTestStruct_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmInstanceState

}  // namespace FppTest
