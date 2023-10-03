// ======================================================================
// \title  Interceptors.cpp
// \author Canham/Bocchino
// \brief  Implementation for CmdSequencerTester::Interceptors
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Os/Stubs/FileStubs.hpp"
#include "CmdSequencerTester.hpp"
#include "gtest/gtest.h"

namespace Svc {

  CmdSequencerTester::Interceptors::Open ::
    Open() :
      fileStatus(Os::File::OP_OK)
  {

  }

  void CmdSequencerTester::Interceptors::Open ::
    enable()
  {
    Os::registerOpenInterceptor(registerFunction, this);
  }

  void CmdSequencerTester::Interceptors::Open ::
    disable()
  {
    Os::clearOpenInterceptor();
  }

  bool CmdSequencerTester::Interceptors::Open ::
    intercept(Os::File::Status& fileStatus)
  {
    fileStatus = this->fileStatus;
    return fileStatus == Os::File::OP_OK;
  }

  bool CmdSequencerTester::Interceptors::Open ::
    registerFunction(
        Os::File::Status& fileStatus,
        const char* fileName,
        Os::File::Mode mode,
        void* ptr
    )
  {
    EXPECT_TRUE(ptr);
    Open *const open = static_cast<Open*>(ptr);
    return open->intercept(fileStatus);
  }

  CmdSequencerTester::Interceptors::Read ::
    Read() :
      errorType(ErrorType::NONE),
      waitCount(0),
      size(0),
      fileStatus(Os::File::OP_OK)
  {

  }

  void CmdSequencerTester::Interceptors::Read ::
    enable()
  {
    Os::registerReadInterceptor(registerFunction, this);
  }

  void CmdSequencerTester::Interceptors::Read ::
    disable()
  {
    Os::clearReadInterceptor();
  }

  bool CmdSequencerTester::Interceptors::Read ::
    intercept(
        Os::File::Status& fileStatus,
        void *buffer,
        NATIVE_INT_TYPE &size
    )
  {
    bool status;
    if (this->errorType == ErrorType::NONE) {
      // No errors: return true
      status = true;
    }
    else if (this->waitCount > 0) {
      // Not time to inject an error yet: decrement wait count
      --this->waitCount;
      status = true;
    }
    else {
      // Time to inject an error: check test scenario
      switch (this->errorType) {
        case ErrorType::READ:
          fileStatus = this->fileStatus;
          break;
        case ErrorType::SIZE:
          size = this->size;
          fileStatus = Os::File::OP_OK;
          break;
        case Read::ErrorType::DATA:
          memcpy(buffer, this->data, size);
          fileStatus = Os::File::OP_OK;
          break;
        default:
          EXPECT_TRUE(false);
          break;
      }
      status = false;
    }
    return status;
  }

  bool CmdSequencerTester::Interceptors::Read ::
    registerFunction(
        Os::File::Status& fileStatus,
        void * buffer,
        NATIVE_INT_TYPE &size,
        bool waitForFull,
        void* ptr
    )
  {
    EXPECT_TRUE(ptr);
    Read *const read = static_cast<Read*>(ptr);
    return read->intercept(fileStatus, buffer, size);
  }

}
