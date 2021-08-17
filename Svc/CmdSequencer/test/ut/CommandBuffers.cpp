// ====================================================================== 
// \title  CommandBuffers.cpp
// \author Canham/Bocchino
// \brief  Command buffers for testing sequences
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Com/ComPacket.hpp"
#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "gtest/gtest.h"

namespace Svc {

  namespace CommandBuffers {

    void create(
        Fw::ComBuffer& comBuff,
        const FwOpcodeType opcode,
        const U32 argument
    ) {
      comBuff.resetSer();
      const FwPacketDescriptorType descriptor = Fw::ComPacket::FW_PACKET_COMMAND;
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serialize(descriptor));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serialize(opcode));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK, comBuff.serialize(argument));
    }

  }

}
