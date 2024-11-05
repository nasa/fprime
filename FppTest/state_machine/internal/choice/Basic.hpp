// ======================================================================
//
// \title  Basic.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with a choice (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_Basic_HPP
#define FppTest_State_Basic_HPP

#include "FppTest/state_machine/internal/choice/BasicStateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A basic state machine with a choice
class Basic final : public BasicStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    Basic();

  private:
    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Implementation of action b
    void action_b(Signal signal  //!< The signal
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
    //! The history associated with action a
    SmHarness::History<Signal, historySize> m_action_a_history;

    //! The history associated with action b of Basic
    SmHarness::History<Signal, historySize> m_action_b_history;

    //! The guard g
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
