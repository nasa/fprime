// ====================================================================== 
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  F Prime sequence file CRCs
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
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

#include "gtest/gtest.h"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/CRCs.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace CRCs {

        void serialize(Fw::SerializeBufferBase& destBuffer) {
          CmdSequencerComponentImpl::FPrimeSequence::CRC crc;
          crc.init();
          crc.update(destBuffer.getBuffAddr(), destBuffer.getBuffLength());
          crc.finalize();
          ASSERT_EQ(destBuffer.serialize(crc.m_computed), Fw::FW_SERIALIZE_OK);
        }

      }

    }

  }

}
