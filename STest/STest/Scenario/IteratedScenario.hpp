// ====================================================================== 
// \title  IteratedScenario.hpp
// \author bocchino
// \brief  Iterate over a collection of scenarios
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_IteratedScenario_HPP
#define STest_IteratedScenario_HPP

#include <assert.h>

#include "STest/Scenario/Scenario.hpp"

namespace STest {

  //! Iterate over a collection of scenarios
  template<typename State> class IteratedScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      //! Construct an IteratedScenario
      IteratedScenario(
          const char *const name //!< The name of the scenario
      ) :
        Scenario<State>(name),
        currentScenario(NULL)
      {
        
      }

      //! Destroy an IteratedScenario
      virtual ~IteratedScenario(void) {

      }

    public:

      // ----------------------------------------------------------------------
      // Public instance methods 
      // ----------------------------------------------------------------------

      //! Return the next scenario to run
      //! \return The next scenario, assuming isDone() is false, or NULL if none
      Scenario<State>* nextScenario(
          State& state //!< The system state
      ) {
        Scenario<State> *scenario = NULL;
        if (!this->isDone()) {
          scenario = this->nextScenario_IteratedScenario(state);
        }
        if (scenario != NULL) {
          scenario->reset();
        }
        return scenario;
      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario(void) {
        this->currentScenario = NULL;
        this->reset_IteratedScenario();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or NULL if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State>* rule = NULL;
        if (this->currentScenario == NULL) {
          this->currentScenario = this->nextScenario(state);
        }
        if (this->currentScenario != NULL) {
          rule = this->currentScenario->nextRule(state);
        }
        while (
            this->currentScenario != NULL and 
            this->currentScenario->isDone() and
            rule == NULL
        ) {
          this->currentScenario = this->nextScenario(state);
          if (this->currentScenario != NULL) {
            rule = this->currentScenario->nextRule(state);
          }
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario(void) const {
        return this->isDone_IteratedScenario();
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected virtual methods 
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by IteratedScenario
      virtual void reset_IteratedScenario(void) = 0;

      //! The virtual implementation of nextScenario required by IteratedScenario
      //! \return The next scenario, assuming isDone() is false, or NULL if none
      virtual Scenario<State>* nextScenario_IteratedScenario(
          State& state //!< The system state
      ) = 0;

      //! The virtual implementation of isDone required by IteratedScenario
      //! \return Whether the scenario is done
      virtual bool isDone_IteratedScenario(void) const = 0;

    private:

      // ----------------------------------------------------------------------
      // Private member variables 
      // ----------------------------------------------------------------------

      //! The current scenario in the iteration
      Scenario<State>* currentScenario;

  };

}

#endif
