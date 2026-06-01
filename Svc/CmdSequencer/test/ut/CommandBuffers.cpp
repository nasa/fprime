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
    // Command buffer layout is [opcode][argument]; the APID is passed via the port argument.
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(opcode));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serializeFrom(argument));
}

}  // namespace CommandBuffers

}  // namespace Svc
