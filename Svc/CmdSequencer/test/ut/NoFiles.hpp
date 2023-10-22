// ======================================================================
// \title  NoFiles.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_NoFiles_HPP
#define Svc_NoFiles_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace NoFiles {

    //! Test sequencer behavior with no input files
    class CmdSequencerTester :
      public Svc::CmdSequencerTester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object CmdSequencerTester
        CmdSequencerTester(
            const SequenceFiles::File::Format::t format =
            SequenceFiles::File::Format::F_PRIME //!< The file format to use
        );

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Initialization
        void Init();

        //! Issue a cancel command with no sequence active
        void NoSequenceActive();

    };

  }

}

#endif
