// ====================================================================== 
// \title  CommandBuffers.cpp
// \author Canham/Bocchino
// \brief  Command buffers for testing sequences
//
// \copyright
// Copyright (C) 2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

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
