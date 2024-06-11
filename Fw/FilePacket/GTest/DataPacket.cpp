// ======================================================================
// \title  Fw/FilePacket/GTest/DataPacket.cpp
// \author bocchino
// \brief  Test utilities for data file packets
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

    void FilePackets::DataPacket ::
      compare(
          const FilePacket::DataPacket& expected,
          const FilePacket::DataPacket& actual
      )
    {
      FilePackets::Header::compare(expected.m_header, actual.m_header);
      ASSERT_EQ(expected.m_byteOffset, actual.m_byteOffset);
      Bytes expectedData(expected.m_data, expected.m_dataSize);
      Bytes actualData(actual.m_data, actual.m_dataSize);
      Bytes::compare(expectedData, actualData);
    }

  }

}
