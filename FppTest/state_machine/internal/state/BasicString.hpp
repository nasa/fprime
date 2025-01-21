// ======================================================================
//
// \title  BasicString.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with string actions (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_BasicString_HPP
#define FppTest_State_BasicString_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/BasicStringStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A basic state machine with string actions
class BasicString final : public BasicStringStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    BasicString();

  private:
    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Implementation of action b
    void action_b(Signal signal,               //!< The signal
                  const Fw::StringBase& value  //!< The value
                  ) final;

  public:
    //! Run the test
    void test();

  private:
    //! The history associated with action a
    SmHarness::History<Signal, historySize> m_action_a_history;

    //! The history associated with action b
    SmHarness::SignalValueHistory<Signal, Fw::String, historySize> m_action_b_history;
};

}  // namespace SmState

}  // end namespace FppTest

#endif
