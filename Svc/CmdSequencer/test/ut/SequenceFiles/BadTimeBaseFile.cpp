// ======================================================================
// \title  BadTimeBaseFile.cpp
// \author Rob Bocchino
// \brief  BadTimeBaseFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadTimeBaseFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

BadTimeBaseFile ::BadTimeBaseFile(const U32 a_n, const Format::t a_format) : File(a_format), n(a_n) {
    Fw::String s;
    s.format("bad_time_base_%u", a_n);
    this->setName(s.toChar());
}

void BadTimeBaseFile ::serializeFPrime(Fw::SerializeBufferBase& buffer) {
    // Header
    const U32 recordDataSize = (this->n - 1) * FPrime::Records::STANDARD_SIZE + FPrime::Records::RECORD_DESCRIPTOR_SIZE;
    const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
    const TimeBase headerTimeBase = TimeBase::TB_PROC_TIME;
    // Create an invalid time base by adding 1 to the stored value
    const FwTimeBaseStoreType headerTimeBaseStore = static_cast<FwTimeBaseStoreType>(headerTimeBase) + 1;
    // First cast to the raw enum type TimeBase::T, then create a TimeBase from that
    const TimeBase recordTimeBase(static_cast<TimeBase::T>(headerTimeBaseStore));
    const U32 timeContext = 0;
    FPrime::Headers::serialize(dataSize, this->n, headerTimeBase, timeContext, buffer);
    // Records
    Fw::Time t(recordTimeBase, 0, 0);
    for (U32 i = 0; i < this->n - 1; ++i) {
        const FwOpcodeType opcode = i;
        const U32 argument = i + 1;
        FPrime::Records::serialize(CmdSequencerComponentImpl::Sequence::Record::RELATIVE, t, opcode, argument, buffer);
    }
    FPrime::Records::serialize(CmdSequencerComponentImpl::Sequence::Record::END_OF_SEQUENCE, t, buffer);
    // CRC
    FPrime::CRCs::serialize(buffer);
}

}  // namespace SequenceFiles

}  // namespace Svc
