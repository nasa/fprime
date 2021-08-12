// ====================================================================== 
// \title  SizeFieldTooSmallFile.cpp
// \author Rob Bocchino
// \brief  SizeFieldTooSmallFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SizeFieldTooSmallFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    SizeFieldTooSmallFile ::
      SizeFieldTooSmallFile(const Format::t format) :
        File("size_field_too_small", format)
    {

    }

    void SizeFieldTooSmallFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      // Header
      const U32 dataSize =
        FPrime::Records::RECORD_DESCRIPTOR_SIZE +
        sizeof(U32) + // seconds
        sizeof(U32) + // subseconds (CRC should land here)
        sizeof(U16); // short size
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
      const FPrime::Records::Descriptor descriptor = 
          CmdSequencerComponentImpl::Sequence::Record::RELATIVE;
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK,
          buffer.serialize(static_cast<U8>(descriptor))
      );
      ASSERT_EQ(
          Fw::FW_SERIALIZE_OK, 
          buffer.serialize(static_cast<U32>(0))
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
