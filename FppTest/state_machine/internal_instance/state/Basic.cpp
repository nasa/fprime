// ======================================================================
// \title  Basic.cpp
// \author bocchino
// \brief  cpp file for Basic component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/Basic.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Basic ::Basic(const char* const compName)
    : BasicComponentBase(compName), m_basic_action_a_history(), m_smStateBasic_action_a_history() {}

Basic ::~Basic() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void Basic::schedIn_handler(FwIndexType portNum, U32 context) {
    // Nothing to do
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void Basic ::FppTest_SmInstanceState_Basic_Basic_action_a(SmId smId,
                                                          FppTest_SmInstanceState_Basic_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::basic);
    this->m_basic_action_a_history.push(signal);
}

void Basic ::FppTest_SmState_Basic_action_a(SmId smId, FppTest_SmState_Basic::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateBasic);
    this->m_smStateBasic_action_a_history.push(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Basic::test() {
    this->m_basic_action_a_history.clear();
    this->m_smStateBasic_action_a_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->basic_getState(), Basic_Basic::State::S);
    ASSERT_EQ(this->smStateBasic_getState(), SmState_Basic::State::S);
    ASSERT_EQ(this->m_basic_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasic_action_a_history.getSize(), 0);
    {
        // Send signal s to basic
        this->basic_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->basic_getState(), Basic_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_basic_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_basic_action_a_history.getItemAt(i), Basic_Basic::Signal::s);
        }
    }
    {
        // Send signal s to smStateBasic
        this->smStateBasic_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStateBasic_getState(), SmState_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_smStateBasic_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_smStateBasic_action_a_history.getItemAt(i), SmState_Basic::Signal::s);
        }
    }
}

}  // namespace SmInstanceState

}  // namespace FppTest
