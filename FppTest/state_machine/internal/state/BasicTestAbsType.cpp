// ======================================================================
//
// \title  BasicTestAbsType.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with TestAbsType actions (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/BasicTestAbsType.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

BasicTestAbsType::BasicTestAbsType() : BasicTestAbsTypeStateMachineBase(), m_action_a_history(), m_action_b_history() {}

void BasicTestAbsType::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void BasicTestAbsType::action_b(Signal signal, const SmHarness::TestAbsType& value) {
    this->m_action_b_history.push(signal, value);
}

void BasicTestAbsType::test() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    const SmHarness::TestAbsType value = SmHarness::Pick::testAbsType();
    this->sendSignal_s(value);
    ASSERT_EQ(this->getState(), State::T);
    this->sendSignal_s(value);
    ASSERT_EQ(this->getState(), State::T);
    const FwSizeType expectedASize = 5;
    ASSERT_EQ(this->m_action_a_history.getSize(), expectedASize);
    for (FwSizeType i = 0; i < expectedASize; i++) {
        ASSERT_EQ(this->m_action_a_history.getItemAt(i), Signal::s);
    }
    ASSERT_EQ(this->m_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_action_b_history.getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_b_history.getValues().getItemAt(0), value);
}

}  // namespace SmState

}  // end namespace FppTest
