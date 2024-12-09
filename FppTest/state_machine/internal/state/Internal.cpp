// ======================================================================
//
// \title  Internal.hpp
// \author R. Bocchino
// \brief  Test class for internal state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/Internal.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

Internal::Internal() : InternalStateMachineBase(), m_action_a_history() {}

void Internal::action_a(Signal signal) {
    this->m_action_a_history.push(signal);
}

void Internal::testInit() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
}

void Internal::testS2_internal() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    this->sendSignal_S1_internal();
    ASSERT_EQ(this->getState(), State::S1_S2);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::S1_internal);
}

void Internal::testS2_to_S3() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    this->sendSignal_S2_to_S3();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S1_S3);
    ASSERT_EQ(this->m_action_a_history.getSize(), 0);
}

void Internal::testS3_internal() {
    this->m_action_a_history.clear();
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S1_S3);
    this->sendSignal_S1_internal();
    ASSERT_EQ(this->getState(), State::S1_S3);
    ASSERT_EQ(this->m_action_a_history.getSize(), 1);
    ASSERT_EQ(this->m_action_a_history.getItemAt(0), Signal::S1_internal);
}

}  // namespace SmState

}  // end namespace FppTest
