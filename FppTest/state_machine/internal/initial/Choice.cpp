// ======================================================================
//
// \title  Choice.hpp
// \author R. Bocchino
// \brief  Test class for choice state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>
#include <limits>

#include "FppTest/state_machine/internal/initial/Choice.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmInitial {

Choice::Choice() : ChoiceStateMachineBase(), m_action_a_history(), m_guard_g() {}

void Choice::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

bool Choice::guard_g(Signal signal) const {
    return m_guard_g.call(signal);
}

void Choice::testFalse() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::T);
    const FwSizeType expectedActionSize = 5;
    const FwSizeType expectedGuardSize = 1;
    this->checkActionsAndGuards(expectedActionSize, expectedGuardSize);
}

void Choice::testTrue() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    this->m_guard_g.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->m_state, State::S);
    const FwSizeType expectedActionSize = 3;
    const FwSizeType expectedGuardSize = 1;
    this->checkActionsAndGuards(expectedActionSize, expectedGuardSize);
}

void Choice::checkActionsAndGuards(FwSizeType expectedActionSize, FwSizeType expectedGuardSize) {
    ASSERT_EQ(this->m_action_a_history.getSize(), expectedActionSize);
    for (FwSizeType i = 0; i < expectedActionSize; i++) {
        ASSERT_EQ(this->m_action_a_history.getItemAt(i), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), expectedGuardSize);
    for (FwSizeType i = 0; i < expectedGuardSize; i++) {
        ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(i), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
}

}  // namespace SmInitial

}  // end namespace FppTest
