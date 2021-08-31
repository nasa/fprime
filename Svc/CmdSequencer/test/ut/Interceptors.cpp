// ====================================================================== 
// \title  Interceptors.cpp
// \author Canham/Bocchino
// \brief  Implementation for Tester::Interceptors
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Os/Stubs/FileStubs.hpp"
#include "Svc/CmdSequencer/test/ut/Tester.hpp"
#include "gtest/gtest.h"

namespace Svc {

  Tester::Interceptors::Open ::
    Open(void) :
      fileStatus(Os::File::OP_OK)
  {

  }

  void Tester::Interceptors::Open ::
    enable(void)
  {
    Os::registerOpenInterceptor(registerFunction, this);
  }

  void Tester::Interceptors::Open ::
    disable(void)
  {
    Os::clearOpenInterceptor();
  }

  bool Tester::Interceptors::Open ::
    intercept(Os::File::Status& fileStatus)
  {
    fileStatus = this->fileStatus;
    return fileStatus == Os::File::OP_OK;
  }

  bool Tester::Interceptors::Open ::
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

  Tester::Interceptors::Read ::
    Read(void) :
      errorType(ErrorType::NONE),
      waitCount(0),
      size(0),
      fileStatus(Os::File::OP_OK)
  {

  }

  void Tester::Interceptors::Read ::
    enable(void)
  {
    Os::registerReadInterceptor(registerFunction, this);
  }

  void Tester::Interceptors::Read ::
    disable(void)
  {
    Os::clearReadInterceptor();
  }

  bool Tester::Interceptors::Read ::
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

  bool Tester::Interceptors::Read ::
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
