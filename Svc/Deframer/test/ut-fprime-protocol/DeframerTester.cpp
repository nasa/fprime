// ======================================================================
// \title  DeframerTester.cpp
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
#include "DeframerTester.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Hash/HashBuffer.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10000

namespace Svc {

    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    DeframerTester ::DeframerTester(InputMode::t inputMode)
        : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
          component("Deframer"),
          m_inputMode(inputMode)
      {
        this->initComponents();
        this->connectPorts();
        component.setup(protocol);
        memset(m_incomingBufferBytes, 0, sizeof m_incomingBufferBytes);
      }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void DeframerTester ::sizeOverflow() {
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

    void DeframerTester ::setUpIncomingBuffer() {
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

    void DeframerTester ::sendIncomingBuffer() {
        switch (m_inputMode) {
            case InputMode::PUSH:
                // Push buffer to framedIn
                invoke_to_framedIn(
                    0,
                    m_incomingBuffer,
                    Drv::RecvStatus::RECV_OK
                );
                break;
            case InputMode::POLL:
                // Call schedIn handler, which polls for buffer
                invoke_to_schedIn(0, 0);
                break;
            default:
                FW_ASSERT(0);
                break;
        }
    }

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    void DeframerTester ::from_comOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer& data,
        U32 context
    ) {
        // Check that a received frame is expected
        ASSERT_GT(m_framesToReceive.size(), 0) <<
            "Queue of frames to receive is empty" << std::endl;
        // Get the frame at the front
        UplinkFrame frame = m_framesToReceive.front();
        m_framesToReceive.pop_front();
        // Check the packet type
        ASSERT_EQ(frame.packetType, Fw::ComPacket::FW_PACKET_COMMAND);
        // Check the packet data
        for (U32 i = 0; i < data.getBuffLength(); i++) {
            EXPECT_EQ(
                (data.getBuffAddr())[i],
                (frame.getData())[FpFrameHeader::SIZE + i]
            );
        }
        // Push the history entry
        this->pushFromPortEntry_comOut(data, context);
    }

    void DeframerTester ::from_bufferOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    ) {
        // Check that a received frame is expected
        ASSERT_GT(m_framesToReceive.size(), 0) <<
            "Queue of frames to receive is empty" << std::endl;
        // Get the frame at the front
        UplinkFrame frame = m_framesToReceive.front();
        m_framesToReceive.pop_front();
        // Check the packet type
        ASSERT_EQ(frame.packetType, Fw::ComPacket::FW_PACKET_FILE);
        // Check the packet data
        for (U32 i = 0; i < fwBuffer.getSize(); i++) {
            // Deframer strips type before sending to FileUplink
            const U32 frameOffset =
                FpFrameHeader::SIZE + sizeof(FwPacketDescriptorType) + i;
            ASSERT_EQ(
                (fwBuffer.getData())[i],
                (frame.getData())[frameOffset]
            );
        }
        // Buffers received on this port are owned by the receiver
        // So delete the allocation now
        // Before deallocating, undo the deframer's adjustment of the pointer
        // by the size of the packet type
        delete[](fwBuffer.getData() - sizeof(FwPacketDescriptorType));
        // Push the history entry
        this->pushFromPortEntry_bufferOut(fwBuffer);
    }

    Fw::Buffer DeframerTester ::from_bufferAllocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    ) {
        this->pushFromPortEntry_bufferAllocate(size);
        U8 *const data = new U8[size];
        memset(data, 0, size);
        Fw::Buffer buffer(data, size);
        return buffer;
    }

    void DeframerTester ::from_bufferDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    ) {
        delete[] fwBuffer.getData();
        this->pushFromPortEntry_bufferDeallocate(fwBuffer);
    }

    void DeframerTester ::from_framedDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& fwBuffer
    ) {
        this->pushFromPortEntry_framedDeallocate(fwBuffer);
    }

    Drv::PollStatus DeframerTester ::from_framedPoll_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer& pollBuffer
    ) {
        this->pushFromPortEntry_framedPoll(pollBuffer);
        U8* incoming = m_incomingBuffer.getData();
        const U32 size = m_incomingBuffer.getSize();
        U8* outgoing = pollBuffer.getData();
        const U32 maxSize = pollBuffer.getSize();
        FW_ASSERT(size <= maxSize, size, maxSize);
        memcpy(outgoing, incoming, size);
        pollBuffer.setSize(size);
        return Drv::PollStatus::POLL_OK;
    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void DeframerTester ::connectPorts() {

        // bufferAllocate
        this->component.set_bufferAllocate_OutputPort(
            0,
            this->get_from_bufferAllocate(0)
        );

        // bufferDeallocate
        this->component.set_bufferDeallocate_OutputPort(
            0,
            this->get_from_bufferDeallocate(0)
        );

        // bufferOut
        this->component.set_bufferOut_OutputPort(
            0,
            this->get_from_bufferOut(0)
        );

        // cmdResponseIn
        this->connect_to_cmdResponseIn(
            0,
            this->component.get_cmdResponseIn_InputPort(0)
        );

        // comOut
        this->component.set_comOut_OutputPort(
            0,
            this->get_from_comOut(0)
        );

        // framedDeallocate
        this->component.set_framedDeallocate_OutputPort(
            0,
            this->get_from_framedDeallocate(0)
        );

        // framedIn
        this->connect_to_framedIn(
            0,
            this->component.get_framedIn_InputPort(0)
        );

        // framedPoll
        this->component.set_framedPoll_OutputPort(
            0,
            this->get_from_framedPoll(0)
        );

        // schedIn
        this->connect_to_schedIn(
            0,
            this->component.get_schedIn_InputPort(0)
        );

    }

    void DeframerTester ::initComponents() {
        this->init();
        this->component.init(INSTANCE);
    }

}
