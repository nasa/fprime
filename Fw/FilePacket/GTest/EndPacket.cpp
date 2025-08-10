// ======================================================================
// \title  Fw/FilePacket/GTest/EndPacket.cpp
// \author bocchino
// \brief  Test utilities for data file packets
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <CFDP/Checksum/GTest/Checksums.hpp>
#include <Fw/FilePacket/GTest/FilePackets.hpp>
#include <Fw/Types/GTest/Bytes.hpp>

namespace Fw {

namespace GTest {

void FilePackets::EndPacket ::compare(const FilePacket::EndPacket& expected, const FilePacket::EndPacket& actual) {
    FilePackets::Header::compare(expected.asHeader(), actual.asHeader());
    CFDP::Checksum expectedChecksum;
    CFDP::Checksum actualChecksum;
    expected.getChecksum(expectedChecksum);
    actual.getChecksum(actualChecksum);
    CFDP::GTest::Checksums::compare(expectedChecksum, actualChecksum);
}

}  // namespace GTest

}  // namespace Fw
