// ======================================================================
// \title  ImmediateFile.cpp
// \author Rob Bocchino
// \brief  ImmediateFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/CmdSequencer/test/ut/SequenceFiles/ImmediateFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

ImmediateFile ::ImmediateFile(const U32 a_n, const Format::t a_format) : File(a_format), n(a_n) {
    Fw::String s;
    s.format("immediate_%u", a_n);
    this->setName(s.toChar());
}

void ImmediateFile ::serializeFPrime(Fw::SerializeBufferBase& buffer) {
    // Header
    const U32 recordDataSize = this->n * FPrime::Records::STANDARD_SIZE;
    const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
    const TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    const U32 timeContext = 0;
    FPrime::Headers::serialize(dataSize, this->n, timeBase, timeContext, buffer);
    // Records
    for (U32 i = 0; i < this->n; i++) {
        Fw::Time t(TimeBase::TB_WORKSTATION_TIME, 0, 0);
        const FwOpcodeType opcode = i;
        const U32 argument = i + 1;
        FPrime::Records::serialize(CmdSequencerComponentImpl::Sequence::Record::RELATIVE, t, opcode, argument, buffer);
    }
    // CRC
    FPrime::CRCs::serialize(buffer);
}

void ImmediateFile ::serializeAMPCS(Fw::SerializeBufferBase& buffer) {
    // Header
    AMPCS::Headers::serialize(buffer);
    // Records
    for (U32 i = 0; i < this->n; ++i) {
        const AMPCSSequence::Record::Time::t time = 0;
        const AMPCSSequence::Record::Opcode::t opcode = i;
        const U32 argument = i + 1;
        AMPCS::Records::serialize(AMPCSSequence::Record::TimeFlag::RELATIVE, time, opcode, argument, buffer);
    }
    // CRC
    AMPCS::CRCs::createFile(buffer, this->getName().toChar());
}

}  // namespace SequenceFiles

}  // namespace Svc
