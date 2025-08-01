// ======================================================================
//
// \title  StateToChild.hpp
// \author R. Bocchino
// \brief  Test class for state-to-self state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_StateToChild_HPP
#define FppTest_State_StateToChild_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/StateToChildStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A state machine for testing state-to-child transitions with hierarchy
class StateToChild final : public StateToChildStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Action IDs
    enum class ActionId { EXIT_S2, EXIT_S3, A, ENTER_S2, ENTER_S3 };

  public:
    //! Constructor
    StateToChild();

  private:
    //! Exit S2
    void action_exitS2(Signal signal  //!< The signal
                       ) final;

    //! Exit S3
    void action_exitS3(Signal signal  //!< The signal
                       ) final;

    //! Action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Enter S2
    void action_enterS2(Signal signal  //!< The signal
                        ) final;

    //! Enter S3
    void action_enterS3(Signal signal  //!< The signal
                        ) final;

  public:
    //! Test initial transition
    void testInit();

    //! Test transition from S2 to S2
    void testS2_to_S2();

    //! Test transition from S2 to S3
    void testS2_to_S3();

    //! Test transition from S3 to S2
    void testS3_to_S2();

  private:
    //! The action history
    SmHarness::SignalValueHistory<Signal, ActionId, historySize> m_actionHistory;
};

}  // namespace SmState

}  // end namespace FppTest

#endif
