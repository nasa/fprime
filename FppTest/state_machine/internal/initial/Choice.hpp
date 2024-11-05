// ======================================================================
//
// \title  Choice.hpp
// \author R. Bocchino
// \brief  Test class for choice state machine (header)
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_Choice_HPP
#define FppTest_Choice_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal/initial/ChoiceStateMachineAc.hpp"

namespace FppTest {

namespace SmInitial {

//! Choice state machine
class Choice final : public ChoiceStateMachineBase {
  public:
    static constexpr FwSizeType historySize = 10;

  public:
    //! Constructor
    Choice();

  private:
    //! Implementation of action a
    void action_a(Signal signal  //!< The signal
                  ) final;

    //! Implementation of guard g
    bool guard_g(Signal signal  //!< The signal
    ) const;

  public:
    //! Test with true guard
    void testTrue();

    //! Test with false guard
    void testFalse();

  private:
    //! Helper function for checking actions and guards
    void checkActionsAndGuards(FwSizeType expectedActionSize,  //!< The expected action size
                               FwSizeType expectedGuardSize    //!< The expected guard size
    );

  private:
    //! The history associated with action a
    SmHarness::History<Signal, historySize> m_action_a_history;

    //! The guard g
    SmHarness::NoArgGuard<Signal, historySize> m_guard_g;
};

}  // namespace SmInitial

}  // end namespace FppTest

#endif
