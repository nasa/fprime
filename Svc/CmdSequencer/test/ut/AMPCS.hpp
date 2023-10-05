// ======================================================================
// \title  AMPCS.hpp
// \author Rob bocchino
// \brief  AMPCS-specific tests
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_AMPCS_HPP
#define Svc_AMPCS_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace AMPCS {

    //! Test sequencer behavior with no input files
    class CmdSequencerTester :
      public Svc::CmdSequencerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object CmdSequencerTester
        CmdSequencerTester();

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Missing CRC
        void MissingCRC();

        //! Missing file
        void MissingFile();

    };

  }

}

#endif
