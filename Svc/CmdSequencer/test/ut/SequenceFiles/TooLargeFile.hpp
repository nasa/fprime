// ======================================================================
// \title  TooLargeFile.hpp
// \author Rob Bocchino
// \brief  TooLargeFile interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_SequenceFiles_TooLargeFile_HPP
#define Svc_SequenceFiles_TooLargeFile_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"

namespace Svc {

namespace SequenceFiles {

//! A file that is too large for the sequence buffer
class TooLargeFile : public File {
  public:
    //! Construct a TooLargeFile
    TooLargeFile(const U32 a_bufferSize,   //!< The sequence buffer size
                 const Format::t a_format  //!< The file format
    );

  public:
    //! Serialize the file in F Prime format
    void serializeFPrime(Fw::SerializeBufferBase& buffer  //!< The buffer
    );

    //! Serialize the file in AMPCS format
    void serializeAMPCS(Fw::SerializeBufferBase& buffer  //!< The buffer
    );

    //! Get the data size
    U32 getDataSize() const;

  public:
    //! The sequence buffer size
    const U32 bufferSize;
};

}  // namespace SequenceFiles

}  // namespace Svc

#endif
