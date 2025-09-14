// ======================================================================
// \title  SizeFieldTooLargeFile.hpp
// \author Rob Bocchino
// \brief  SizeFieldTooLargeFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_SequenceFiles_SizeFieldTooLargeFile_HPP
#define Svc_SequenceFiles_SizeFieldTooLargeFile_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"

namespace Svc {

namespace SequenceFiles {

//! A file with a size field that is too large
class SizeFieldTooLargeFile : public File {
  public:
    //! Construct a SizeFieldTooLargeFile
    SizeFieldTooLargeFile(const U32 a_bufferSize,   //!< The buffer size
                          const Format::t a_format  //!< The file format
    );

  public:
    //! Serialize the file in F Prime format
    void serializeFPrime(Fw::SerializeBufferBase& buffer  //!< The buffer
    );

  public:
    //! The buffer size
    const U32 bufferSize;
};

}  // namespace SequenceFiles

}  // namespace Svc

#endif
