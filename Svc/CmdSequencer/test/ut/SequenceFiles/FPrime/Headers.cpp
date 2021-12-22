// ====================================================================== 
// \title  Headers.cpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
