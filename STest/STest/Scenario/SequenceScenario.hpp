// ====================================================================== 
// \title  SequenceScenario.hpp
// \author bocchino
// \brief  A sequence of scenarios
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_SequenceScenario_HPP
#define STest_SequenceScenario_HPP

#include <assert.h>

#include "STest/Scenario/IteratedScenario.hpp"
#include "STest/Scenario/ScenarioArray.hpp"

namespace STest {


  //! A sequence of scenarios
  template<typename State> class SequenceScenario :
    public IteratedScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      //! Construct a SequenceScenario from an array of scenarios
      SequenceScenario(
          const char *const name, //!< The name of the scenario
          Scenario<State>** scenarios, //!< The scenarios in the array
          const U32 size //!< The size of the array
      ) :
        IteratedScenario<State>(name),
        scenarioArray(new ScenarioArray<State>(scenarios, size)),
        done(false)
      {
        
      }


      //! Destroy object SequenceScenario
      virtual ~SequenceScenario(void) {
        delete this->scenarioArray;
      }

    protected:

      // ----------------------------------------------------------------------
      // IteratedScenario implemntation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by IteratedScenario
      void reset_IteratedScenario(void) {
        this->scenarioArray->reset();
        this->done = false;
      }

      //! The virtual implementation of nextScenario required by IteratedScenario
      //! \return The next scenario, assuming isDone() is false, or NULL if none
      Scenario<State>* nextScenario_IteratedScenario(
          State& state //!< The system state
      ) {
        Scenario<State> *scenario = NULL;
        if (!this->done) {
          assert(this->scenarioArray != NULL);
          scenario = this->scenarioArray->nextScenario();
        }
        if (!this->done and scenario == NULL) {
          this->done = true;
        }
        return scenario;
      }

      //! The virtual implementation of isDone required by IteratedScenario
      //! \return Whether the scenario is done
      bool isDone_IteratedScenario(void) const {
        return this->done;
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected member variables 
      // ----------------------------------------------------------------------

      //! The scenario array
      ScenarioArray<State>* scenarioArray;

      //! Whether the sequence scenario is done
      bool done;

  };

}

#endif
