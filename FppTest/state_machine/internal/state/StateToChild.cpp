// ======================================================================
//
// \title  StateToChild.hpp
// \author R. Bocchino
// \brief  Test class for state-to-self state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/StateToChild.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

StateToChild::StateToChild() : StateToChildStateMachineBase(), m_actionHistory() {}

void StateToChild::action_exitS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S2);
}

void StateToChild::action_exitS3(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::EXIT_S3);
}

void StateToChild::action_a(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::A);
}

void StateToChild::action_enterS2(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S2);
}

void StateToChild::action_enterS3(Signal signal) {
    this->m_actionHistory.push(signal, ActionId::ENTER_S3);
}

void StateToChild::testInit() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    ASSERT_EQ(this->m_actionHistory.getSize(), 1);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    ASSERT_EQ(signals.getItemAt(0), Signal::__FPRIME_AC_INITIAL_TRANSITION);
    ASSERT_EQ(actions.getItemAt(0), ActionId::ENTER_S2);
}

void StateToChild::testS2_to_S2() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->m_actionHistory.clear();
    this->sendSignal_S1_to_S2();
    ASSERT_EQ(this->getState(), State::S1_S2);
    const FwSizeType expectedSize = 3;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S2);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S2);
    ASSERT_EQ(actions.getItemAt(1), ActionId::A);
    ASSERT_EQ(actions.getItemAt(2), ActionId::ENTER_S2);
}

void StateToChild::testS2_to_S3() {
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

void StateToChild::testS3_to_S2() {
    this->m_actionHistory.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->sendSignal_S2_to_S3();
    this->m_actionHistory.clear();
    ASSERT_EQ(this->getState(), State::S1_S3);
    this->sendSignal_S1_to_S2();
    ASSERT_EQ(this->getState(), State::S1_S2);
    const FwSizeType expectedSize = 3;
    ASSERT_EQ(this->m_actionHistory.getSize(), expectedSize);
    const auto& signals = this->m_actionHistory.getSignals();
    const auto& actions = this->m_actionHistory.getValues();
    for (FwSizeType i = 0; i < expectedSize; i++) {
        ASSERT_EQ(signals.getItemAt(i), Signal::S1_to_S2);
    }
    ASSERT_EQ(actions.getItemAt(0), ActionId::EXIT_S3);
    ASSERT_EQ(actions.getItemAt(1), ActionId::A);
    ASSERT_EQ(actions.getItemAt(2), ActionId::ENTER_S2);
}
}  // namespace SmState

}  // end namespace FppTest
