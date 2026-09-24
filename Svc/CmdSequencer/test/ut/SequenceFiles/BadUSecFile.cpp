// ======================================================================
// \title  BadUSecFile.cpp
// \author Rob Bocchino
// \brief  BadUSecFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadUSecFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

BadUSecFile ::BadUSecFile(const Format::t a_format) : File("bad_usec", a_format) {}

void BadUSecFile ::serializeFPrime(Fw::LinearBufferBase& buffer) {
    // Header
    const TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    const U32 timeContext = 0;
    const U32 numRecords = 1;
    const U32 recordDataSize = numRecords * SequenceFiles::FPrime::Records::STANDARD_SIZE;
    const U32 dataSize = recordDataSize + FPrime::CRCs::SIZE;
    FPrime::Headers::serialize(dataSize, numRecords, timeBase, timeContext, buffer);

    // Records
    const FPrime::Records::Descriptor descriptor = CmdSequencerComponentImpl::Sequence::Record::RELATIVE;
    const U32 seconds = 0;
    const U32 uSeconds = 1000000;
    const FwOpcodeType opcode = 0;
    const U32 argument = 1;

    Fw::ComBuffer opcodeAndArgument;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, opcodeAndArgument.serializeFrom(opcode));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, opcodeAndArgument.serializeFrom(argument));

    const U8* const buffAddr = opcodeAndArgument.getBuffAddr();
    const U32 size = opcodeAndArgument.getSize();
    const U32 recSize = sizeof(FwPacketDescriptorType) + size;
    const FwPacketDescriptorType cmdDescriptor = Fw::ComPacketType::FW_PACKET_COMMAND;

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(static_cast<U8>(descriptor)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(seconds));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(uSeconds));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(recSize));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(cmdDescriptor));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serializeFrom(buffAddr, size, Fw::Serialization::OMIT_LENGTH));

    // CRC
    FPrime::CRCs::serialize(buffer);
}

}  // namespace SequenceFiles

}  // namespace Svc
