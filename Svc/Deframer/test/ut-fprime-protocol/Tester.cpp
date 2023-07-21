// ======================================================================
// \title  Tester.cpp
// \brief  Implementation file for Deframer test with F Prime protocol
// \author mstarch, bocchino
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <cstring>
#include <limits>

#include "Fw/Types/Assert.hpp"
#include "Tester.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Hash/HashBuffer.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10000

namespace Svc {

    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    Tester ::Tester()
        : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
          component("Deframer")
      {
        this->initComponents();
        this->connectPorts();
        component.setup(protocol);
        memset(m_incomingBufferBytes, 0, sizeof m_incomingBufferBytes);
      }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void Tester ::sizeOverflow() {
        U8 data[FpFrameHeader::SIZE];
        Fw::Buffer buffer(data, sizeof data);
        Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
        Fw::SerializeStatus status = serialRepr.serialize(FpFrameHeader::START_WORD);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        FpFrameHeader::TokenType size = std::numeric_limits<U32>::max();
        status = serialRepr.serialize(size);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        this->component.processBuffer(buffer);
        // Assert no output
        ASSERT_FROM_PORT_HISTORY_SIZE(0);
    }

    // ----------------------------------------------------------------------
    // Public instance methods
    // ----------------------------------------------------------------------

    void Tester ::setUpIncomingBuffer() {
        const U32 bufferSize = STest::Pick::lowerUpper(
            1,
            sizeof m_incomingBufferBytes
        );
        ASSERT_LE(bufferSize, sizeof m_incomingBufferBytes);
        m_incomingBuffer = Fw::Buffer(
            m_incomingBufferBytes,
            bufferSize
        );
    }

    void Tester ::sendIncomingBuffer() {
        // Push buffer to framedIn
        invoke_to_framedIn(
            0,
            m_incomingBuffer,
            Drv::RecvStatus::RECV_OK
        );
    }

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    void Tester ::from_deframedOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer,
        Fw::Buffer& context
    ) {
        // Check that a received frame is expected
        ASSERT_GT(m_framesToReceive.size(), 0) <<
            "Queue of frames to receive is empty" << std::endl;
        // Get the frame at the front
        UplinkFrame frame = m_framesToReceive.front();
        m_framesToReceive.pop_front();
        // Check the packet data
        for (U32 i = 0; i < fwBuffer.getSize(); i++) {
            const U32 frameOffset = FpFrameHeader::SIZE + i;
            // Deframer strips type before sending to FileUplink
            ASSERT_EQ(
                (fwBuffer.getData())[i],
                (frame.getData())[frameOffset]
            );
        }
        // Buffers received on this port are owned by the receiver
        // So delete the allocation now
        // Before deallocating, undo the deframer's adjustment of the pointer
        // by the size of the packet type
        delete[](fwBuffer.getData());
        // Push the history entry
        this->pushFromPortEntry_deframedOut(fwBuffer, context);
    }

    Fw::Buffer Tester ::from_bufferAllocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    ) {
        this->pushFromPortEntry_bufferAllocate(size);
        U8 *const data = new U8[size];
        memset(data, 0, size);
        Fw::Buffer buffer(data, size);
        return buffer;
    }

    void Tester ::from_framedDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    ) {
        this->pushFromPortEntry_framedDeallocate(fwBuffer);
    }
}
