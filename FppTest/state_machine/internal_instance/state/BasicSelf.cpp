// ======================================================================
// \title  BasicSelf.cpp
// \author bocchino
// \brief  cpp file for BasicSelf component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/BasicSelf.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

BasicSelf::BasicSelf(const char* const compName)
    : BasicSelfComponentBase(compName), m_smStateBasicSelf_action_a_history() {}

BasicSelf::~BasicSelf() {}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void BasicSelf::FppTest_SmState_BasicSelf_action_a(SmId smId, FppTest_SmState_BasicSelf::Signal signal) {
    ASSERT_EQ(smId, SmId::smStateBasicSelf);
    this->m_smStateBasicSelf_action_a_history.push(signal);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BasicSelf::test() {
    this->m_smStateBasicSelf_action_a_history.clear();
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStateBasicSelf_getState(), SmState_BasicSelf::State::S);
    ASSERT_EQ(this->m_smStateBasicSelf_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_smStateBasicSelf_action_a_history.getItemAt(0),
              SmState_BasicSelf::Signal::__FPRIME_AC_INITIAL_TRANSITION);
    this->m_smStateBasicSelf_action_a_history.clear();
    this->smStateBasicSelf_sendSignal_s();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStateBasicSelf_getState(), SmState_BasicSelf::State::S);
    const FwSizeType expectedSize = 6;
    ASSERT_EQ(this->m_smStateBasicSelf_action_a_history.getSize(), expectedSize);
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(this->m_smStateBasicSelf_action_a_history.getItemAt(i), SmState_BasicSelf::Signal::s);
    }
}

}  // namespace SmInstanceState

}  // namespace FppTest
