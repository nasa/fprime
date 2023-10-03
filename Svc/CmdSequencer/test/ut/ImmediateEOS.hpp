// ======================================================================
// \title  ImmediateEOS.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with EOS record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_ImmediateEOS_HPP
#define Svc_ImmediateEOS_HPP

#include "Svc/CmdSequencer/test/ut/ImmediateBase.hpp"

namespace Svc {

  namespace ImmediateEOS {

    //! Test sequences with immediate commands followed by an EOS marker
    class CmdSequencerTester :
      public ImmediateBase::CmdSequencerTester
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

        //! Inject file errors
        void FileErrors();

        //! Run a complete sequence and then issue a command response
        void UnexpectedCommandResponse();

        //! Run a sequence and, while it is running, start a new sequence
        //! The new sequence should cause an error
        void NewSequence();

        //! Run a sequence manually
        void Manual();

        //! Run an automatic sequence by command
        void AutoByCommand();

        //! Run an automatic sequence through a port call
        void AutoByPort();

        //! Send invalid manual commands while a sequence is running
        void InvalidManualCommands();

        //! Sequence timeout
        void SequenceTimeout();

        //! Start and cancel a sequence
        void Cancel();

        //! Validate a sequence file
        void Validate();

        // ----------------------------------------------------------------------
        // Private helper methods
        // ----------------------------------------------------------------------

      private:

        //! Execute commands for a manual sequence
        void executeCommandsManual(
            const char *const fileName, //!< The file name
            const U32 numCommands //!< The number of commands in the sequence
        );

    };

  }

}

#endif
