// ====================================================================== 
// \title  MixedFile.cpp
// \author Rob Bocchino
// \brief  MixedFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MixedFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    MixedFile ::
      MixedFile(const Format::t format) :
        File("mixed", format)
    {

    }

    void MixedFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const NATIVE_INT_TYPE numRecs = 4;
      const U32 recordDataSize = 
        numRecs * FPrime::Records::STANDARD_SIZE;
      const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      FPrime::Headers::serialize(
          dataSize,
          numRecs,
          timeBase,
          timeContext,
          buffer
      );
      // Records
      {
        Fw::Time t;
        // Record 1: Absolute command
        t.set(TB_WORKSTATION_TIME, 2, 0);
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::ABSOLUTE,
            t,
            0,
            1,
            buffer
        );
        // Record 2: Immediate command
        t.set(TB_WORKSTATION_TIME, 0, 0);
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE,
            t,
            2,
            3,
            buffer
        );
        // Record 3: Relative command
        t.set(TB_WORKSTATION_TIME, 1, 0);
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE,
            t,
            4,
            5,
            buffer
        );
        // Record 4: Immediate command
        t.set(TB_WORKSTATION_TIME, 0, 0);
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE,
            t,
            6,
            7,
            buffer
        );
      }
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

    void MixedFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // Header
      AMPCS::Headers::serialize(buffer);
      // Records
      {
        // Record 1: Absolute command
        const AMPCSSequence::Record::Time::t time = 2;
        const AMPCSSequence::Record::Opcode::t opcode = 0;
        const U32 argument = 1;
        AMPCS::Records::serialize(
            AMPCSSequence::Record::TimeFlag::ABSOLUTE,
            time,
            opcode,
            argument,
            buffer
        );
      }
      {
        // Record 2: Immediate command
        const AMPCSSequence::Record::Time::t time = 0;
        const AMPCSSequence::Record::Opcode::t opcode = 2;
        const U32 argument = 3;
        AMPCS::Records::serialize(
            AMPCSSequence::Record::TimeFlag::RELATIVE,
            time,
            opcode,
            argument,
            buffer
        );
      }
      {
        // Record 3: Relative command
        const AMPCSSequence::Record::Time::t time = 1;
        const AMPCSSequence::Record::Opcode::t opcode = 4;
        const U32 argument = 5;
        AMPCS::Records::serialize(
            AMPCSSequence::Record::TimeFlag::RELATIVE,
            time,
            opcode,
            argument,
            buffer
        );
      }
      {
        // Record 4: Immediate command
        const AMPCSSequence::Record::Time::t time = 0;
        const AMPCSSequence::Record::Opcode::t opcode = 6;
        const U32 argument = 7;
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
