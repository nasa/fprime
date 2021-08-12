// ====================================================================== 
// \title  MissingCRCFile.hpp
// \author Rob Bocchino
// \brief  MissingCRCFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_MissingCRCFile_HPP
#define Svc_SequenceFiles_MissingCRCFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    class MissingCRCFile :
      public File
    {

      public:

        //! Construct a MissingCRCFile
        MissingCRCFile(
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
