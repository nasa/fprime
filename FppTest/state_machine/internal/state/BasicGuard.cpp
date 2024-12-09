// ======================================================================
//
// \title  BasicGuard.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with guard (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/BasicGuard.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

BasicGuard::BasicGuard() : BasicGuardStateMachineBase(), m_action_a_history(), m_guard_g() {}

void BasicGuard::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

bool BasicGuard::guard_g(Signal signal) const {
    return this->m_guard_g.call(signal);
}

void BasicGuard::testFalse() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    this->sendSignal_s();
    ASSERT_EQ(this->getState(), State::S);
    this->checkActionsAndGuards(0, 1);
}

void BasicGuard::testTrue() {
    this->m_action_a_history.clear();
    this->m_guard_g.reset();
    this->m_guard_g.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
    this->sendSignal_s();
    ASSERT_EQ(this->getState(), State::T);
    this->sendSignal_s();
    ASSERT_EQ(this->getState(), State::T);
    this->checkActionsAndGuards(6, 1);
}

void BasicGuard::checkActionsAndGuards(FwSizeType expectedActionSize, FwSizeType expectedGuardSize) {
    ASSERT_EQ(this->m_action_a_history.getSize(), expectedActionSize);
    for (FwSizeType i = 0; i < expectedActionSize; i++) {
        ASSERT_EQ(this->m_action_a_history.getItemAt(i), Signal::s);
    }
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), expectedGuardSize);
    for (FwSizeType i = 0; i < expectedGuardSize; i++) {
        ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(i), Signal::s);
    }
}

}  // namespace SmState

}  // end namespace FppTest
