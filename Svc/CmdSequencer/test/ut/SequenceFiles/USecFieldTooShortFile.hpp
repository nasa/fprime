// ====================================================================== 
// \title  USecFieldTooShortFile.hpp
// \author Rob Bocchino
// \brief  USecFieldTooShortFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_USecFieldTooShortFile_HPP
#define Svc_SequenceFiles_USecFieldTooShortFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A file with a microseconds field that is too short
    class USecFieldTooShortFile :
      public File
    {

      public:

        //! Construct a USecFieldTooShortFile
        USecFieldTooShortFile(
            const Format::t format //!< The file format
        );

      public:

        //! Serialize the file in F Prime format
        void serializeFPrime(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

    };

  }

}

#endif
