// ======================================================================
// \title  DeframerComponentImpl.cpp
// \author mstarch
// \brief  cpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/Deframer/DeframerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

DeframerComponentImpl ::DeframerComponentImpl(const char* const compName) : DeframerComponentBase(compName), DeframingProtocolInterface(),
    m_protocol(NULL), m_in_ring(m_ring_buffer, sizeof(m_ring_buffer))
{}

void DeframerComponentImpl ::init(const NATIVE_INT_TYPE instance) {
    DeframerComponentBase::init(instance);
}

DeframerComponentImpl ::~DeframerComponentImpl(void) {}

void DeframerComponentImpl ::setup(DeframingProtocol& protocol) {
    FW_ASSERT(m_protocol == NULL);
    m_protocol = &protocol;
    protocol.setup(*this);
}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DeframerComponentImpl ::framedIn_handler(const NATIVE_INT_TYPE portNum,
                                              Fw::Buffer& recvBuffer,
                                              Drv::RecvStatus recvStatus) {
    if (Drv::RECV_OK == recvStatus) {
        processBuffer(recvBuffer);
    }
    framedDeallocate_out(0, recvBuffer);
}

void DeframerComponentImpl ::schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    Fw::Buffer buffer(m_poll_buffer, sizeof(m_poll_buffer));
    // Call read poll if it is hooked up
    if (isConnected_framedPoll_OutputPort(0)) {
        Drv::PollStatus status = framedPoll_out(0, buffer);
        if (status == Drv::POLL_OK) {
            processBuffer(buffer);
        }
    }
}

Fw::Buffer DeframerComponentImpl ::allocate(const U32 size)  {
    return bufferAllocate_out(0, size);
}

void DeframerComponentImpl ::route(Fw::Buffer& data) {
    // Read the packet type from the data buffer
    I32 packet_type = Fw::ComPacket::FW_PACKET_UNKNOWN;
    Fw::SerializeBufferBase& serial = data.getSerializeRepr();
    serial.setBuffLen(data.getSize());
    serial.deserialize(packet_type);

    // Process variable type
    switch (packet_type) {
        case Fw::ComPacket::FW_PACKET_COMMAND: {
            Fw::ComBuffer com;
            com.setBuff(data.getData(), data.getSize());
            comOut_out(0, com, 0);
            // Return buffer immediately as cmdDisp will not return buffers for us. Com buffers copy, so this is safe!
            bufferDeallocate_out(0, data);
            break;
        }
        case Fw::ComPacket::FW_PACKET_FILE: {
            // If file uplink is possible, handle files.  Otherwise ignore.
            if (isConnected_bufferAllocate_OutputPort(0)) {
                // Shift the buffer to ignore the packet type
                data.setData(data.getData() + sizeof(I32));
                data.setSize(data.getSize() - sizeof(I32));
                bufferOut_out(0, data);
            }
            break;
        }
        default:
            return;
    }
}


void DeframerComponentImpl ::processRing() {
    FW_ASSERT(m_protocol != NULL);
    // Inner-loop, process ring buffer looking for at least the header
    U32 needed = 0;
    while (m_in_ring.get_remaining_size() >= needed) {
        DeframingProtocol::DeframingStatus status = m_protocol->deframe(m_in_ring, needed);
        // Successful deframing consumes messages
        if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
            m_in_ring.rotate(needed);
        }
        // Error statuses  reset needed and rotate away 1 byte
        else if (status != DeframingProtocol::DEFRAMING_MORE_NEEDED) {
            m_in_ring.rotate(1);
            needed = 0;
            // Checksum errors get logged as it is unlinkly to get to a checksum check on random data
            if (status == DeframingProtocol::DEFRAMING_INVALID_CHECKSUM) {
                Fw::Logger::logMsg("[ERROR] Deframing checksum validation failed\n");
            }
        }
    }
}

void DeframerComponentImpl ::processBuffer(Fw::Buffer& buffer) {
    NATIVE_UINT_TYPE buffer_offset = 0;
    while (buffer_offset < buffer.getSize()) {
        NATIVE_UINT_TYPE ser_size = (buffer.getSize() >= m_in_ring.get_remaining_size(true))
                                        ? m_in_ring.get_remaining_size(true)
                                        : static_cast<NATIVE_UINT_TYPE>(buffer.getSize());
        m_in_ring.serialize(buffer.getData() + buffer_offset, ser_size);
        buffer_offset = buffer_offset + ser_size;
        processRing();
    }
}

}  // end namespace Svc
