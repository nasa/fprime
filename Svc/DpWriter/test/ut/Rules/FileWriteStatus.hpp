// ======================================================================
// \title  FileWriteStatus.hpp
// \author Rob Bocchino
// \brief  FileWriteStatus class interface
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_FileWriteStatus_HPP
#define Svc_FileWriteStatus_HPP

#include "Svc/DpWriter/test/ut/Rules/Rules.hpp"
#include "Svc/DpWriter/test/ut/TestState/TestState.hpp"

namespace Svc {

  namespace FileWriteStatus {

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

        //! Rule FileWriteStatus::OK
        Rules::FileWriteStatus::OK ruleOK;

        //! Rule FileWriteStatus::Error
        Rules::FileWriteStatus::Error ruleError;

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
