// ======================================================================
//
// \title  Polymorphism.hpp
// \author R. Bocchino
// \brief  Test class for polymorphic state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_Polymorphism_HPP
#define FppTest_State_Polymorphism_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/PolymorphismStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A hierarchical state machine with behavioral polymorphism
class Polymorphism final : public PolymorphismStateMachineBase {
  public:
    //! Constructor
    Polymorphism();

  public:
    //! Test initial transition
    void testInit();

    //! Test polymorphic transition in S2
    void testS2_poly();

    //! Test transition from S2 to S3
    void testS2_to_S3();

    //! Test polymorphic transition in S3
    void testS3_poly();
};

}  // namespace SmState

}  // end namespace FppTest

#endif
