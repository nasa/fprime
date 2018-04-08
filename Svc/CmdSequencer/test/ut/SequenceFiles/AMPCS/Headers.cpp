// ====================================================================== 
// \title  Headers.cpp
// \author Rob Bocchino
// \brief  AMPCS sequence file headers
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

#include "gtest/gtest.h"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/Headers.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace Headers {

        void serialize(Fw::SerializeBufferBase& buffer) {
          serialize(0x11223344, buffer);
        }

        void serialize(
            const U32 value,
            Fw::SerializeBufferBase& buffer
        ) {
          ASSERT_EQ(Fw::FW_SERIALIZE_OK, buffer.serialize(value));
        }

      }

    }

  }

}
