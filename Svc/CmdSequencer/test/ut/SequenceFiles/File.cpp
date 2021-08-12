// ====================================================================== 
// \title  File.cpp
// \author Rob Bocchino
// \brief  File implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Types/EightyCharString.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "gtest/gtest.h"

#define BAD_FILE_FORMAT \
  ASSERT_TRUE(0) << "Bad file format " << this->format << "\n"

namespace Svc {

  namespace SequenceFiles {

    File ::
      File(const Format::t format) :
        format(format)
    {

    }

    File ::
      File (const char* const baseName, const Format::t format) :
        format(format)
    {
      this->setName(baseName);
    }

    File ::
      ~File(void)
    {

    }

    void File ::
      setName(const char *const baseName)
    {
      this->name = "bin/";
      switch (this->format) {
        case Format::F_PRIME:
          this->name += "f_prime_";
          break;
        case Format::AMPCS:
          this->name += "ampcs_";
          break;
        default:
          BAD_FILE_FORMAT;
          break;
      }
      this->name += baseName;
      this->name += ".bin";
    }

    const Fw::EightyCharString& File ::
      getName(void) const
    {
      return this->name;
    }

    void File ::getErrorInfo(ErrorInfo& errorInfo) {
      switch (this->format) {
        case Format::F_PRIME:
          errorInfo.open.fileName = this->name;
          errorInfo.headerRead.waitCount = 0;
          errorInfo.headerRead.fileName = this->name;
          errorInfo.dataRead.waitCount = 1;
          errorInfo.dataRead.fileName = this->name;
          break;
        case Format::AMPCS:
          errorInfo.open.fileName = this->name;
          errorInfo.open.fileName += ".CRC32";
          errorInfo.headerRead.waitCount = 1;
          errorInfo.headerRead.fileName = this->name;
          errorInfo.dataRead.waitCount = 2;
          errorInfo.dataRead.fileName = this->name;
          break;
        default:
          BAD_FILE_FORMAT;
          break;
      }
    }

    void File ::
      write(void)
    {
      Buffers::FileBuffer buffer;
      switch (this->format) {
        case Format::F_PRIME:
          this->serializeFPrime(buffer);
          break;
        case Format::AMPCS:
          this->serializeAMPCS(buffer);
          break;
        default:
          BAD_FILE_FORMAT;
          break;
      };
      Buffers::write(buffer, this->name.toChar());
    }

    void File ::
      remove(void)
    {
      Fw::EightyCharString s("rm -f ");
      s += this->getName();
      int status = system(s.toChar());
      ASSERT_EQ(0, status);
    }

    void File ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      ASSERT_TRUE(0) << "serializeFPrime is not implemented for " << this->name << "\n";
    }

    void File ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      ASSERT_TRUE(0) << "serializeAMPCS is not implemented for " << this->name << "\n";
    }

  }

}

