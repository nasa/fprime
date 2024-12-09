// ======================================================================
//
// \title  StateToState.hpp
// \author R. Bocchino
// \brief  Test class for state-to-state state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/StateToState.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

StateToState::StateToState() : StateToStateStateMachineBase(), m_actionHistory() {}

void StateToState::action_exitS1(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S1);
}

void StateToState::action_exitS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToState::action_exitS3(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToState::action_a(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::A);
}

void StateToState::action_enterS1(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S1);
}

void StateToState::action_enterS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToState::action_enterS3(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S3);
}

void StateToState::action_enterS4(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S4);
}

void StateToState::action_enterS5(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S5);
}

void StateToState::testInit() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S1);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S2);
}

void StateToState::testS2_to_S3() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->m_actionHistory.clear();
    this->sendSignal_S2_to_S3();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S1_S3);
    const FwSizeType expectedSize = 2;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S2_to_S3);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::ENTER_S3);
}

void StateToState::testS2_to_S4() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->m_actionHistory.clear();
    this->sendSignal_S1_to_S4();
    this->sendSignal_S1_to_S4();
    ASSERT_EQ(this->getState(), State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS2_to_S5() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->m_actionHistory.clear();
    this->sendSignal_S1_to_S5();
    this->sendSignal_S1_to_S5();
    ASSERT_EQ(this->getState(), State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S5);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS3_to_S4() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->sendSignal_S2_to_S3();
    this->m_actionHistory.clear();
    this->sendSignal_S1_to_S4();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S4);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

void StateToState::testS3_to_S5() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->sendSignal_S2_to_S3();
    this->m_actionHistory.clear();
    this->sendSignal_S1_to_S5();
    ASSERT_EQ(this->getState(), State::S4_S5);
    const FwSizeType expectedSize = 5;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S5);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::EXIT_S1);
    ASSERT_EQ(actions.getItemAt(2), ActionId::A);
    ASSERT_EQ(actions.getItemAt(3), ActionId::ENTER_S4);
    ASSERT_EQ(actions.getItemAt(4), ActionId::ENTER_S5);
}

}  // namespace SmState

}  // end namespace FppTest
