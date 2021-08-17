// ====================================================================== 
// \title  BadCRCFile.cpp
// \author Rob Bocchino
// \brief  BadCRCFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadCRCFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    BadCRCFile ::
      BadCRCFile(const Format::t format) :
        File("bad_crc", format)
    {

    }

    void BadCRCFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const U32 recordData = 0x10;
      const U32 dataSize = sizeof recordData + FPrime::CRCs::SIZE;
      const U32 numRecords = 1;
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      FPrime::Headers::serialize(
          dataSize,
          numRecords,
          timeBase,
          timeContext,
          buffer
      );
      // Records
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serialize(recordData));
      // CRC
      const U8 *const addr = buffer.getBuffAddr();
      const U32 size = buffer.getBuffLength();
      this->crc.init();
      this->crc.update(addr, size);
      this->crc.finalize();
      crc.m_stored = this->crc.m_computed + 1;
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK,
          buffer.serialize(this->crc.m_stored)
      );
    }

    void BadCRCFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // Header
      AMPCS::Headers::serialize(buffer);
      // Records
      const U32 recordData = 0x10;
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serialize(recordData));
      // CRC
      AMPCS::CRCs::computeCRC(buffer, this->crc);
      this->crc.m_stored = this->crc.m_computed + 1;
      AMPCS::CRCs::writeCRC(this->crc.m_stored, this->getName().toChar());
    }

    const CmdSequencerComponentImpl::FPrimeSequence::CRC& BadCRCFile ::
      getCRC(void) const
    {
      return this->crc;
    }

  }

}
