// ======================================================================
//
// \title  InputPairU16U32.hpp
// \author R. Bocchino
// \brief  Test class for state machine with a pair of choice inputs (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_InputPairU16U32_HPP
#define FppTest_State_InputPairU16U32_HPP

#include "FppTest/state_machine/internal/choice/InputPairU16U32StateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A basic state machine with a pair of choice inputs
class InputPairU16U32 final : public InputPairU16U32StateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    InputPairU16U32();

  private:
    //! Implementation of action a
    void action_a(Signal signal,  //!< The signal
                  U32 value       //!< The value
                  ) final;

    //! Implementation of guard g
    bool guard_g(Signal signal,  //!< The signal
                 U32 value       //!< The value
    ) const final;

  public:
    //! Run the test with signal s1 and true guard
    void testS1True();

    //! Run the test with signal s1 and false guard
    void testS1False();

    //! Run the test with signal s2 and true guard
    void testS2True();

    //! Run the test with signal s2 and false guard
    void testS2False();

  private:
    //! The history associated with action a
    SmHarness::SignalValueHistory<Signal, U32, historySize> m_action_a_history;

    //! The guard g
    SmHarness::Guard<Signal, U32, historySize> m_guard_g;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
