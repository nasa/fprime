// ======================================================================
// \title  RuleScenario.hpp
// \author bocchino
// \brief  Apply a single rule once
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_RuleScenario_HPP
#define STest_RuleScenario_HPP

#include "STest/Scenario/Scenario.hpp"

namespace STest {

  //! Apply a single rule once
  template<typename State> class RuleScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct object RuleScenario
      RuleScenario(
          Rule<State>& rule //!< The rule
      ) :
        Scenario<State>(rule.name),
        rule(rule),
        done(false)
      {

      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        this->done = false;
      }

      //! the virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State> *rule = nullptr;
        if (!this->isDone() && this->rule.precondition(state)) {
          rule = &this->rule;
          this->done = true;
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        return this->done;
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The rule
      Rule<State>& rule;

      //! Whether the scenario is done
      bool done;

  };

}

#endif
