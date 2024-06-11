// ======================================================================
// \title  RepeatedScenario.hpp
// \author bocchino
// \brief  Repeat a scenario
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_RepeatedScenario_HPP
#define STest_RepeatedScenario_HPP

#include "STest/Scenario/IteratedScenario.hpp"

namespace STest {

  //! \brief  Repeat a scenario
  template<typename State> class RepeatedScenario :
    public IteratedScenario<State>
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a RepeatedScenario
      RepeatedScenario(
          const char *const name, //!< The name of the scenario
          Scenario<State>& scenario //!< The scenario to repeat
      ) :
        IteratedScenario<State>(name),
        scenario(scenario),
        done(false)
      {

      }

    public:

      // ----------------------------------------------------------------------
      // IteratedScenario implementation
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by IteratedScenario
      void reset_IteratedScenario() {
        this->scenario.reset();
        this->done = scenario.isDone();
      }

      //! The virtual implementation of nextScenario required by IteratedScenario
      //! \return The next scenario, assuming isDone() is false, or nullptr if none
      Scenario<State>* nextScenario_IteratedScenario(
          State& state //!< The system state
      ) {
        return &this->scenario;
      }

      //! The virtual implementation of isDone required by IteratedScenario
      //! \return Whether the scenario is done
      bool isDone_IteratedScenario() const {
        return this->done;
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected member variables
      // ----------------------------------------------------------------------

      //! The scenario to repeat
      Scenario<State>& scenario;

      //! Whether the iterated scenario is done
      bool done;

  };

}

#endif
