// ====================================================================== 
// \title  Mixed.hpp
// \author Canham/Bocchino
// \brief  Test mixed immediate, relative, and absolute commands
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

#ifndef Svc_Mixed_HPP
#define Svc_Mixed_HPP

#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"

namespace Svc {

  namespace Mixed {

    //! Test sequences with mixed absolute and relative commands
    class Tester :
      public MixedRelativeBase::Tester
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

        //! Run an automatic sequence by command
        void AutoByCommand(void);

        //! Validate a sequence file
        void Validate(void);

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
