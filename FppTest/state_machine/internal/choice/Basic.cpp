// ======================================================================
//
// \title  Basic.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with a choice (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/Basic.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

Basic::Basic() : BasicStateMachineBase(), m_action_a_history(), m_action_b_history(), m_guard_g() {}

void Basic::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void Basic::action_b(Signal signal) {
    this->m_action_b_history.push(signal);
}

bool Basic::guard_g(Signal signal) const {
    return this->m_guard_g.call(signal);
}

void Basic::testTrue() {
    this->m_action_a_history.clear();
    this->m_action_b_history.clear();
    this->m_guard_g.reset();
    this->m_guard_g.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S2);
}

void Basic::testFalse() {
    this->m_action_a_history.clear();
    this->m_action_b_history.clear();
    this->m_guard_g.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_action_b_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->getState(), State::S3);
}

}  // namespace SmChoice

}  // end namespace FppTest
