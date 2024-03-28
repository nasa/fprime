// ======================================================================
// \title  ProductGetIn.hpp
// \author Rob Bocchino
// \brief  ProductGetIn class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_ProductGetIn_HPP
#define Svc_ProductGetIn_HPP

#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace ProductGetIn {

    class Tester {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! BufferValid
        void BufferValid();

        //! BufferInvalid
        void BufferInvalid();

      public:

        // ----------------------------------------------------------------------
        // Rules
        // ----------------------------------------------------------------------

        //! Rule ProductGetIn::BufferValid
        Rules::ProductGetIn::BufferValid ruleBufferValid;

        //! Rule ProductGetIn::BufferInvalid
        Rules::ProductGetIn::BufferInvalid ruleBufferInvalid;

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
