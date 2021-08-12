// ====================================================================== 
// \title  ImmediateEOSFile.cpp
// \author Rob Bocchino
// \brief  ImmediateEOSFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/ImmediateEOSFile.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace SequenceFiles {

    ImmediateEOSFile ::
      ImmediateEOSFile(const U32 n, const Format::t format) :
        File(format),
        n(n)
    {
      Fw::EightyCharString s;
      s.format("immediate_%u_eos", n);
      this->setName(s.toChar());
    }

    void ImmediateEOSFile ::
      serializeFPrime(Fw::SerializeBufferBase& buffer)
    {
      ASSERT_GE(this->n, 2U);
      // Header
      const U32 recordDataSize = 
        // n - 1 standard records
        (this->n-1) * FPrime::Records::STANDARD_SIZE +
        // 1 end-of-sequence record
        FPrime::Records::EOS_SIZE;
      const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
      FPrime::Headers::serialize(
          dataSize,
          this->n,
          TB_WORKSTATION_TIME,
          0,
          buffer
      );
      // Standard records
      Fw::Time t(TB_WORKSTATION_TIME, 0, 0);
      for (U32 i = 0; i < this->n - 1; i++) {
        const FwOpcodeType opcode = i;
        const U32 argument = i + 1;
        FPrime::Records::serialize(
            CmdSequencerComponentImpl::Sequence::Record::RELATIVE,
            t,
            opcode,
            argument,
            buffer
        );
      }
      // EOS record
      FPrime::Records::serialize(
          CmdSequencerComponentImpl::Sequence::Record::END_OF_SEQUENCE,
          t,
          buffer
      );
      // CRC
      FPrime::CRCs::serialize(buffer);
    }

  }

}
