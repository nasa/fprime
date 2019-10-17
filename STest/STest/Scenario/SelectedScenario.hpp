// ====================================================================== 
// \title  SelectedScenario.hpp
// \author bocchino
// \brief  Randomly select a scenario and run it
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_SelectedScenario_HPP
#define STest_SelectedScenario_HPP

#include <assert.h>
#include <string.h>

#include "STest/Random/Random.hpp"
#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/ScenarioArray.hpp"

namespace STest {

  //! Randomly select a scenario and run it
  template<typename State> class SelectedScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      //! Construct a SelectedScenario object
      SelectedScenario(
          const char *const name, //!< The name of the scenario
          Scenario<State>** scenarios, //!< The scenarios in the array
          const U32 size //!< The size of the array
      ) :
        Scenario<State>(name),
        scenarioArray(new ScenarioArray<State>(scenarios, size)),
        selectedScenario(NULL)
      {

      }

      //! Destroy a SelectedScenario object
      virtual ~SelectedScenario(void) {
        assert(this->scenarioArray != NULL);
        delete this->scenarioArray;
      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario(void) {
        this->selectedScenario = NULL;
        assert(this->scenarioArray != NULL);
        this->scenarioArray->reset();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or NULL if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State>* rule = NULL;
        if (this->selectedScenario == NULL) {
          rule = this->selectScenario(state);
        }
        else {
          rule = this->selectedScenario->nextRule(state);
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario(void) const {
        bool result = true;
        if (this->selectedScenario != NULL) {
          result = this->selectedScenario->isDone();
        }
        else {
          Scenario<State>* *const scenarios = 
            this->scenarioArray->getScenarios();
          assert(scenarios != NULL);
          for (U32 i = 0; i < scenarioArray->size; ++i) {
            assert(scenarios[i] != NULL);
            if (!scenarios[i]->isDone()) {
              result = false;
              break;
            }
          }
        }
        return result;
      }

    private:

      // ----------------------------------------------------------------------
      // Private helper methods
      // ----------------------------------------------------------------------

      //! Select a scenario and return a rule from it
      //! \return The rule
      Rule<State>* selectScenario(
          State& state //!< The system state
      ) {
        Rule<State>* rule = NULL;
        const U32 size = this->scenarioArray->size;
        bool seen[size];
        memset(seen, 0, sizeof(seen));
        U32 numSeen = 0;
        assert(this->scenarioArray != NULL);
        Scenario<State> **const scenarios = 
          this->scenarioArray->getScenarios();
        assert(scenarios != NULL);
        while (numSeen < size) {
          const U32 i = this->scenarioArray->getRandomIndex();
          if (seen[i]) {
            continue;
          }
          Scenario<State> *const scenario = scenarios[i];
          assert(scenario != NULL);
          rule = scenario->nextRule(state);
          if (rule != NULL) {
            this->selectedScenario = scenario;
            break;
          }
          seen[i] = true;
          ++numSeen;
        }
        return rule;
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables 
      // ----------------------------------------------------------------------

      //! ScenarioArray containing the scenarios to select
      ScenarioArray<State>* scenarioArray;

      //! The selected scenario
      Scenario<State>* selectedScenario;

  };

}

#endif
