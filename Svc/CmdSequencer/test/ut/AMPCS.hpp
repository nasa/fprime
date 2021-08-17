// ====================================================================== 
// \title  AMPCS.hpp
// \author Rob bocchino
// \brief  AMPCS-specific tests
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_AMPCS_HPP
#define Svc_AMPCS_HPP

#include "Svc/CmdSequencer/test/ut/Tester.hpp"

namespace Svc {

  namespace AMPCS {

    //! Test sequencer behavior with no input files
    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object Tester
        Tester(void);

      public:

        // ---------------------------------------------------------------------- 
        // Tests
        // ---------------------------------------------------------------------- 

        //! Missing CRC
        void MissingCRC(void);

        //! Missing file
        void MissingFile(void);

    };

  }

}

#endif
