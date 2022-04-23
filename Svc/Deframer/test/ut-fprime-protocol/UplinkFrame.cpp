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

    // ----------------------------------------------------------------------
    // Public static methods 
    // ----------------------------------------------------------------------

    U32 Tester::UplinkFrame::getMinPacketSize() {
        // Packet must hold the packet type
        return sizeof(FwPacketDescriptorType);
    }

    U32 Tester::UplinkFrame::getInvalidPacketSize() {
        FW_ASSERT(
            MAX_FRAME_SIZE >= NON_PACKET_SIZE,
            MAX_FRAME_SIZE,
            NON_PACKET_SIZE
        );
        const U32 result = MAX_FRAME_SIZE - NON_PACKET_SIZE;
        // Make sure the size is invalid
        FW_ASSERT(
            result > getMaxValidCommandPacketSize(),
            result,
            getMaxValidCommandPacketSize()
        );
        FW_ASSERT(
            result > getMaxValidFilePacketSize(),
            result,
            getMaxValidFilePacketSize()
        );
        return result;
    }

    U32 Tester::UplinkFrame::getMaxValidFilePacketSize() {
        FW_ASSERT(
            MAX_VALID_FRAME_SIZE >= NON_PACKET_SIZE,
            MAX_VALID_FRAME_SIZE,
            NON_PACKET_SIZE
        );
        return MAX_VALID_FRAME_SIZE - NON_PACKET_SIZE;
    }

    U32 Tester::UplinkFrame::getMaxValidCommandPacketSize() {
        return std::min(
            // Packet must fit into a com buffer
            static_cast<U32>(FW_COM_BUFFER_MAX_SIZE),
            // Frame must fit into the ring buffer
            getMaxValidFilePacketSize()
        );
    }

    Tester::UplinkFrame Tester::UplinkFrame::random() {
        // Randomly set the packet type
        Fw::ComPacket::ComPacketType packetType =
            Fw::ComPacket::FW_PACKET_UNKNOWN;
        const U32 packetSelector = STest::Pick::lowerUpper(0,1);
        U32 maxValidPacketSize = 0;
        switch (packetSelector) {
            case 0:
                packetType = Fw::ComPacket::FW_PACKET_COMMAND;
                maxValidPacketSize = getMaxValidCommandPacketSize();
                break;
            case 1:
                packetType = Fw::ComPacket::FW_PACKET_FILE;
                maxValidPacketSize = getMaxValidFilePacketSize();
                break;
            default:
                FW_ASSERT(0);
                break;
        }
        // Randomly set the packet size
        U32 packetSize = 0;
        // Invalidate 1 in 100 packet sizes
        const U32 invalidSizeIndex = STest::Pick::startLength(0, 100);
        if (invalidSizeIndex == 0) {
            // This packet size fits in the test buffer,
            // but is invalid for the deframer
            packetSize = getInvalidPacketSize();
        }
        else {
            // Choose a valid packet size
            // This packet size fits in the test buffer and is
            // valid for the deframer
            packetSize = STest::Pick::lowerUpper(
                getMinPacketSize(),
                maxValidPacketSize
            );
        }
        // Construct the frame
        UplinkFrame frame = UplinkFrame(packetType, packetSize);
        // Randomly invalidate the frame, or leave it alone
        frame.randomlyInvalidate();
        // Return the frame
        return frame;
    }

}
