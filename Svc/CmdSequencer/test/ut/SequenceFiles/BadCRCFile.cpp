// ======================================================================
// \title  BadCRCFile.cpp
// \author Rob Bocchino
// \brief  BadCRCFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadCRCFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

BadCRCFile ::BadCRCFile(const Format::t a_format) : File("bad_crc", a_format) {}

void BadCRCFile ::serializeFPrime(Fw::LinearBufferBase& buffer) {
    // Header
    const U32 recordData = 0x10;
    const U32 dataSize = sizeof recordData + FPrime::CRCs::SIZE;
    const U32 numRecords = 1;
    const TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    const U32 timeContext = 0;
    FPrime::Headers::serialize(dataSize, numRecords, timeBase, timeContext, buffer);
    // Records
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(recordData));
    // CRC
    const U8* const addr = buffer.getBuffAddr();
    const U32 size = buffer.getSize();
    this->crc.init();
    this->crc.update(addr, size);
    U32 crcFinal = 0;
    this->crc.m_computed.finalize(crcFinal);
    crc.m_stored = crcFinal + 1;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(this->crc.m_stored));
}

void BadCRCFile ::serializeAMPCS(Fw::LinearBufferBase& buffer) {
    // Header
    AMPCS::Headers::serialize(buffer);
    // Records
    const U32 recordData = 0x10;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(recordData));
    // CRC
    AMPCS::CRCs::computeCRC(buffer, this->crc);
    U32 crcFinal = 0;
    this->crc.m_computed.finalize(crcFinal);
    crc.m_stored = crcFinal + 1;
    AMPCS::CRCs::writeCRC(this->crc.m_stored, this->getName().toChar());
}

const CmdSequencerComponentImpl::FPrimeSequence::CRC& BadCRCFile ::getCRC() const {
    return this->crc;
}

}  // namespace SequenceFiles

}  // namespace Svc
