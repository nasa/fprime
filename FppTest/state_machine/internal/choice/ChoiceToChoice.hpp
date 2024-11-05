// ======================================================================
//
// \title  ChoiceToChoice.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a choice-to-choice transition (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United Choices Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_Choice_ChoiceToChoice_HPP
#define FppTest_Choice_ChoiceToChoice_HPP

#include "FppTest/state_machine/internal/choice/ChoiceToChoiceStateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A state machine with a choice-to-choice transition
class ChoiceToChoice final : public ChoiceToChoiceStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Action IDs
    enum class ActionId {
        EXIT_S1,
        A,
        ENTER_S2,
    };

  public:
    //! Constructor
    ChoiceToChoice();

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

    //! Implementation of guard g1
    bool guard_g1(Signal signal  //!< The signal
    ) const final;

    //! Implementation of guard g2
    bool guard_g2(Signal signal  //!< The signal
    ) const final;

  public:
    //! Run the test with g1 true
    void testG1True();

    //! Run the test with g1 false and g2 true
    void testG1FalseG2True();

    //! Run the test with g1 false and g2 false
    void testG1FalseG2False();

  private:
    //! The action history
    SmHarness::SignalValueHistory<Signal, ActionId, historySize> m_actionHistory;

    //! The guard g1
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g1;

    //! The guard g2
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g2;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
