// ====================================================================== 
// \title  MixedRelativeBase.hpp
// \author Canham/Bocchino
// \brief  Base class for Mixed and Relative
//
// \copyright
// Copyright (C) 2018 California Institute of Technology.
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

#ifndef Svc_MixedRelativeBase_HPP
#define Svc_MixedRelativeBase_HPP

#include "Svc/CmdSequencer/test/ut/Tester.hpp"

namespace Svc {

  namespace MixedRelativeBase {

    //! Base class for Mixed and Relative
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
        // Tests parameterized by file type
        // ---------------------------------------------------------------------- 

        //! Run an automatic sequence by command
        void parameterizedAutoByCommand(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

    };

  }

}

#endif
