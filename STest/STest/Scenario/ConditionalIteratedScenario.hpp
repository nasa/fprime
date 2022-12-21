// ======================================================================
// \title  ConditionalIteratedScenario.hpp
// \author bocchino
// \brief  Iterate a scenario while a condition holds
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_ConditionalIteratedScenario_HPP
#define STest_ConditionalIteratedScenario_HPP

#include "STest/Scenario/IteratedScenario.hpp"

namespace STest {

  //! Iterate a scenario while a condition holds
  template<typename State> class ConditionalIteratedScenario :
    public IteratedScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a ConditionalIteratedScenario
      ConditionalIteratedScenario(
          const char *const name, //!< The name of the ConditionalIteratedScenario
          IteratedScenario<State>& scenario //!< The scenario to run
      ) :
        IteratedScenario<State>(name),
        scenario(scenario),
        done(false)
      {

      }

      //! Destroy object ConditionalIteratedScenario
      virtual ~ConditionalIteratedScenario() {

      }

    protected:

      // ----------------------------------------------------------------------
      // IteratedScenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by IteratedScenario
      void reset_IteratedScenario() {
        this->scenario.reset();
        this->done = this->scenario.isDone();
      }

      //! The virtual implementation of nextScenario required by IteratedScenario
      //! \return The next scenario, assuming isDone() is false, or NULL if none
      Scenario<State>* nextScenario_IteratedScenario(
          State& state //!< The system state
      ) {
        Scenario<State>* scenario = nullptr;
        if (!this->condition_ConditionalIteratedScenario(state)) {
          this->done = true;
        }
        if (!this->isDone()) {
          scenario = this->scenario.nextScenario(state);
          this->done = this->scenario.isDone();
        }
        this->nextScenario_ConditionalIteratedScenario(scenario);
        return scenario;
      }

      //! The virtual implementation of isDone required by IteratedScenario
      //! \return Whether the scenario is done
      bool isDone_IteratedScenario() const {
        return this->done;
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected virtual methods
      // ----------------------------------------------------------------------

      //! The virtual condition required by ConditionalIteratedScenario
      //! \return Whether the condition holds
      virtual bool condition_ConditionalIteratedScenario(
          const State& state //!< The system state
      ) const = 0;

      //! The virtual implementation of nextScenario required by ConditionalIteratedScenario
      virtual void nextScenario_ConditionalIteratedScenario(
          const Scenario<State> *const nextScenario //!< The scenario being returned
      ) = 0;

    protected:

      // ----------------------------------------------------------------------
      // Protected member variables
      // ----------------------------------------------------------------------

      //! The scenario to run
      IteratedScenario<State>& scenario;

      //! Whether the iterated scenario is done
      bool done;

  };

}

#endif
