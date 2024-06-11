// ======================================================================
// \title  Health.hpp
// \author bocchino
// \brief  Interface for BufferLogger health tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Health_HPP
#define Svc_Health_HPP

#include "BufferLoggerTester.hpp"

namespace Svc {

  namespace Health {

    class BufferLoggerTester :
      public Svc::BufferLoggerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Health ping test
        void Ping();

    };

  }

}

#endif
