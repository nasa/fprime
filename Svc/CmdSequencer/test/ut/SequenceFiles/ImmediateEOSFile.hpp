// ====================================================================== 
// \title  ImmediateEOSFile.hpp
// \author Rob Bocchino
// \brief  ImmediateEOSFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_ImmediateEOSFile_HPP
#define Svc_SequenceFiles_ImmediateEOSFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A file containing n records. Each of the first n-1 records
    //! is a relative command with a zero time tag.
    //! The last record is END_OF_SEQUENCE.
    class ImmediateEOSFile :
      public File
    {

      public:

        //! Construct an ImmediateEOSFile
        ImmediateEOSFile(
            const U32 n, //!< The number of records
            const Format::t format //!< The file format
        );

      public:

        //! Serialize the file in F Prime format
        void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

      public:

        //! The number of records
        const U32 n;

    };

  }

}

#endif
