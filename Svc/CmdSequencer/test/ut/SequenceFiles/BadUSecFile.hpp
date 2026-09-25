// ======================================================================
// \title  BadUSecFile.hpp
// \author sjsreehari
// \brief  BadUSecFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_SequenceFiles_BadUSecFile_HPP
#define Svc_SequenceFiles_BadUSecFile_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"

namespace Svc {

namespace SequenceFiles {

//! A file with a microseconds field >= 1,000,000
class BadUSecFile : public File {
  public:
    //! Construct a BadUSecFile
    BadUSecFile(const Format::t a_format = Format::F_PRIME  //!< The file format
    );

  public:
    //! Serialize the file in F Prime format
    void serializeFPrime(Fw::LinearBufferBase& buffer  //!< The buffer
    );
};

}  // namespace SequenceFiles

}  // namespace Svc

#endif
