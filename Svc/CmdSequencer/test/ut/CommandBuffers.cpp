// ======================================================================
// \title  CommandBuffers.cpp
// \author Canham/Bocchino
// \brief  Command buffers for testing sequences
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace CommandBuffers {

void create(Fw::ComBuffer& comBuff, const FwOpcodeType opcode, const U32 argument) {
    comBuff.resetSer();
    const FwPacketDescriptorType descriptor = Fw::ComPacketType::FW_PACKET_COMMAND;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(descriptor));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(opcode));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(argument));
}

}  // namespace CommandBuffers

}  // namespace Svc
