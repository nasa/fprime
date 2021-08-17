// ====================================================================== 
// \title  RelativeFile.cpp
// \author Rob Bocchino
// \brief  RelativeFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/RelativeFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    RelativeFile ::
      RelativeFile(const U32 n, const Format::t format) :
        File(format),
        n(n)
    {
      Fw::EightyCharString s;
      s.format("relative_%u", n);
      this->setName(s.toChar());
    }

    void RelativeFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const U32 recordDataSize =
        this->n * FPrime::Records::STANDARD_SIZE;
      const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      FPrime::Headers::serialize(
          dataSize,
          this->n,
          timeBase,
          timeContext,
          buffer
      );
      // Records
      for (U32 i = 0; i < n; ++i) {
        const U32 seconds = 2;
        const U32 microseconds = 0;
        const FwOpcodeType opcode = i;
        const U32 argument = i + 1;
        Fw::Time t(TB_WORKSTATION_TIME, seconds, microseconds);
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE,
            t,
            opcode,
            argument,
            buffer
        );
      }
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

    void RelativeFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // Header
      AMPCS::Headers::serialize(buffer);
      // Records
      for (U32 i = 0; i < this->n; ++i) {
        const AMPCSSequence::Record::Time::t time = 2;
        const AMPCSSequence::Record::Opcode::t opcode = i;
        const U32 argument = i + 1;
        AMPCS::Records::serialize(
            AMPCSSequence::Record::TimeFlag::RELATIVE,
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
