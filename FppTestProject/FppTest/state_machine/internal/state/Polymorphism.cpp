// ======================================================================
//
// \title  Polymorphism.hpp
// \author R. Bocchino
// \brief  Test class for polymorphic state machine (implementation)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal/state/Polymorphism.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace FppTest {

namespace SmState {

Polymorphism::Polymorphism() : PolymorphismStateMachineBase() {}

void Polymorphism::testInit() {
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->m_id, id);
    ASSERT_EQ(this->getState(), State::S1_S2);
}

void Polymorphism::testS2_poly() {
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    this->sendSignal_poly();
    this->sendSignal_poly();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S4);
}

void Polymorphism::testS2_to_S3() {
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    ASSERT_EQ(this->getState(), State::S1_S2);
    this->sendSignal_S2_to_S3();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S1_S3);
}

void Polymorphism::testS3_poly() {
    const FwEnumStoreType id = SmHarness::Pick::stateMachineId();
    this->initBase(id);
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S1_S3);
    this->sendSignal_poly();
    this->sendSignal_poly();
    this->sendSignal_S2_to_S3();
    ASSERT_EQ(this->getState(), State::S5);
}

}  // namespace SmState

}  // end namespace FppTest
