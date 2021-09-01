// ====================================================================== 
// \title  Records.cpp
// \author Rob Bocchino
// \brief  AMPCS sequence file records
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Com/ComPacket.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/Records.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace Records {

        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag,
            const AMPCSSequence::Record::Time::t time,
            const Fw::SerializeBufferBase &cmdField,
            Fw::SerializeBufferBase& dest
        ) {
          const AMPCSSequence::Record::TimeFlag::Serial::t serialTimeFlag =
            timeFlag;
          const AMPCSSequence::Record::CmdLength::t cmdLength = 
            cmdField.getBuffLength();
          const U8 *const addr = cmdField.getBuffAddr();
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, dest.serialize(serialTimeFlag));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, dest.serialize(time));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, dest.serialize(cmdLength));
          ASSERT_EQ(
              Fw::FW_SERIALIZE_OK,
              // true means "don't serialize the length"
              dest.serialize(addr, cmdLength, true)
          );
        }

        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag,
            const AMPCSSequence::Record::Time::t time,
            const AMPCSSequence::Record::Opcode::t opcode,
            const U32 argument,
            Fw::SerializeBufferBase& dest
        ) {
          Fw::ComBuffer cmdField;
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, cmdField.serialize(opcode));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, cmdField.serialize(argument));
          Records::serialize(timeFlag, time, cmdField, dest);
        }

        void serialize(
            const AMPCSSequence::Record::TimeFlag::t timeFlag,
            const AMPCSSequence::Record::Time::t time,
            Fw::SerializeBufferBase& dest
        ) {
          Fw::ComBuffer cmdField;
          Records::serialize(timeFlag, time, cmdField, dest);
        }

      }

    }

  }

}
