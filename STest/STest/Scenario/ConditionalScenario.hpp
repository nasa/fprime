// ======================================================================
// \title  ConditionalScenario.hpp
// \author bocchino
// \brief  Run a scenario while a condition holds
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_ConditionalScenario_HPP
#define STest_ConditionalScenario_HPP

#include "STest/Scenario/Scenario.hpp"

namespace STest {

  //! Run a scenario while a condition holds
  template<typename State> class ConditionalScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a ConditionalScenario
      ConditionalScenario(
          const char *const name, //!< The name of the ConditionalScenario
          Scenario<State>& scenario //!< The scenario to run
      ) :
        Scenario<State>(name),
        scenario(scenario)
      {

      }

      //! Destroy object ConditionalScenario
      virtual ~ConditionalScenario() {

      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        this->scenario.reset();
        this->reset_ConditionalScenario();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State>* rule = nullptr;
        if (this->condition_ConditionalScenario(state)) {
          rule = this->scenario.nextRule(state);
        }
        this->nextRule_ConditionalScenario(rule);
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        return this->scenario.isDone();
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected virtual methods
      // ----------------------------------------------------------------------

      //! The virtual condition required by ConditionalScenario
      //! \return Whether the condition holds
      virtual bool condition_ConditionalScenario(
          const State& state //!< The system state
      ) const = 0;

      //! The virtual implementation of nextRule required by ConditionalScenario
      virtual void nextRule_ConditionalScenario(
          const Rule<State> *const nextRule //!< The next rule
      ) = 0;

      //! The virtual implementation of reset required by ConditionalScenario
      virtual void reset_ConditionalScenario() = 0;

    protected:

      // ----------------------------------------------------------------------
      // Protected member variables
      // ----------------------------------------------------------------------

      //! The scenario to run
      Scenario<State>& scenario;

  };

}

#endif
