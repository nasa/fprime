// ======================================================================
//
// \title  Sequence.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with a choice sequence (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_Sequence_HPP
#define FppTest_State_Sequence_HPP

#include "FppTest/state_machine/internal/choice/SequenceStateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A basic state machine with a choice sequence
class Sequence final : public SequenceStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    Sequence();

  private:
    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Implementation of action b
    void action_b(Signal signal  //!< The signal
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

    //! Run the test with g1 true and g2 true
    void testG1FalseG2True();

    //! Run the test with g1 true and g2 false
    void testG1FalseG2False();

  private:
    //! The history associated with action a
    SmHarness::History<Signal, historySize> m_action_a_history;

    //! The history associated with action b
    SmHarness::History<Signal, historySize> m_action_b_history;

    //! The guard g1
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g1;

    //! The guard g2
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g2;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
