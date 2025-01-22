// ======================================================================
//
// \title  Basic.hpp
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

#include "FppTest/state_machine/internal/initial/Basic.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmInitial {

Basic::Basic() : BasicStateMachineBase(), m_action_a_history() {}

void Basic::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void Basic::test() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    const FwSizeType expectedSize = 3;
    ASSERT_EQ(this->m_action_a_history.getSize(), expectedSize);
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(this->m_action_a_history.getItemAt(i), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
}

}  // namespace SmInitial

}  // end namespace FppTest
