// ======================================================================
// \title  Polymorphism.cpp
// \author bocchino
// \brief  cpp file for Polymorphism component implementation class
// ======================================================================

#include <gtest/gtest.h>

#include "FppTest/state_machine/internal_instance/state/Polymorphism.hpp"

namespace FppTest {

namespace SmInstanceState {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Polymorphism::Polymorphism(const char* const compName) : PolymorphismComponentBase(compName) {}

Polymorphism::~Polymorphism() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Polymorphism::testInit() {
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S2);
}

void Polymorphism::testS2_poly() {
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S2);
    this->smStatePolymorphism_sendSignal_poly();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S4);
}

void Polymorphism::testS2_to_S3() {
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S2);
    this->smStatePolymorphism_sendSignal_S2_to_S3();
    const auto status = this->doDispatch();
    ASSERT_EQ(status, MSG_DISPATCH_OK);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S3);
}

void Polymorphism::testS3_poly() {
    this->init(queueDepth, instanceId);
    ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S2);
    {
        this->smStatePolymorphism_sendSignal_S2_to_S3();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S1_S3);
    }
    {
        this->smStatePolymorphism_sendSignal_poly();
        const auto status = this->doDispatch();
        ASSERT_EQ(status, MSG_DISPATCH_OK);
        ASSERT_EQ(this->smStatePolymorphism_getState(), SmState_Polymorphism::State::S5);
    }
}

}  // namespace SmInstanceState

}  // namespace FppTest
