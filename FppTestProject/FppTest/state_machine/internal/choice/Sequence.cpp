// ======================================================================
//
// \title  Sequence.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with choice sequence (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/Sequence.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

Sequence::Sequence()
    : SequenceStateMachineBase(), m_action_a_history(), m_action_b_history(), m_guard_g1(), m_guard_g2() {}

void Sequence::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void Sequence::action_b(Signal signal) {
    this->m_action_b_history.push(signal);
}

bool Sequence::guard_g1(Signal signal) const {
    return this->m_guard_g1.call(signal);
}

bool Sequence::guard_g2(Signal signal) const {
    return this->m_guard_g2.call(signal);
}

void Sequence::testG1True() {
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
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S2);
}

void Sequence::testG1FalseG2True() {
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
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_b_history.getSize(), 0);
    ASSERT_EQ(this->getState(), State::S3);
}

void Sequence::testG1FalseG2False() {
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
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_action_b_history.getSize(), 1);
    ASSERT_EQ(this->m_action_b_history.getItemAt(0), Signal::s);
    ASSERT_EQ(this->getState(), State::S4);
}

}  // namespace SmChoice

}  // end namespace FppTest
