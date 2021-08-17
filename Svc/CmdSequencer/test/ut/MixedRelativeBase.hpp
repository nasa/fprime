// ====================================================================== 
// \title  MixedRelativeBase.hpp
// \author Canham/Bocchino
// \brief  Base class for Mixed and Relative
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
