// ======================================================================
// \title  Mixed.hpp
// \author Canham/Bocchino
// \brief  Test mixed immediate, relative, and absolute commands
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_Mixed_HPP
#define Svc_Mixed_HPP

#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"

namespace Svc {

  namespace Mixed {

    //! Test sequences with mixed absolute and relative commands
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

        //! Execute command 1 (immediate)
        void executeCommand1(
            const char *const fileName //!< The file name
        );

        //! Execute command 2 (absolute)
        void executeCommand2(
            const char *const fileName //!< The file name
        );

        //! Execute command 3 (relative)
        void executeCommand3(
            const char *const fileName //!< The file name
        );

        //! Execute command 4 (immediate)
        void executeCommand4(
            const char *const fileName //!< The file name
        );

    };

  }

}

#endif
