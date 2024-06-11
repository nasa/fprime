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

#include "BufferLoggerTester.hpp"

namespace Svc {

  namespace Errors {

    class BufferLoggerTester :
      public Svc::BufferLoggerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Log file open error
        void LogFileOpen();

        //! Log file write error
        void LogFileWrite();

        //! Log file validation error
        void LogFileValidation();

    };

  }

}

#endif
