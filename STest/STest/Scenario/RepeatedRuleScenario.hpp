// ======================================================================
// \title  RepeatedRuleScenario.hpp
// \author bocchino
// \brief  Repeatedly apply a rule
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_RepeatedRuleScenario_HPP
#define STest_RepeatedRuleScenario_HPP

#include "STest/Scenario/Scenario.hpp"

namespace STest {

  //! Repeatedly apply a rule
  template<typename State> class RepeatedRuleScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct object RepeatedRuleScenario
      RepeatedRuleScenario(
          Rule<State>& rule //!< The rule
      ) :
        Scenario<State>(rule.getName()),
        rule(rule)
      {

      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        // Do nothing
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State> *rule = nullptr;
        if (this->rule.precondition(state)) {
          rule = &this->rule;
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        return false;
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The rule
      Rule<State>& rule;

  };

}

#endif
