// ======================================================================
//
// \title  Internal.hpp
// \author R. Bocchino
// \brief  Test class for internal state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_Internal_HPP
#define FppTest_State_Internal_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/InternalStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A hierarchical state machine with an internal transition
class Internal final : public InternalStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    Internal();

  private:
    //! Action a
    void action_a(Signal signal  //!< The signal
                  ) final;

  public:
    //! Test initial transition
    void testInit();

    //! Test internal transition in S2
    void testS2_internal();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test internal transition in S3
    void testS3_internal();

  private:
    //! The history of action a
    SmHarness::History<Signal, historySize> m_action_a_history;
};

}  // namespace SmState

}  // end namespace FppTest

#endif
