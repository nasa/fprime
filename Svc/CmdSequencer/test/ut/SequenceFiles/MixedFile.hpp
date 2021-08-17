// ====================================================================== 
// \title  MixedFile.hpp
// \author Rob Bocchino
// \brief  MixedFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_MixedFile_HPP
#define Svc_SequenceFiles_MixedFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A file containing mixed immediate, relative, and absolute commands:
    // 1. An absolute command
    // 2. An immediate command
    // 3. A relative command
    // 4. An immediate command
    class MixedFile :
      public File
    {

      public:

        //! Construct a MixedFile
        MixedFile(
            const Format::t format //!< The file format
        );

      public:

        //! Serialize the file in F Prime format
        void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

        //! Serialize the file in AMPCS format
        void serializeAMPCS(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

    };

  }

}

#endif
