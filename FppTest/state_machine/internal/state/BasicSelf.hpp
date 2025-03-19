// ======================================================================
//
// \title  BasicSelf.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with self transition (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_BasicSelf_HPP
#define FppTest_State_BasicSelf_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/BasicSelfStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A basic state machine with a self transition
class BasicSelf final : public BasicSelfStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    BasicSelf();

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

}  // namespace SmState

}  // end namespace FppTest

#endif
