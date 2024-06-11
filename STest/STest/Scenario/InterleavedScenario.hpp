// ======================================================================
// \title  InterleavedScenario.hpp
// \author bocchino
// \brief  Randomly interleave several scenarios
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_InterleavedScenario_HPP
#define STest_InterleavedScenario_HPP

#include <cassert>
#include <cstring>

#include "STest/Scenario/Scenario.hpp"
#include "STest/Scenario/ScenarioArray.hpp"

namespace STest {

  //! Randomly interleave several scenarios
  template<typename State> class InterleavedScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct an InterleavedScenario object
      InterleavedScenario(
          const char *const name, //!< The name of the scenario
          Scenario<State>** scenarios, //!< An array containing the scenarios to interleave
          const U32 size //!< The size of the array
      ) :
        Scenario<State>(name),
        scenarioArray(new ScenarioArray<State>(scenarios, size)),
        seen(new bool[size])
      {

      }

      //! Destroy an InterleavedScenario object
      ~InterleavedScenario() {
        if (this->scenarioArray != nullptr) {
          delete this->scenarioArray;
        }
        if (this->seen != nullptr) {
          delete[] this->seen;
        }
      }

    protected:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        assert(this->scenarioArray != nullptr);
        this->scenarioArray->reset();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        assert(this->scenarioArray != nullptr);
        Rule<State>* rule = nullptr;
        memset(this->seen, 0, this->scenarioArray->size * sizeof(bool));
        U32 numSeen = 0;
        Scenario<State>* *const scenarios =
          this->scenarioArray->getScenarios();
        U32 numIterations = 0;
        const U32 maxIterations = 0xFFFFFFFFU;
        while (numSeen < this->scenarioArray->size) {
          assert(numIterations < maxIterations);
          ++numIterations;
          const U32 i = this->scenarioArray->getRandomIndex();
          if (this->seen[i]) {
            continue;
          }
          rule = scenarios[i]->nextRule(state);
          if (rule != nullptr) {
            break;
          }
          this->seen[i] = true;
          ++numSeen;
        }
        return rule;
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        bool result = true;
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
        return result;
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected member variables
      // ----------------------------------------------------------------------

      //! The scenarios to interleave
      ScenarioArray<State>* scenarioArray;

      //! An array to store the scenarios seen
      bool* seen;

  };

}

#endif
