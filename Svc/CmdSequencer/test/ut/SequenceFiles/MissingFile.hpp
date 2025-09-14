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

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"

namespace Svc {

namespace SequenceFiles {

//! A missing file
class MissingFile : public File {
  public:
    //! Construct a MissingFile
    MissingFile(const Format::t a_format  //!< The file format
    );

  public:
    //! Serialize the file in AMPCS format
    void serializeAMPCS(Fw::SerializeBufferBase& buffer  //!< The buffer
    );
};

}  // namespace SequenceFiles

}  // namespace Svc

#endif
