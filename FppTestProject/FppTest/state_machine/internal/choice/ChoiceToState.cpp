// ======================================================================
//
// \title  ChoiceToState.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a choice-to-state transition (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/ChoiceToState.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

ChoiceToState::ChoiceToState() : ChoiceToStateStateMachineBase(), m_actionHistory(), m_guard_g() {}

void ChoiceToState::action_exitS1(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S1);
}

void ChoiceToState::action_a(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::A);
}

bool ChoiceToState::guard_g(Signal signal) const {
    return this->m_guard_g.call(signal);
}

void ChoiceToState::action_enterS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S2);
}

void ChoiceToState::action_enterS3(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S3);
}

void ChoiceToState::testTrue() {
    this->m_actionHistory.clear();
    this->m_guard_g.reset();
    this->m_guard_g.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_actionHistory.getSize(), 0);
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(0), Signal::s);
    const FwIndexType expectedSize = 5;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::s);
    }
    const auto& values = this->m_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(values.getItemAt(3), ActionId::A);
    ASSERT_EQ(values.getItemAt(4), ActionId::ENTER_S3);
    ASSERT_EQ(this->getState(), State::S2_S3);
}

void ChoiceToState::testFalse() {
    this->m_actionHistory.clear();
    this->m_guard_g.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_actionHistory.getSize(), 0);
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g.getCallHistory().getItemAt(0), Signal::s);
    const FwIndexType expectedSize = 4;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::s);
    }
    const auto& values = this->m_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(values.getItemAt(3), ActionId::ENTER_S3);
    ASSERT_EQ(this->getState(), State::S2_S3);
}

}  // namespace SmChoice

}  // end namespace FppTest
