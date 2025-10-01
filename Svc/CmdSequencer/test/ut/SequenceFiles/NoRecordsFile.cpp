// ======================================================================
// \title  NoRecordsFile.cpp
// \author Rob Bocchino
// \brief  NoRecordsFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/NoRecordsFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

NoRecordsFile ::NoRecordsFile(const Format::t a_format) : File("norecords", a_format) {}

void NoRecordsFile::serializeFPrime(Fw::SerializeBufferBase& buffer) {
    // Header
    const FwSizeType numRecs = 0;
    const U32 recordDataSize = numRecs * FPrime::Records::STANDARD_SIZE;
    const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
    const TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    const U32 timeContext = 0;
    FPrime::Headers::serialize(dataSize, numRecs, timeBase, timeContext, buffer);

    // No Records

    // CRC
    FPrime::CRCs::serialize(buffer);
}

void NoRecordsFile ::serializeAMPCS(Fw::SerializeBufferBase& buffer) {
    // Header
    AMPCS::Headers::serialize(buffer);
    // No Records

    // CRC
    AMPCS::CRCs::createFile(buffer, this->getName().toChar());
}

}  // namespace SequenceFiles

}  // namespace Svc
