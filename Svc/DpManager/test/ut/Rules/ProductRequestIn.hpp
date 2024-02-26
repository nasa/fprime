// ======================================================================
// \title  ProductRequestIn.hpp
// \author Rob Bocchino
// \brief  ProductRequestIn class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_ProductRequestIn_HPP
#define Svc_ProductRequestIn_HPP

#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace ProductRequestIn {

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

        //! Rule ProductRequestIn::BufferValid
        Rules::ProductRequestIn::BufferValid ruleBufferValid;

        //! Rule ProductRequestIn::BufferInvalid
        Rules::ProductRequestIn::BufferInvalid ruleBufferInvalid;

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
