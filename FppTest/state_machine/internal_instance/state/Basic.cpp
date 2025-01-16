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
    : BasicComponentBase(compName), m_basic1_action_a_history(), m_smStateBasic1_action_a_history() {}

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
    ASSERT_TRUE((smId == SmId::basic1) || (smId == SmId::basic2));
    if (smId == SmId::basic1) {
        this->m_basic1_action_a_history.push(signal);
    } else {
        this->m_basic2_action_a_history.push(signal);
    }
}

void Basic ::FppTest_SmState_Basic_action_a(SmId smId, FppTest_SmState_Basic::Signal signal) {
    ASSERT_TRUE((smId == SmId::smStateBasic1) || (smId == SmId::smStateBasic2));
    if (smId == SmId::smStateBasic1) {
        this->m_smStateBasic1_action_a_history.push(signal);
    } else {
        this->m_smStateBasic2_action_a_history.push(signal);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Basic::test() {
    this->m_basic1_action_a_history.clear();
    this->m_smStateBasic1_action_a_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->basic1_getState(), Basic_Basic::State::S);
    ASSERT_EQ(this->smStateBasic1_getState(), SmState_Basic::State::S);
    ASSERT_EQ(this->m_basic1_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_basic2_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_smStateBasic1_action_a_history.getSize(), 0);
    {
        // Send signal s to basic1
        this->basic1_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->basic1_getState(), Basic_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_basic1_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_basic1_action_a_history.getItemAt(i), Basic_Basic::Signal::s);
        }
    }
    {
        // Send signal s to basic2
        this->basic2_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->basic2_getState(), Basic_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_basic2_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_basic2_action_a_history.getItemAt(i), Basic_Basic::Signal::s);
        }
    }
    {
        // Send signal s to smStateBasic1
        this->smStateBasic1_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStateBasic1_getState(), SmState_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_smStateBasic1_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_smStateBasic1_action_a_history.getItemAt(i), SmState_Basic::Signal::s);
        }
    }
    {
        // Send signal s to smStateBasic2
        this->smStateBasic2_sendSignal_s();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStateBasic2_getState(), SmState_Basic::State::T);
        const FwSizeType expectedSize = 6;
        ASSERT_EQ(this->m_smStateBasic2_action_a_history.getSize(), expectedSize);
        for (FwSizeType i = 0; i < expectedSize; i++) {
            ASSERT_EQ(this->m_smStateBasic2_action_a_history.getItemAt(i), SmState_Basic::Signal::s);
        }
    }
}

}  // namespace SmInstanceState

}  // namespace FppTest
