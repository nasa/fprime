// ======================================================================
//
// \title  BasicGuardTestArray.hpp
// \author R. Bocchino
// \brief  Test class for basic state machine with TestArray guard (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_State_BasicGuardTestArray_HPP
#define FppTest_State_BasicGuardTestArray_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/state/BasicGuardTestArrayStateMachineAc.hpp"

namespace FppTest {

namespace SmState {

//! A basic state machine with a TestArray guard
class BasicGuardTestArray final : public BasicGuardTestArrayStateMachineBase {
  public:
    //! The history size
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    BasicGuardTestArray();

  private:
    //! Implementation of action a
    void action_a(Signal signal,                     //!< The signal
                  const SmHarness::TestArray& value  //!< The value
                  ) final;

    //! Implementation of guard g
    bool guard_g(Signal signal,                     //!< The signal
                 const SmHarness::TestArray& value  //!< The value
    ) const;

  public:
    //! Test with true guard
    void testTrue();

    //! Test with false guard
    void testFalse();

  private:
    //! The history associated with action a
    SmHarness::SignalValueHistory<Signal, SmHarness::TestArray, historySize> m_action_a_history;

    //! The guard g
    SmHarness::Guard<Signal, SmHarness::TestArray, historySize> m_guard_g;
};

}  // namespace SmState

}  // end namespace FppTest

#endif
