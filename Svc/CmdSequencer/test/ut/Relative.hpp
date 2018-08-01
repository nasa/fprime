// ====================================================================== 
// \title  Relative.hpp
// \author Canham/Bocchino
// \brief  Test relative command sequences
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

#ifndef Svc_Relative_HPP
#define Svc_Relative_HPP

#include "Svc/CmdSequencer/test/ut/MixedRelativeBase.hpp"

namespace Svc {

  namespace Relative {

    //! Test sequences with immediate commands followed by an  marker
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

    };

  }

}

#endif
