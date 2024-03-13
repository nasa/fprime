// ======================================================================
// \title  BufferSendIn.hpp
// \author Rob Bocchino
// \brief  BufferSendIn class interface
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_BufferSendIn_HPP
#define Svc_BufferSendIn_HPP

#include "Svc/DpWriter/test/ut/Rules/Rules.hpp"
#include "Svc/DpWriter/test/ut/TestState/TestState.hpp"

namespace Svc {

namespace BufferSendIn {

class Tester {
  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! OK
    void OK();

    //! Invalid buffer
    void InvalidBuffer();

    //! Buffer too small for packet
    void BufferTooSmallForPacket();

    //! Invalid header hash
    void InvalidHeaderHash();

    //! Invalid header
    void InvalidHeader();

    //! Buffer too small for data
    void BufferTooSmallForData();

    //! File open error
    void FileOpenError();

    //! File write error
    void FileWriteError();

  public:
    // ----------------------------------------------------------------------
    // Rules
    // ----------------------------------------------------------------------

    //! Rule BufferSendIn::OK
    Rules::BufferSendIn::OK ruleOK;

    //! Rule BufferSendIn::InvalidBuffer
    Rules::BufferSendIn::InvalidBuffer ruleInvalidBuffer;

    //! Rule BufferSendIn::BufferTooSmallForPacket
    Rules::BufferSendIn::BufferTooSmallForPacket ruleBufferTooSmallForPacket;

    //! Rule BufferSendIn::InvalidHeaderHash
    Rules::BufferSendIn::InvalidHeaderHash ruleInvalidHeaderHash;

    //! Rule BufferSendIn::InvalidHeader
    Rules::BufferSendIn::InvalidHeader ruleInvalidHeader;

    //! Rule BufferSendIn::BufferTooSmallForData
    Rules::BufferSendIn::BufferTooSmallForData ruleBufferTooSmallForData;

    //! Rule BufferSendIn::FileOpenError
    Rules::BufferSendIn::FileOpenError ruleFileOpenError;

    //! Rule BufferSendIn::FileWriteError
    Rules::BufferSendIn::FileWriteError ruleFileWriteError;

  public:
    // ----------------------------------------------------------------------
    // Public member variables
    // ----------------------------------------------------------------------

    //! Test state
    TestState testState;
};

}  // namespace BufferSendIn

}  // namespace Svc

#endif
