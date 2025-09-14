// ======================================================================
//
// \title  Nested.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_Nested_HPP
#define FppTest_Nested_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/initial/NestedStateMachineAc.hpp"

namespace FppTest {

namespace SmInitial {

//! Nested state machine
class Nested final : public NestedStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    Nested();

  private:
    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

  public:
    //! Run the test
    void test();

  private:
    //! The history associated with action a
    SmHarness::History<Signal, historySize> m_action_a_history;
};

}  // namespace SmInitial

}  // end namespace FppTest

#endif
