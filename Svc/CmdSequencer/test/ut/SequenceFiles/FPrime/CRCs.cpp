// ======================================================================
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file CRCs
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/CRCs.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

namespace FPrime {

namespace CRCs {

void serialize(Fw::LinearBufferBase& destBuffer) {
    CmdSequencerComponentImpl::FPrimeSequence::CRC crc;
    crc.init();
    crc.update(destBuffer.getBuffAddr(), destBuffer.getSize());
    U32 crcFinal = 0;
    crc.m_computed.finalize(crcFinal);
    ASSERT_EQ(destBuffer.serializeFrom(crcFinal), Fw::FW_SERIALIZE_OK);
}

}  // namespace CRCs

}  // namespace FPrime

}  // namespace SequenceFiles

}  // namespace Svc
