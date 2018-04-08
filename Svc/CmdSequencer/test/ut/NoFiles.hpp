// ====================================================================== 
// \title  NoFiles.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

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
