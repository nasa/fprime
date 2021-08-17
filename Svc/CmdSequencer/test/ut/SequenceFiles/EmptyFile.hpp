// ====================================================================== 
// \title  EmptyFile.hpp
// \author Rob Bocchino
// \brief  EmptyFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_EmptyFile_HPP
#define Svc_SequenceFiles_EmptyFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! An empty file
    class EmptyFile :
      public File
    {

      public:

        //! Construct an empty file
        EmptyFile(
            const Format::t format //!< The file format
        );

      public:

        //! Serialize an empty file in F Prime format
        void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

        //! Serialize an empty file in AMPCS format
        void serializeAMPCS(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

    };

  }

}

#endif
