// ====================================================================== 
// \title  AMPCS.hpp
// \author Rob bocchino
// \brief  AMPCS-specific tests
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
