// ====================================================================== 
// \title  NoFiles.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_NoFiles_HPP
#define Svc_NoFiles_HPP

#include "Svc/CmdSequencer/test/ut/Tester.hpp"

namespace Svc {

  namespace NoFiles {

    //! Test sequencer behavior with no input files
    class Tester :
      public Svc::Tester
    {

      public:

        // ----------------------------------------------------------------------
        // Constructors
        // ----------------------------------------------------------------------

        //! Construct object Tester
        Tester(
            const SequenceFiles::File::Format::t format = 
            SequenceFiles::File::Format::F_PRIME //!< The file format to use
        );

      public:

        // ---------------------------------------------------------------------- 
        // Tests
        // ---------------------------------------------------------------------- 

        //! Initialization
        void Init(void);

        //! Issue a cancel command with no sequence active
        void NoSequenceActive(void);

    };

  }

}

#endif
