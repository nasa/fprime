// ====================================================================== 
// \title  ImmediateEOSFile.hpp
// \author Rob Bocchino
// \brief  ImmediateEOSFile interface
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
