// ======================================================================
// \title  Scenario.hpp
// \author bocchino
// \brief  A test scenario
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_Scenario_HPP
#define STest_Scenario_HPP

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "STest/Random/Random.hpp"
#include "STest/Rule/Rule.hpp"

namespace STest {

  //! A test scenario
  template<typename State> class Scenario {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct a Scenario object
      Scenario(
          const char *const name //!< The name of the scenario
      ) :
        name(name),
        showRules(false)
      {
        this->setShowRules();
      }

      //! Destroy a Scenario object
      virtual ~Scenario() {

      }

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Reset the scenario state
      void reset() {
        this->reset_Scenario();
      }

      //! Run the scenario until there are no more rules to apply
      //! \return The number of steps taken
      U32 run(
          State& state //!< The system state
      ) {
        U32 numSteps = 0;
        this->runHelper(state, numSteps);
        return numSteps;
      }

      //! Return the next rule to apply
      //! \return The next rule, or nullptr if none
      Rule<State>* nextRule(
          State& state //!< The system state
      ) {
        Rule<State> *rule = nullptr;
        if (!this->isDone()) {
          rule = this->nextRule_Scenario(state);
        }
        return rule;
      }

      //! Query whether the scenario is done
      //! \return Whether the scenario is done
      bool isDone() const {
        return this->isDone_Scenario();
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected virtual methods
      // ----------------------------------------------------------------------

      //! The virtual implementation of reset required by Scenario
      virtual void reset_Scenario() = 0;

      //! The virtual implementation of nextRule required by Scenario
      //! \return The next rule, assuming isDone() is false, or nullptr if none
      virtual Rule<State>* nextRule_Scenario(
          State& state //!< The system state
      ) = 0;

      //! The virtual implementation of isDone required by Scenario
      //! \return Whether the scenario is done
      virtual bool isDone_Scenario() const = 0;

    private:

      // ----------------------------------------------------------------------
      // Private helper functions
      // ----------------------------------------------------------------------

      //! Run helper function
      void runHelper(
          State& state, //!< The system state
          U32& numSteps //!< The number of steps
      ) {
        this->reset();
        Rule<State>* rule = this->nextRule(state);
        while (rule != nullptr) {
          this->applyNextRule(*rule, state);
          ++numSteps;
          if (this->isDone()) {
            break;
          }
          rule = this->nextRule(state);
        }
      }

      //! Apply the next rule
      //! \return The number of steps taken
      void applyNextRule(
          Rule<State>& rule, //!< The rule
          State& state //!< The state
      ) {
        if (this->showRules) {
          printf(
              "[Scenario %s] Applying rule %s\n",
              this->name,
              rule.getName()
          );
        }
        rule.apply(state);
      }

      //! Set showRules
      void setShowRules() {
        const int status = system("test -f show-rules");
        if (status == 0) {
          showRules = true;
        }
      }

    public:

      // ----------------------------------------------------------------------
      // Public member variables
      // ----------------------------------------------------------------------

      //! The name of the scenario
      const char *const name;

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! Whether to report rule applications
      bool showRules;

  };

}

#endif
