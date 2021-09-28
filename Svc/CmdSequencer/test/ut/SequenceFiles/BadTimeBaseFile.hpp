// ====================================================================== 
// \title  BadTimeBaseFile.hpp
// \author Rob Bocchino
// \brief  BadTimeBaseFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_BadTimeBaseFile_HPP
#define Svc_SequenceFiles_BadTimeBaseFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    // A file containing records with bad time bases
    class BadTimeBaseFile :
      public File
    {

      public:

        //! Construct a BadTimeBaseFile
        BadTimeBaseFile(
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
