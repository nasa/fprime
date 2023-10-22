// ====================================================================== 
// \title  ImmediateBase.hpp
// \author Canham/Bocchino
// \brief  Base class for Immediate and ImmediateEOS
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_ImmediateBase_HPP
#define Svc_ImmediateBase_HPP

#include "CmdSequencerTester.hpp"

namespace Svc {

  namespace ImmediateBase {

    //! Base class for Immediate and ImmediateEOS
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
        // Tests parameterized by file type
        // ---------------------------------------------------------------------- 

        //! Run an automatic sequence by command
        void parameterizedAutoByCommand(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

        //! Run an automatic sequence by port call
        void parameterizedAutoByPort(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

        //! Send invalid manual commands while a sequence is running
        void parameterizedInvalidManualCommands(
            SequenceFiles::File& file //!< The file
        );

        //! Run a manual sequence
        void parameterizedManual(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands //!< The number of commands in the sequence
        );

        //! Run a sequence and, while it is running, start a new sequence
        //! The new sequence should cause an error
        void parameterizedNewSequence(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

        //! Load a sequence on initialization and then run it
        void parameterizedLoadOnInit(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

        //! Load a sequence, then run a sequence, then try to run a pre-loaded 
        //! sequence
        void parameterizedLoadRunRun(
            SequenceFiles::File& file, //!< The file
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound //!< The number of commands to execute
        );

      protected:

        // ---------------------------------------------------------------------- 
        // Protected helper methods
        // ---------------------------------------------------------------------- 

        //! Execute sequence commands for an automatic sequence
        void executeCommandsAuto(
            const char *const fileName, //!< The file name
            const U32 numCommands, //!< The number of commands in the sequence
            const U32 bound, //!< The number of commands to run
            const CmdExecMode::t mode //!< The mode
        );

        //! Execute sequence commands with a command response error
        void executeCommandsError(
            const char *const fileName, //!< The file name
            const U32 numCommands //!< The number of commands in the sequence
        );

    };

  }

}

#endif
