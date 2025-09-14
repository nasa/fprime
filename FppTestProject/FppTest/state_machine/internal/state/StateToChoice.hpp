// ======================================================================
//
// \title  StateToChoice.hpp
// \author R. Bocchino
// \brief  Test class for state-to-choice state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_StateToChoice_HPP
#define FppTest_State_StateToChoice_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/StateToChoiceStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A state machine for testing state-to-choice transitions with hierarchy
class StateToChoice final : public StateToChoiceStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Action IDs
    enum class ActionId { EXIT_S1, EXIT_S2, EXIT_S3, A, ENTER_S1, ENTER_S2, ENTER_S3, ENTER_S4 };

  public:
    //! Constructor
    StateToChoice();

  private:
    //! Exit S1
    void action_exitS1(Signal signal  //!< The signal
                       ) final;

    //! Exit S2
    void action_exitS2(Signal signal  //!< The signal
                       ) final;

    //! Exit S3
    void action_exitS3(Signal signal  //!< The signal
                       ) final;

    //! Action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Enter S1
    void action_enterS1(Signal signal  //!< The signal
                        ) final;

    //! Enter S2
    void action_enterS2(Signal signal  //!< The signal
                        ) final;

    //! Enter S3
    void action_enterS3(Signal signal  //!< The signal
                        ) final;

    //! Enter S4
    void action_enterS4(Signal signal  //!< The signal
                        ) final;

    //! Guard g
    bool guard_g(Signal signal  //!< The signal
    ) const final;

  public:
    //! Test initial transition
    void testInit();

    //! Test transition S2 to C
    void testS2_to_C();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test transition S2 to S4
    void testS2_to_S4();

    //! Test transition S3 to C
    void testS3_to_C();

    //! Test transition S3 to S4
    void testS3_to_S4();

  private:
    //! The action history
    SmHarness::SignalValueHistory<Signal, ActionId, historySize> m_actionHistory;

    //! The guard g
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g;
};

}  // namespace SmState

}  // end namespace FppTest

#endif
