// ====================================================================== 
// \title  Fw/FilePacket/GTest/PathName.cpp
// \author bocchino
// \brief  Test utilities for start file packets
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include <Fw/FilePacket/GTest/FilePackets.hpp>
#include <Fw/Types/GTest/Bytes.hpp>

namespace Fw {

  namespace GTest {

    void FilePackets::PathName ::
      compare(
          const FilePacket::PathName& expected,
          const FilePacket::PathName& actual
      ) 
    {
      ASSERT_EQ(expected.length, actual.length);
      Bytes expectedPath(
          reinterpret_cast<const U8*>(expected.value),
          expected.length
      );
      Bytes actualPath(
          reinterpret_cast<const U8*>(actual.value),
          actual.length
      );
      Bytes::compare(expectedPath, actualPath);
    }

  }

}
