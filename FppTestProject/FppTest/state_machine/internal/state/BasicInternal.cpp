// ======================================================================
//
// \title  BasicInternal.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/BasicInternal.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

BasicInternal::BasicInternal() : BasicInternalStateMachineBase(), m_action_a_history() {}

void BasicInternal::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void BasicInternal::test() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    this->m_action_a_history.clear();
    this->sendSignal_s();
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::s);
}

}  // namespace SmState

}  // end namespace FppTest
