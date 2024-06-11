// ======================================================================
// \title  RandomlyBoundedScenario.hpp
// \author bocchino
// \brief  Run a scenario, applying a random bound on the number of steps
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_RandomlyBoundedScenario_HPP
#define STest_RandomlyBoundedScenario_HPP

#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"

namespace STest {

  //! Run a scenario, applying a random bound on the number of steps
  template<typename State> class RandomlyBoundedScenario :
    public Scenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a RandomlyBoundedScenario
      RandomlyBoundedScenario(
          const char *const name, //!< The name of the bounded scenario
          Scenario<State>& scenario, //!< The scenario to run
          const U32 start, //!< The start value of the random range
          const U32 length //!< The number of values in the random range, including the start value
      ) :
        Scenario<State>(name),
        scenario(scenario),
        start(start),
        length(length),
        boundedScenario(nullptr)
      {

      }

      //! Destroy object RandomlyBoundedScenario
      virtual ~RandomlyBoundedScenario() {
        if (this->boundedScenario != nullptr) {
          delete this->boundedScenario;
        }
      }

    private:

      // ----------------------------------------------------------------------
      // Scenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      void reset_Scenario() {
        if (this->boundedScenario != nullptr) {
          delete this->boundedScenario;
        }
        const U32 bound = Random::startLength(this->start, this->length);
        this->boundedScenario = new BoundedScenario<State>(
            this->name,
            this->scenario,
            bound
        );
        assert(this->boundedScenario != nullptr);
        this->boundedScenario->reset();
      }

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) {
        assert(this->boundedScenario != nullptr);
        return this->boundedScenario->nextRule(state);
      }

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      bool isDone_Scenario() const {
        assert(this->boundedScenario != nullptr);
        return this->boundedScenario->isDone();
      }

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The scenario to run
      Scenario<State>& scenario;

      //! The start value of the random range
      const U32 start;

      //! The number of values in the random range, including the start value
      const U32 length;

      //! The underlying bounded scenario
      BoundedScenario<State>* boundedScenario;

  };

}

#endif
