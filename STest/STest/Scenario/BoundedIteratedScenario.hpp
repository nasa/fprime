// ====================================================================== 
// \title  BoundedIteratedScenario.hpp
// \author bocchino
// \brief  Run an iterated scenario, bounding the number of iterations
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef STest_BoundedIteratedScenario_HPP
#define STest_BoundedIteratedScenario_HPP

#include "STest/Scenario/ConditionalIteratedScenario.hpp"

namespace STest {

  //! Run a scenario, bounding the number of iterations
  template<typename State> class BoundedIteratedScenario :
    public ConditionalIteratedScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      //! Construct a BoundedIteratedScenario
      BoundedIteratedScenario(
          const char *const name, //!< The name of the BoundedIteratedScenario
          IteratedScenario<State>& scenario, //!< The scenario to run
          const U32 bound //!< The bound
      ) :
        ConditionalIteratedScenario<State>(name, scenario),
        numIterations(0),
        bound(bound)
      {

      }

      //! Destroy a BoundedIteratedScenario
      virtual ~BoundedIteratedScenario(void) {

      }

    public:

      // ----------------------------------------------------------------------
      // ConditionalIteratedScenario implementation
      // ----------------------------------------------------------------------

      //! The virtual condition required by ConditionalIteratedScenario
      //! \return Whether the condition holds
      bool condition_ConditionalIteratedScenario(
          const State& state //!< The system state
      ) const {
        return this->numIterations < this->bound;
      }

      //! The virtual implementation of nextScenario required by ConditionalIteratedScenario
      void nextScenario_ConditionalIteratedScenario(
          const Scenario<State> *const nextScenario //!< The next scenario
      ) {
        if (nextScenario != NULL) {
          ++this->numIterations;
        }
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables 
      // ----------------------------------------------------------------------

      //! The number of iterations
      U32 numIterations;

      //! The bound on the number of iterations
      const U32 bound;

  };

}

#endif
