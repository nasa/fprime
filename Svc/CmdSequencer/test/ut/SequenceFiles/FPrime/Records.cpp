// ====================================================================== 
// \title  Records.cpp
// \author Rob Bocchino
// \brief  F Prime sequence file records
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Com/ComPacket.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/Records.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace Records {

        void serialize(
            Records::Descriptor desc,
            const Fw::Time &time,
            const Fw::ComBuffer &opcodeAndArgument,
            Fw::SerializeBufferBase& destBuffer
        ) {
          const U8 descU8 = desc;
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(descU8));
          if (desc != CmdSequencerComponentImpl::Sequence::Record::END_OF_SEQUENCE) {
            const U8 *const buffAddr = opcodeAndArgument.getBuffAddr();
            const U32 size = opcodeAndArgument.getBuffLength();
            const U32 recSize = sizeof(FwPacketDescriptorType) + size;
            const FwPacketDescriptorType cmdDescriptor = Fw::ComPacket::FW_PACKET_COMMAND;
            const U32 seconds = time.getSeconds();
            const U32 uSeconds = time.getUSeconds();
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(seconds));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(uSeconds));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(recSize));
            ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(cmdDescriptor));
            ASSERT_EQ(
                Fw::FW_SERIALIZE_OK,
                // true means "don't serialize the size"
                destBuffer.serialize(buffAddr, size, true)
            );
          }
        }

        void serialize(
            Descriptor desc,
            const Fw::Time &time,
            const FwOpcodeType opcode,
            const U32 argument,
            Fw::SerializeBufferBase& destBuffer
        ) {
          Fw::ComBuffer opcodeAndArgument;
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, opcodeAndArgument.serialize(opcode));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, opcodeAndArgument.serialize(argument));
          Records::serialize(desc, time, opcodeAndArgument, destBuffer);
        }

        void serialize(
            Descriptor desc,
            const Fw::Time &time,
            Fw::SerializeBufferBase& destBuffer
        ) {
          Fw::ComBuffer opcodeAndArgument;
          Records::serialize(desc, time, opcodeAndArgument, destBuffer);
        }

      }

    }

  }

}
