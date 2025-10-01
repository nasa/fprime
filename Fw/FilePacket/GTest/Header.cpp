// ======================================================================
// \title  Fw/FilePacket/GTest/Header.cpp
// \author bocchino
// \brief  Test utilities for file packet headers
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

void FilePackets::Header ::compare(const FilePacket::Header& expected, const FilePacket::Header& actual) {
    ASSERT_EQ(expected.getType(), actual.getType());
    ASSERT_EQ(expected.getSequenceIndex(), actual.getSequenceIndex());
}

}  // namespace GTest

}  // namespace Fw
