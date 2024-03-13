// ======================================================================
// \title  FileOpenStatus.hpp
// \author Rob Bocchino
// \brief  FileOpenStatus class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_FileOpenStatus_HPP
#define Svc_FileOpenStatus_HPP

#include "Svc/DpWriter/test/ut/Rules/Rules.hpp"
#include "Svc/DpWriter/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace FileOpenStatus {

    class Tester {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! OK
        void OK();

        //! Error
        void Error();

      public:

        // ----------------------------------------------------------------------
        // Rules
        // ----------------------------------------------------------------------

        //! Rule FileOpenStatus::OK
        Rules::FileOpenStatus::OK ruleOK;

        //! Rule FileOpenStatus::Error
        Rules::FileOpenStatus::Error ruleError;

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
