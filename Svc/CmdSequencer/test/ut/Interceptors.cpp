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

#include "Os/Stub/test/File.hpp"
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
      Os::Stub::File::Test::Data::setNextStatus(this->fileStatus);
  }

  void CmdSequencerTester::Interceptors::Open ::
    disable()
  {
      Os::Stub::File::Test::Data::setNextStatus(Os::File::Status::OP_OK);
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
    Os::Stub::File::Test::Data::testData.setReadOverride(CmdSequencerTester::Interceptors::Read::registerFunction, this);
  }

  void CmdSequencerTester::Interceptors::Read ::
    disable()
  {
      Os::Stub::File::Test::Data::testData.setReadOverride(Os::Stub::File::Test::Data::basicRead, nullptr);
  }

  bool CmdSequencerTester::Interceptors::Read ::
    intercept(
        Os::File::Status& fileStatus,
        U8 *buffer,
        FwSignedSizeType &size
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

    Os::File::Status CmdSequencerTester::Interceptors::Read ::
    registerFunction(
        U8 * buffer,
        FwSignedSizeType &size,
        bool waitForFull,
        void* ptr
    )
  {
    Os::File::Status fileStatus = Os::File::Status::OP_OK;
    EXPECT_TRUE(ptr);
    Read *const read = static_cast<Read*>(ptr);
    if (read->intercept(fileStatus, buffer, size)) {
        fileStatus = Os::Stub::File::Test::Data::basicRead(buffer, size, waitForFull, nullptr);
    }
      return fileStatus;
  }

}
