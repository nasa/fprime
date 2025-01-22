// ======================================================================
//
// \title  Nested.hpp
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
#include <limits>

#include "FppTest/state_machine/internal/initial/Nested.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmInitial {

Nested::Nested() : NestedStateMachineBase(), m_action_a_history() {}

void Nested::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void Nested::test() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S_T);
    const FwSizeType expectedActionSize = 6;
    ASSERT_EQ(this->m_action_a_history.getSize(), expectedActionSize);
    for (FwSizeType i = 0; i < expectedActionSize; i++) {
        ASSERT_EQ(this->m_action_a_history.getItemAt(i), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
}

}  // namespace SmInitial

}  // end namespace FppTest
