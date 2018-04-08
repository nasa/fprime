// ====================================================================== 
// \title  Headers.cpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
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
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/Headers.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace FPrime {

      namespace Headers {

        void serialize(
            U32 dataSize,
            U32 numRecords,
            FwTimeBaseStoreType timeBase,
            FwTimeContextStoreType timeContext,
            Fw::SerializeBufferBase& destBuffer
        ) {
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(dataSize));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(numRecords));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(timeBase));
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, destBuffer.serialize(timeContext));
        }

      }

    }

  }

}
