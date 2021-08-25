// ====================================================================== 
// \title  USecFieldTooShortFile.cpp
// \author Rob Bocchino
// \brief  USecFieldTooShortFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/USecFieldTooShortFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    USecFieldTooShortFile ::
      USecFieldTooShortFile(const Format::t format) :
        File("usec_field_too_short", format)
    {

    }

    void USecFieldTooShortFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const TimeBase timeBase = TB_WORKSTATION_TIME;
      const U32 timeContext = 0;
      const U32 numRecords = 1;
      const U32 recordSize =
          FPrime::Records::RECORD_DESCRIPTOR_SIZE + // descriptor
          sizeof(U32) + // seconds (CRC should land here)
          sizeof(U16); // short microseconds
      const U32 dataSize = numRecords * recordSize;
      FPrime::Headers::serialize(
          dataSize,
          numRecords,
          timeBase,
          timeContext,
          buffer
      );
      // Records
      const FPrime::Records::Descriptor descriptor = 
        CmdSequencerComponentImpl::Sequence::Record::RELATIVE;
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK,
          buffer.serialize(static_cast<U8>(descriptor))
      );
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK, 
          buffer.serialize(static_cast<U16>(0))
      );
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

  }

}
