// ====================================================================== 
// \title  Immediate.hpp
// \author Canham/Bocchino
// \brief  Test immediate command sequences with  record
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_Immediate_HPP
#define Svc_Immediate_HPP

#include "Svc/CmdSequencer/test/ut/ImmediateBase.hpp"

namespace Svc {

  namespace Immediate {

    //! Test sequences with immediate commands followed by an  marker
    class Tester :
      public ImmediateBase::Tester
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

        //! Don't load any sequence, then try to run a sequence
        void NeverLoaded(void);

        //! Inject file errors
        void FileErrors(void);

        //! Load a sequence, then run a sequence, then try to run a pre-loaded 
        //! sequence
        void LoadRunRun(void);

        //! Run a complete sequence and then issue a command response
        void UnexpectedCommandResponse(void);

        //! Run a sequence and, while it is running, start a new sequence
        //! The new sequence should cause an error
        void NewSequence(void);

        //! Run a sequence manually
        void Manual(void);

        //! Run a sequence with failed commands
        void FailedCommands(void);

        //! Run an automatic sequence by command
        void AutoByCommand(void);

        //! Run an automatic sequence through a port call
        void AutoByPort(void);

        //! Send invalid manual commands while a sequence is running
        void InvalidManualCommands(void);

        //! Load a sequence on initialization and then run it
        void LoadOnInit(void);

        //! Sequence timeout
        void SequenceTimeout(void);

        //! Start and cancel a sequence
        void Cancel(void);

        //! Validate a sequence file
        void Validate(void);

      private:

        // ----------------------------------------------------------------------
        // Private helper methods 
        // ----------------------------------------------------------------------

        //! Execute commands for a manual sequence
        void executeCommandsManual(
            const char *const fileName, //!< The file name
            const U32 numCommands //!< The number of commands in the sequence
        );

    };

  }

}

#endif
