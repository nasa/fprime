// ====================================================================== 
// \title  BoundedScenario.hpp
// \author bocchino
// \brief  Run a scenario, bounding the number of steps
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_BoundedScenario_HPP
#define STest_BoundedScenario_HPP

#include "STest/Scenario/ConditionalScenario.hpp"

namespace STest {

  //! Run a scenario, bounding the number of steps
  template<typename State> class BoundedScenario :
    public ConditionalScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      //! Construct a BoundedScenario object
      BoundedScenario(
          const char *const name, //!< The name of the bounded scenario
          Scenario<State>& scenario, //!< The scenario to run
          const U32 bound //!< The bound
      ) :
        ConditionalScenario<State>(name, scenario),
        numSteps(0),
        bound(bound)
      {

      }

      //! Destroy a BoundedScenario object
      ~BoundedScenario(void) {

      }

    public:

      // ----------------------------------------------------------------------
      // ConditionalScenario implementation
      // ----------------------------------------------------------------------

      //! The virtual condition required by CondtiionalScenario
      //! \return Whether the condition holds
      bool condition_ConditionalScenario(
          const State& state //!< The system state
      ) const {
        return this->numSteps < this->bound;
      }

      //! The virtual implementation of nextRule required by ConditionalScenario
      void nextRule_ConditionalScenario(
          const Rule<State> *const nextRule //!< The next rule
      ) {
        if (nextRule != NULL) {
          ++this->numSteps;
        }
      }

      //! The virtual implementation of reset required by ConditionalScenario
      void reset_ConditionalScenario(void) {
        this->numSteps = 0;
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables 
      // ----------------------------------------------------------------------

      //! The number of steps
      U32 numSteps;

      //! The bound on the number of steps
      const U32 bound;

  };

}

#endif
