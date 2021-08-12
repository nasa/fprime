// ====================================================================== 
// \title  MissingFile.hpp
// \author Rob Bocchino
// \brief  MissingFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_MissingFile_HPP
#define Svc_SequenceFiles_MissingFile_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    //! A missing file
    class MissingFile :
      public File
    {

      public:

        //! Construct a MissingFile
        MissingFile(
            const Format::t format //!< The file format
        );

      public:

        //! Serialize the file in AMPCS format
        void serializeAMPCS(
            Fw::SerializeBufferBase& buffer //!< The buffer
        );

    };

  }

}

#endif
