// ======================================================================
//
// \title  JunctionToState.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a junction-to-state transition (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_JunctionToState_HPP
#define FppTest_State_JunctionToState_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/choice/JunctionToStateStateMachineAc.hpp"

namespace FppTest {

namespace SmJunction {

//! A state machine with a junction-to-state transition
class JunctionToState final : public JunctionToStateStateMachineBase {
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
    JunctionToState();

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

}  // namespace SmJunction

}  // end namespace FppTest

#endif
