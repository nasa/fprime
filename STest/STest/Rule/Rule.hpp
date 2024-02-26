// ======================================================================
// \title  Rule.hpp
// \author bocchino
// \brief  Rule interface for scenario testing
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef STest_Rule_HPP
#define STest_Rule_HPP

#include "STest/testing.hpp"
#include "STest/types/basic_types.h"

namespace STest {

  template<typename State> class Rule {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors
      // ----------------------------------------------------------------------

      //! Construct object Rule
      Rule(
          const char *const name //!< The name of the rule
      ) :
        m_name(name)
      {

      }

      //! Destroy object Rule
      virtual ~Rule() {

      }

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Apply the rule
      void apply(
          State& state //!< The system state
      ) {
        ASSERT_TRUE(this->precondition(state))
          << "precondition failed applying rule " << this->m_name;
        this->action(state);
      }

      //! Evaluate the precondition associated with the rule
      //! \return Whether the condition holds
      virtual bool precondition(
          const State& state //!< The system state
      ) = 0;

      //! Get rule name
      char const * getName() const {
        return this->m_name;
      }

    protected:

      // ----------------------------------------------------------------------
      // Protected instance methods
      // ----------------------------------------------------------------------

      //! Perform the action associated with the rule
      virtual void action(
          State& state //!< The system state
      ) = 0;

    public:

      // ----------------------------------------------------------------------
      // Public member variables
      // ----------------------------------------------------------------------

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The name of the rule
      const char *const m_name;

  };

}

#endif
