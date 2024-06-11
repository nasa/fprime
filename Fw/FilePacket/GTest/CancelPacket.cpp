// ======================================================================
// \title  Fw/FilePacket/GTest/CancelPacket.cpp
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

namespace Fw {

  namespace GTest {

    void FilePackets::CancelPacket ::
      compare(
          const FilePacket::CancelPacket& expected,
          const FilePacket::CancelPacket& actual
      )
    {
      FilePackets::Header::compare(expected.m_header, actual.m_header);
    }

  }

}
