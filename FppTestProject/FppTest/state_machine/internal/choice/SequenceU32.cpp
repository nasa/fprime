// ======================================================================
//
// \title  SequenceU32.hpp
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

#include "FppTest/state_machine/internal/choice/SequenceU32.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

SequenceU32::SequenceU32()
    : SequenceU32StateMachineBase(), m_action_a_history(), m_action_b_history(), m_guard_g1(), m_guard_g2() {}

void SequenceU32::action_a(Signal signal, U32 value) {
    this->m_action_a_history.push(signal, value);
}

void SequenceU32::action_b(Signal signal) {
    this->m_action_b_history.push(signal);
}

bool SequenceU32::guard_g1(Signal signal) const {
    return this->m_guard_g1.call(signal);
}

bool SequenceU32::guard_g2(Signal signal, U32 value) const {
    return this->m_guard_g2.call(signal, value);
}

void SequenceU32::testG1True() {
    this->m_action_a_history.clear();
    this->m_action_b_history.clear();
    this->m_guard_g1.reset();
    this->m_guard_g1.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->sendSignal_s(value);
    this->sendSignal_s(value);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S2);
}

void SequenceU32::testG1FalseG2True() {
    this->m_action_a_history.clear();
    this->m_action_b_history.clear();
    this->m_guard_g1.reset();
    this->m_guard_g2.reset();
    this->m_guard_g2.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->sendSignal_s(value);
    this->sendSignal_s(value);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S3);
}

void SequenceU32::testG1FalseG2False() {
    this->m_action_a_history.clear();
    this->m_action_b_history.clear();
    this->m_guard_g1.reset();
    this->m_guard_g2.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    const U32 value = STest::Pick::any();
    this->sendSignal_s(value);
    this->sendSignal_s(value);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_action_b_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->getState(), State::S4);
}

}  // namespace SmChoice

}  // end namespace FppTest
