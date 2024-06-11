// ======================================================================
// \title  CLEAR_EVENT_THROTTLE.hpp
// \author Rob Bocchino
// \brief  CLEAR_EVENT_THROTTLE class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_CLEAR_EVENT_THROTTLE_HPP
#define Svc_CLEAR_EVENT_THROTTLE_HPP

#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace CLEAR_EVENT_THROTTLE {

    class Tester {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! OK
        void OK();

      public:

        // ----------------------------------------------------------------------
        // Rules
        // ----------------------------------------------------------------------

        //! Rule CLEAR_EVENT_THROTTLE::OK
        Rules::CLEAR_EVENT_THROTTLE::OK ruleOK;

      public:

        // ----------------------------------------------------------------------
        // Public member variables
        // ----------------------------------------------------------------------

        //! Test state
        TestState testState;

    };

  }

}

#endif
