// ====================================================================== 
// \title  MissingCRCFile.cpp
// \author Rob Bocchino
// \brief  MissingCRCFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MissingCRCFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    MissingCRCFile ::
      MissingCRCFile(const Format::t format) :
        File("invalid_record", format)
    {

    }

    void MissingCRCFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const U8 data = 1;
      const U32 numRecords = 1;
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      FPrime::Headers::serialize(
          sizeof data,
          numRecords,
          timeBase,
          timeContext,
          buffer
      );
      // Records + CRC
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK,
          buffer.serialize(data)
      );
    }

    void MissingCRCFile ::
      serializeAMPCS(Fw::SerializeBufferBase& buffer)
    {
      // Header
      AMPCS::Headers::serialize(buffer);
      // Records
      const U8 data = 1;
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK,
          buffer.serialize(data)
      );
      // CRC
      AMPCS::CRCs::removeFile(this->getName().toChar());
    }

  }

}
