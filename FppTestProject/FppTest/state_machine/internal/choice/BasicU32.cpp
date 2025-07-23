// ======================================================================
//
// \title  BasicU32.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with a U32 choice (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/BasicU32.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

BasicU32::BasicU32() : BasicU32StateMachineBase(), m_action_a_history(), m_action_b_history(), m_guard_g() {}

void BasicU32::action_a(Signal signal, U32 value) {
    this->m_action_a_history.push(signal, value);
}

void BasicU32::action_b(Signal signal) {
    this->m_action_b_history.push(signal);
}

bool BasicU32::guard_g(Signal signal, U32 value) const {
    return this->m_guard_g.call(signal, value);
}

void BasicU32::testTrue() {
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
    const U32 value = STest::Pick::any();
    this->sendSignal_s(value);
    this->sendSignal_s(value);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S2);
}

void BasicU32::testFalse() {
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
    const U32 value = STest::Pick::any();
    this->sendSignal_s(value);
    this->sendSignal_s(value);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_action_b_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->getState(), State::S3);
}

}  // namespace SmChoice

}  // end namespace FppTest
