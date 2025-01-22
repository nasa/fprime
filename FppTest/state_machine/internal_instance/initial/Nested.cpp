// ======================================================================
// \title  Nested.cpp
// \author bocchino
// \brief  cpp file for Nested component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/initial/Nested.hpp"

namespace FppTest {

namespace SmInstanceInitial {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Nested::Nested(const char* const compName)
    : NestedComponentBase(compName), m_nested_action_a_history(), m_smInitialNested_action_a_history() {}

Nested::~Nested() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void Nested::FppTest_SmInitial_Nested_action_a(SmId smId, FppTest_SmInitial_Nested::Signal signal) {
    ASSERT_EQ(smId, SmId::smInitialNested);
    this->m_smInitialNested_action_a_history.push(signal);
}

void Nested::FppTest_SmInstanceInitial_Nested_Nested_action_a(SmId smId,
                                                              FppTest_SmInstanceInitial_Nested_Nested::Signal signal) {
    ASSERT_EQ(smId, SmId::nested);
    this->m_nested_action_a_history.push(signal);
}

// ----------------------------------------------------------------------
// Overflow hook implementations for internal state machines
// ----------------------------------------------------------------------

void Nested ::smInitialNested_stateMachineOverflowHook(SmId smId,
                                                       FwEnumStoreType signal,
                                                       Fw::SerializeBufferBase& buffer) {
    // Nothing to do
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Nested::test() {
    this->m_nested_action_a_history.clear();
    this->m_smInitialNested_action_a_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->nested_getState(), Nested_Nested::State::S_T);
    ASSERT_EQ(this->smInitialNested_getState(), SmInitial_Nested::State::S_T);
    const FwSizeType expectedActionSize = 6;
    ASSERT_EQ(this->m_nested_action_a_history.getSize(), expectedActionSize);
    ASSERT_EQ(this->m_smInitialNested_action_a_history.getSize(), expectedActionSize);
    for (FwSizeType i = 0; i < expectedActionSize; i++) {
        ASSERT_EQ(this->m_nested_action_a_history.getItemAt(i), Nested_Nested::Signal::__FPRIME_AC_INITIAL_TRANSITION);
        ASSERT_EQ(this->m_smInitialNested_action_a_history.getItemAt(i),
                  SmInitial_Nested::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
}

}  // namespace SmInstanceInitial

}  // namespace FppTest
