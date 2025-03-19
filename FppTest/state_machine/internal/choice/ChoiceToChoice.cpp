// ======================================================================
//
// \title  ChoiceToChoice.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a choice-to-choice transition (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/choice/ChoiceToChoice.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmChoice {

ChoiceToChoice::ChoiceToChoice() : ChoiceToChoiceStateMachineBase(), m_actionHistory(), m_guard_g1(), m_guard_g2() {}

void ChoiceToChoice::action_exitS1(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S1);
}

void ChoiceToChoice::action_a(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::A);
}

void ChoiceToChoice::action_enterS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S2);
}

bool ChoiceToChoice::guard_g1(Signal signal) const {
    return this->m_guard_g1.call(signal);
}

bool ChoiceToChoice::guard_g2(Signal signal) const {
    return this->m_guard_g2.call(signal);
}

void ChoiceToChoice::testG1True() {
    this->m_actionHistory.clear();
    this->m_guard_g1.reset();
    this->m_guard_g2.reset();
    this->m_guard_g1.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_actionHistory.getSize(), 0);
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::s);
    }
    const auto& values = this->m_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->getState(), State::S2_S3);
}

void ChoiceToChoice::testG1FalseG2True() {
    this->m_actionHistory.clear();
    this->m_guard_g1.reset();
    this->m_guard_g2.reset();
    this->m_guard_g2.setReturnValue(true);
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_actionHistory.getSize(), 0);
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::s);
    }
    const auto& values = this->m_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->getState(), State::S2_S3);
}

void ChoiceToChoice::testG1FalseG2False() {
    this->m_actionHistory.clear();
    this->m_guard_g1.reset();
    this->m_guard_g2.reset();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 0);
    ASSERT_EQ(this->m_actionHistory.getSize(), 0);
    this->sendSignal_s();
    this->sendSignal_s();
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    ASSERT_EQ(this->m_guard_g2.getCallHistory().getSize(), 1);
    ASSERT_EQ(this->m_guard_g1.getCallHistory().getItemAt(0), Signal::s);
    const FwIndexType expectedSize = 3;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    for (FwIndexType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::s);
    }
    const auto& values = this->m_actionHistory.getValues();
    ASSERT_EQ(values.getItemAt(0), ActionId::EXIT_S1);
    ASSERT_EQ(values.getItemAt(1), ActionId::A);
    ASSERT_EQ(values.getItemAt(2), ActionId::ENTER_S2);
    ASSERT_EQ(this->getState(), State::S2_S4);
}

}  // namespace SmChoice

}  // end namespace FppTest
