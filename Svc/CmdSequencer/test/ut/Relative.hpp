// ======================================================================
// \title  Relative.hpp
// \author Canham/Bocchino
// \brief  Test relative command sequences
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Relative_HPP
#define Svc_Relative_HPP

#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"

namespace Svc {

  namespace Relative {

    //! Test sequences with immediate commands followed by a marker
    class CmdSequencerTester :
      public MixedRelativeBase::CmdSequencerTester
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

        //! Run an automatic sequence by command
        void AutoByCommand();

        //! Validate a sequence file
        void Validate();

      private:

        // ----------------------------------------------------------------------
        // Private helper methods
        // ----------------------------------------------------------------------

        //! Execute sequence commands for an automatic sequence
        void executeCommandsAuto(
            const char *const fileName, //!< The file name
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound, //!< The number of commands to run
            const CmdExecMode::t mode //!< The mode
        );

    };

  }

}

#endif
