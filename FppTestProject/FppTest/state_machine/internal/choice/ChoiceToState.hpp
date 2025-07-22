// ======================================================================
//
// \title  ChoiceToState.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a choice-to-state transition (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_ChoiceToState_HPP
#define FppTest_State_ChoiceToState_HPP

#include "FppTest/state_machine/internal/choice/ChoiceToStateStateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A state machine with a choice-to-state transition
class ChoiceToState final : public ChoiceToStateStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Action IDs
    enum class ActionId {
        EXIT_S1,
        A,
        ENTER_S2,
        ENTER_S3,
    };

  public:
    //! Constructor
    ChoiceToState();

  private:
    //! Implementation of action exitS1
    void action_exitS1(Signal signal  //!< The signal
                       ) final;

    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Implementation of action enterS2
    void action_enterS2(Signal signal  //!< The signal
                        ) final;

    //! Implementation of action enterS3
    void action_enterS3(Signal signal  //!< The signal
                        ) final;

    //! Implementation of guard g
    bool guard_g(Signal signal  //!< The signal
    ) const final;

  public:
    //! Run the test with the true guard
    void testTrue();

    //! Run the test with the false guard
    void testFalse();

  private:
    //! The action history
    SmHarness::SignalValueHistory<Signal, ActionId, historySize> m_actionHistory;

    //! The guard g
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
