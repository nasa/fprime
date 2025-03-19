// ======================================================================
//
// \title  BasicGuardTestStruct.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with TestStruct guard (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/BasicGuardTestStruct.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

BasicGuardTestStruct::BasicGuardTestStruct()
    : BasicGuardTestStructStateMachineBase(), m_action_a_history(), m_guard_g() {}

void BasicGuardTestStruct::action_a(Signal signal, const SmHarness::TestStruct& value) {
    this->m_action_a_history.push(signal, value);
}

bool BasicGuardTestStruct::guard_g(Signal signal, const SmHarness::TestStruct& value) const {
    return this->m_guard_g.call(signal, value);
}

void BasicGuardTestStruct::testFalse() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 0);
    const SmHarness::TestStruct value = SmHarness::Pick::testStruct();
    this->sendSignal_s(value);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
}

void BasicGuardTestStruct::testTrue() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    this->m_guard_g.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    const SmHarness::TestStruct value = SmHarness::Pick::testStruct();
    this->sendSignal_s(value);
    ASSERT_EQ(this->getState(), State::T);
    this->sendSignal_s(value);
    ASSERT_EQ(this->getState(), State::T);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getValues().getItemAt(0), value);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getSignals().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_action_a_history.getValues().getItemAt(0), value);
}

}  // namespace SmState

}  // end namespace FppTest
