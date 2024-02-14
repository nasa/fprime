// ======================================================================
// \title  Fw/FilePacket/GTest/StartPacket.cpp
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

    void FilePackets::StartPacket ::
      compare(
          const FilePacket::StartPacket& expected,
          const FilePacket::StartPacket& actual
      )
    {
      FilePackets::Header::compare(expected.m_header, actual.m_header);
      ASSERT_EQ(expected.m_fileSize, actual.m_fileSize);
      PathName::compare(expected.m_sourcePath, actual.m_sourcePath);
      PathName::compare(expected.m_destinationPath, actual.m_destinationPath);
    }

  }

}
