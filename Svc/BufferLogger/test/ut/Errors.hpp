// ======================================================================
// \title  Errors.hpp
// \author bocchino, mereweth
// \brief  Interface for BufferLogger error tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Errors_HPP
#define Svc_Errors_HPP

#include "Tester.hpp"

namespace Svc {

  namespace Errors {

    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Log file open error
        void LogFileOpen(void);

        //! Log file write error
        void LogFileWrite(void);

        //! Log file validation error
        void LogFileValidation(void);

    };

  }

}

#endif
