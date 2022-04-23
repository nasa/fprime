// ======================================================================
// \title  UplinkFrame.cpp
// \author mstarch, bocchino
// \brief  Implementation file for Tester::UplinkFrame
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Tester.hpp"

namespace Svc {

    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------
    
    Tester::UplinkFrame::UplinkFrame(
        Fw::ComPacket::ComPacketType packetType,
        U32 packetSize
    ) :
        packetType(packetType),
        packetSize(packetSize),
        copyOffset(0),
        valid(false)
    {
        // Fill in random data
        for (U32 i = 0; i < sizeof data; ++i) {
            data[i] = STest::Pick::lowerUpper(0, 0xFF);
        }
        // Update the frame header
        this->updateHeader();
        // Update the hash value
        this->updateHash();
        // Check validity of packet type and size
        if (
            (packetType == Fw::ComPacket::FW_PACKET_COMMAND) &&
            (packetSize <= getMaxValidCommandPacketSize())
        ) {
            valid = true;
        }
        if (
            (packetType == Fw::ComPacket::FW_PACKET_FILE) &&
            (packetSize <= getMaxValidFilePacketSize())
        ) {
            valid = true;
        }
    }

    // ----------------------------------------------------------------------
    // Public instance methods 
    // ----------------------------------------------------------------------

    U32 Tester::UplinkFrame::getSize() const {
        return NON_PACKET_SIZE + packetSize;
    }

    U32 Tester::UplinkFrame::getRemainingCopySize() const {
        const U32 frameSize = getSize();
        FW_ASSERT(frameSize >= copyOffset, frameSize, copyOffset);
        return frameSize - copyOffset;
    }

    bool Tester::UplinkFrame::isValid() const {
        return valid;
    }

    void Tester::UplinkFrame::copyDataOut(
        Fw::SerialBuffer& serialBuffer,
        U32 size
    ) {
        ASSERT_LE(copyOffset + size, getSize());
        const Fw::SerializeStatus status = serialBuffer.pushBytes(
            &data[copyOffset],
            size
        );
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        copyOffset += size;
    }

    const Tester::UplinkFrame::FrameData&
        Tester::UplinkFrame::getData() const
    {
        return data;
    }

}
