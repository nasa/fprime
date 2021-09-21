// ======================================================================
// \title  ScenarioArray.hpp
// \author bocchino
// \brief  An array of scenarios
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_ScenarioArray_HPP
#define STest_ScenarioArray_HPP

#include <cassert>

#include "STest/Random/Random.hpp"

namespace STest {

  //! An array of scenarios
  template<typename State>class ScenarioArray {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a ScenarioArray object
      ScenarioArray (
          Scenario<State>** scenarios, //!< The scenarios in the array
          const U32 size //!< The number of scenarios in the array
      ) :
        size(size),
        scenarios(scenarios),
        sequenceIndex(0)
      {

      }

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Get a random index into the array
      //! \return The index
      U32 getRandomIndex() const {
        const U32 index = Random::startLength(0, this->size);
        assert(index < this->size);
        return index;
      }

      //! Reset the sequence index and reset all child scenarios
      void reset() {
        this->sequenceIndex = 0;
        for (U32 i = 0; i < this->size; ++i) {
          this->scenarios[i]->reset();
        }
      }

      //! Return the next scenario in the sequence
      Scenario<State>* nextScenario() {
        Scenario<State>* scenario = nullptr;
        if (this->sequenceIndex < this->size) {
          scenario = this->scenarios[this->sequenceIndex];
          ++this->sequenceIndex;
        }
        if (scenario != nullptr) {
          scenario->reset();
        }
        return scenario;
      }

      //! Get the scenarios
      Scenario<State>** getScenarios() const {
        assert(this->scenarios != nullptr);
        return this->scenarios;
      }

    public:

      // ----------------------------------------------------------------------
      // Public member variables
      // ----------------------------------------------------------------------

      //! The number of scenarios in the array
      const U32 size;

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The scenarios in the array
      Scenario<State>** scenarios;

      //! The sequence index
      U32 sequenceIndex;

  };

}

#endif
