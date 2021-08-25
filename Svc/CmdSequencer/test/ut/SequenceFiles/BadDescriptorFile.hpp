// ====================================================================== 
// \title  BadDescriptorFile.hpp
// \author Rob Bocchino
// \brief  BadDescriptorFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_BadDescriptorFile_HPP
#define Svc_SequenceFiles_BadDescriptorFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A file with a bad record descriptor
    class BadDescriptorFile :
      public File
    {

      public:

        //! Construct a BadDescriptorFile
        BadDescriptorFile(
            const U32 n, //!< The number of records
            const Format::t = Format::F_PRIME //!< The file format
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

      public:

        //! The number of records
        const U32 n;

    };

  }

}

#endif
