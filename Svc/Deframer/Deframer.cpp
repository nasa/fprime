// ======================================================================
// \title  Deframer.cpp
// \author mstarch
// \brief  cpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Logger/Logger.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Svc/Deframer/Deframer.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Deframer ::Deframer(const char* const compName) : 
    DeframerComponentBase(compName), 
    DeframingProtocolInterface(),
    m_protocol(nullptr),
    m_in_ring(m_ring_buffer, sizeof(m_ring_buffer))
{

}

void Deframer ::init(const NATIVE_INT_TYPE instance) {
    DeframerComponentBase::init(instance);
}

Deframer ::~Deframer() {}

void Deframer ::setup(DeframingProtocol& protocol) {
    // Check that this is the first time we are calling setup
    FW_ASSERT(m_protocol == nullptr);
    // Assign the protocol passed in to m_protocol
    m_protocol = &protocol;
    // Pass *this as the DeframingProtocolInstance to protocol setup
    // Deframer is derived from and implements DeframingProtocolInterface
    protocol.setup(*this);
}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Deframer ::cmdResponseIn_handler(
    NATIVE_INT_TYPE portNum,
    FwOpcodeType opcode,
    U32 cmdSeq,
    const Fw::CmdResponse& response
) {
    // Nothing to do
}

void Deframer ::framedIn_handler(
    const NATIVE_INT_TYPE portNum,
    Fw::Buffer& recvBuffer,
    const Drv::RecvStatus& recvStatus
) {
    // Check whether there is data to process
    if (Drv::RecvStatus::RECV_OK == recvStatus.e) {
        // There is: process the data
        processBuffer(recvBuffer);
    }
    // Deallocate the buffer
    framedDeallocate_out(0, recvBuffer);
}

void Deframer ::schedIn_handler(
    const NATIVE_INT_TYPE portNum,
    NATIVE_UINT_TYPE context
) {
    // Check for data
    Fw::Buffer buffer(m_poll_buffer, sizeof(m_poll_buffer));
    auto status = framedPoll_out(0, buffer);
    if (status == Drv::PollStatus::POLL_OK) {
        // Data exists: process it
        processBuffer(buffer);
    }
}

Fw::Buffer Deframer ::allocate(const U32 size)  {
    return bufferAllocate_out(0, size);
}

void Deframer ::route(Fw::Buffer& packetBuffer) {

    // Read the packet type from the packet buffer
    FwPacketDescriptorType packetType = Fw::ComPacket::FW_PACKET_UNKNOWN;
    auto status = Fw::FW_SERIALIZE_OK;
    {
        auto& serial = packetBuffer.getSerializeRepr();
        serial.setBuffLen(packetBuffer.getSize());
        status = serial.deserialize(packetType);
    }

    // Whethter to deallocate the packet buffer
    bool deallocate = true;

    // Process the packet
    if (status == Fw::FW_SERIALIZE_OK) {
        U8* packetData = packetBuffer.getData();
        const auto packetSize = packetBuffer.getSize();
        switch (packetType) {
            case Fw::ComPacket::FW_PACKET_COMMAND: {
                // Allocate a com buffer on the stack
                Fw::ComBuffer com;
                // Copy the contents of the packet buffer into the com buffer
                com.setBuff(packetData, packetSize);
                // Send the com buffer
                comOut_out(0, com, 0);
                break;
            }
            case Fw::ComPacket::FW_PACKET_FILE: {
                // If file uplink is possible, handle the file packet.
                // Otherwise ignore it.
                if (isConnected_bufferOut_OutputPort(0)) {
                    // Shift the packet buffer to skip the packet type
                    // The FileUplink component does not expect the packet
                    // type to be there.
                    packetBuffer.setData(packetData + sizeof(packetType));
                    packetBuffer.setSize(packetSize - sizeof(packetType));
                    // Send the packet buffer
                    bufferOut_out(0, packetBuffer);
                    // Transfer ownership of the buffer to the receiver
                    deallocate = false;
                }
                break;
            }
            default:
                break;
        }
    }

    if (deallocate) {
        // Deallocate the packet buffer
        bufferDeallocate_out(0, packetBuffer);
    }

}

void Deframer ::processRing() {
    FW_ASSERT(m_protocol != nullptr);
    // Maximum limit to the loop as at least one byte is process per iteration unless needed > remaining size
    const U32 loop_limit = m_in_ring.get_capacity() + 1;

    // Inner-loop, process ring buffer looking for at least the header
    U32 i = 0;
    for (i = 0; (m_in_ring.get_allocated_size() > 0) and (i < loop_limit); i++) {
        const U32 remaining = m_in_ring.get_allocated_size();
        U32 needed = 0; // Needed is an out-only variable, and should be reset each loop
        DeframingProtocol::DeframingStatus status = m_protocol->deframe(m_in_ring, needed);
        FW_ASSERT(needed != 0); //Deframing protocol must always set needed to a non-zero value
        FW_ASSERT(remaining == m_in_ring.get_allocated_size()); // Deframing protocol must not consume data only view it
        // Successful deframing consumes messages
        if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
            m_in_ring.rotate(needed);
        }
        // Break on the condition that more is needed
        else if (status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
            // Deframing protocol reported inconsistent "more is needed" and needed size
            FW_ASSERT(needed > m_in_ring.get_allocated_size(), needed, m_in_ring.get_allocated_size());
            break;
        }
        // Error statuses  reset needed and rotate away 1 byte
        else {
            m_in_ring.rotate(1);
            // Checksum errors get logged as it is unlikely to get to a checksum check on random data
            if (status == DeframingProtocol::DEFRAMING_INVALID_CHECKSUM) {
                Fw::Logger::logMsg("[ERROR] Deframing checksum validation failed\n");
            }
        }
    }
    // In every iteration of the loop above data is removed from the buffer, or we break from the loop due. Thus at
    // worst case the loop should be called m_in_ring.get_capacity() times before exiting due an empty buffer. If it hits
    // the limit, something went horribly wrong.
    FW_ASSERT(i < loop_limit);
}

void Deframer ::processBuffer(Fw::Buffer& buffer) {
    U32 i = 0;
    U32 buffer_offset = 0; // Max buffer size is U32
    // Note: max iteration bounded by processing 1 byte per iteration
    for (i = 0; (i < (buffer.getSize() + 1)) and (buffer_offset < buffer.getSize()); i++) {
        U32 remaining = buffer.getSize() - buffer_offset;
        NATIVE_UINT_TYPE ser_size = (remaining >= m_in_ring.get_free_size())
                                        ? m_in_ring.get_free_size()
                                        : static_cast<NATIVE_UINT_TYPE>(remaining);
        m_in_ring.serialize(buffer.getData() + buffer_offset, ser_size);
        // TODO: Assert here that serialization succeeded
        // Then we can remove the +1 in the loop test and the assertion at the end
        buffer_offset = buffer_offset + ser_size;
        processRing();
    }
    // Note: this assert can trip when the processRing function failed to process data
    // or when the circular buffer is not large enough to hold a complete message. Both
    // are hard-failures
    FW_ASSERT(i <= buffer.getSize(), buffer.getSize());
}

}  // end namespace Svc
