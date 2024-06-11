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

#include <cassert>
#include <cstring>

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
        selectedScenario(nullptr),
        seen(new bool[size])
      {

      }

      //! Destroy a SelectedScenario object
      virtual ~SelectedScenario() {
        if (this->scenarioArray != nullptr) {
          delete this->scenarioArray;
        }
        if (this->seen != nullptr) {
          delete[] this->seen;
        }
      }

    public:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        this->selectedScenario = nullptr;
        assert(this->scenarioArray != nullptr);
        this->scenarioArray->reset();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        Rule<State>* rule = nullptr;
        if (this->selectedScenario == nullptr) {
          rule = this->selectScenario(state);
        }
        else {
          rule = this->selectedScenario->nextRule(state);
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        bool result = true;
        if (this->selectedScenario != nullptr) {
          result = this->selectedScenario->isDone();
        }
        else {
          Scenario<State>* *const scenarios =
            this->scenarioArray->getScenarios();
          assert(scenarios != nullptr);
          for (U32 i = 0; i < scenarioArray->size; ++i) {
            assert(scenarios[i] != nullptr);
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
        Rule<State>* rule = nullptr;
        const U32 size = this->scenarioArray->size;
        memset(this->seen, 0, size * sizeof(bool));
        U32 numSeen = 0;
        assert(this->scenarioArray != nullptr);
        Scenario<State> **const scenarios =
          this->scenarioArray->getScenarios();
        assert(scenarios != nullptr);
        while (numSeen < size) {
          const U32 i = this->scenarioArray->getRandomIndex();
          if (this->seen[i]) {
            continue;
          }
          Scenario<State> *const scenario = scenarios[i];
          assert(scenario != nullptr);
          rule = scenario->nextRule(state);
          if (rule != nullptr) {
            this->selectedScenario = scenario;
            break;
          }
          this->seen[i] = true;
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

      //! An array to store the scenarios seen
      bool* seen;

  };

}

#endif
