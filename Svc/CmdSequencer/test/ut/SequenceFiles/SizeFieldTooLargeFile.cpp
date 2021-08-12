// ====================================================================== 
// \title  SizeFieldTooLargeFile.cpp
// \author Rob Bocchino
// \brief  SizeFieldTooLargeFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SizeFieldTooLargeFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    SizeFieldTooLargeFile ::
      SizeFieldTooLargeFile(
          const U32 bufferSize,
          const Format::t format
      ) :
        File("size_field_too_large", format),
        bufferSize(bufferSize)
    {

    }

    void SizeFieldTooLargeFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const U32 recordSize =
          sizeof(U8) + // Descriptor
          sizeof(U32) + // Seconds
          sizeof(U32) + // Microseconds
          sizeof(U32); // Size
      const U32 dataSize = recordSize + FPrime::CRCs::SIZE;
      const U32 numRecs = 1;
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
        // Descriptor
        const FPrime::Records::Descriptor descriptor = 
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE;
        ASSERT_EQ(
            Fw::FW_SERIALIZE_OK, 
            buffer.serialize(static_cast<U8>(descriptor))
        );
        // Seconds
        ASSERT_EQ(
            Fw::FW_SERIALIZE_OK,
            buffer.serialize(static_cast<U32>(0))
        );
        // Microseconds
        ASSERT_EQ(
            Fw::FW_SERIALIZE_OK,
            buffer.serialize(static_cast<U32>(0))
        );
        // Record size
        ASSERT_EQ(
            Fw::FW_SERIALIZE_OK, 
            buffer.serialize(static_cast<U32>(2 * this->bufferSize))
        );
      }
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

  }

}
