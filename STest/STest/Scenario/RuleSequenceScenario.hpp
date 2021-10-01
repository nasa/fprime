// ======================================================================
// \title  RuleSequenceScenario.hpp
// \author bocchino
// \brief  Apply a fixed sequence of rules
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_RuleSequenceScenario_HPP
#define STest_RuleSequenceScenario_HPP

#include <cassert>

#include "STest/Scenario/SequenceScenario.hpp"
#include "STest/Scenario/RuleScenario.hpp"

namespace STest {

  //! \brief  Apply a fixed sequence of rules
  template<typename State> class RuleSequenceScenario :
    public SequenceScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a RuleSequenceScenario from an array of rules
      RuleSequenceScenario(
          const char *const name, //!< The name of the scenario
          Rule<State>** rules, //!< The rules in the array
          const U32 size //!< The size of the array
      ) :
        SequenceScenario<State>(name, new Scenario<State>*[size], size)
      {
        assert(this->scenarioArray != nullptr);
        Scenario<State>* *const scenarios = this->scenarioArray->getScenarios();
        assert(scenarios != nullptr);
        for (U32 i = 0; i < size; ++i) {
          scenarios[i] = new RuleScenario<State>(*rules[i]);
        }
      }

      //! Destroy object RuleSequenceScenario
      ~RuleSequenceScenario() {
        assert(this->scenarioArray != nullptr);
        Scenario<State>* *const scenarios = this->scenarioArray->getScenarios();
        assert(scenarios != nullptr);
        for (U32 i = 0; i < this->scenarioArray->size; ++i) {
          assert(scenarios[i] != nullptr);
          delete scenarios[i];
        }
        delete scenarios;
      }

  };

}

#endif
