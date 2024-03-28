// ======================================================================
// \title  BufferGetStatus.hpp
// \author Rob Bocchino
// \brief  BufferGetStatus class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_BufferGetStatus_HPP
#define Svc_BufferGetStatus_HPP

#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace BufferGetStatus {

    class Tester {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Valid
        void Valid();

        //! Invalid
        void Invalid();

      public:

        // ----------------------------------------------------------------------
        // Rules
        // ----------------------------------------------------------------------

        //! Rule BufferGetStatus::Valid
        Rules::BufferGetStatus::Valid ruleValid;

        //! Rule BufferGetStatus::Invalid
        Rules::BufferGetStatus::Invalid ruleInvalid;

      private:

        // ----------------------------------------------------------------------
        // Public member variables
        // ----------------------------------------------------------------------

        //! Test state
        TestState testState;

    };

  }

}

#endif
