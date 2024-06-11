// ======================================================================
// \title  ProductSendIn.hpp
// \author Rob Bocchino
// \brief  ProductSendIn class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_ProductSendIn_HPP
#define Svc_ProductSendIn_HPP

#include "Svc/DpManager/test/ut/Rules/Rules.hpp"
#include "Svc/DpManager/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace ProductSendIn {

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

        //! Rule ProductSendIn::OK
        Rules::ProductSendIn::OK ruleOK;

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
