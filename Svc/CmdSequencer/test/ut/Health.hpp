// ======================================================================
// \title  Health.hpp
// \author Canham/Bocchino
// \brief  Test health pings
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Health_HPP
#define Svc_Health_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace Health {

    class CmdSequencerTester :
      public Svc::CmdSequencerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Test health pings
        void Ping();

    };

  }

}

#endif
