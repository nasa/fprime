// ====================================================================== 
// \title  BadDescriptorFile.cpp
// \author Rob Bocchino
// \brief  BadDescriptorFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadDescriptorFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    BadDescriptorFile ::
      BadDescriptorFile(const U32 n, const Format::t format) :
        File(format),
        n(n)
    {
      Fw::EightyCharString baseName;
      baseName.format("bad_descriptor_%u", n);
      this->setName(baseName.toChar());
    }

    void BadDescriptorFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      const U32 recordDataSize = 
        this->n * SequenceFiles::FPrime::Records::STANDARD_SIZE;
      const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
      FPrime::Headers::serialize(
          dataSize,
          this->n,
          timeBase,
          timeContext,
          buffer
      );
      // Records
      for (U32 record = 0; record < this->n; record++) {
        Fw::Time t(TB_WORKSTATION_TIME, 0, 0);
        // Force an invalid record descriptor
        FPrime::Records::Descriptor descriptor =
          static_cast<FPrime::Records::Descriptor>(10);
        FPrime::Records::serialize(
            descriptor,
            t,
            record,
            record + 1,
            buffer
        );
      }
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

    void BadDescriptorFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // Header
      AMPCS::Headers::serialize(buffer);
      // Records
      for (U32 i = 0; i < this->n; ++i) {
        // Force an invalid time flag
        const AMPCSSequence::Record::TimeFlag::t timeFlag =
          static_cast<AMPCSSequence::Record::TimeFlag::t>(10);
        const AMPCSSequence::Record::Time::t time = 0;
        const AMPCSSequence::Record::Opcode::t opcode = i;
        const U32 argument = i + 1;
        AMPCS::Records::serialize(
            timeFlag,
            time,
            opcode,
            argument,
            buffer
        );
      }
      // CRC
      AMPCS::CRCs::createFile(buffer, this->getName().toChar());
    }

  }

}
