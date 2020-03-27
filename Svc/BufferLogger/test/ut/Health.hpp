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

#include "Tester.hpp"

namespace Svc {

  namespace Health {

    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Health ping test
        void Ping(void);

    };

  }

}

#endif
