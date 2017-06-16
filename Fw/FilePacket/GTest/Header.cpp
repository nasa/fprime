// ====================================================================== 
// \title  Fw/FilePacket/GTest/Header.cpp
// \author bocchino
// \brief  Test utilities for file packet headers
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
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

#include <Fw/FilePacket/GTest/FilePackets.hpp>

namespace Fw {

  namespace GTest {

    void FilePackets::Header ::
      compare(
          const FilePacket::Header& expected,
          const FilePacket::Header& actual
      ) 
    {
      ASSERT_EQ(expected.type, actual.type);
      ASSERT_EQ(expected.sequenceIndex, actual.sequenceIndex);
    }

  }

}
