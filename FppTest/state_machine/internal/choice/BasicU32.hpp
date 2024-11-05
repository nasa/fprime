// ======================================================================
//
// \title  BasicU32.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with a U32 choice (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_BasicU32_HPP
#define FppTest_State_BasicU32_HPP

#include "FppTest/state_machine/internal/choice/BasicU32StateMachineAc.hpp"
#include "FppTest/state_machine/internal/harness/Harness.hpp"

namespace FppTest {

namespace SmChoice {

//! A basic state machine with a U32 choice
class BasicU32 final : public BasicU32StateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    BasicU32();

  private:
    //! Implementation of action a
    void action_a(Signal signal,  //!< The signal
                  U32 value       //!< The value
                  ) final;

    //! Implementation of action b
    void action_b(Signal signal  //!< The signal
                  ) final;

    //! Implementation of guard g
    bool guard_g(Signal signal,  //!< The signal
                 U32 value       //!< The value
    ) const final;

  public:
    //! Run the test with the true guard
    void testTrue();

    //! Run the test with the false guard
    void testFalse();

  private:
    //! The history associated with action a
    SmHarness::SignalValueHistory<Signal, U32, historySize> m_action_a_history;

    //! The history associated with action b
    SmHarness::History<Signal, historySize> m_action_b_history;

    //! The guard g
    SmHarness::Guard<Signal, U32, historySize> m_guard_g;
};

}  // namespace SmChoice

}  // end namespace FppTest

#endif
