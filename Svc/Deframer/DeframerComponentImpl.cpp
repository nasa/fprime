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
    m_protocol(nullptr), m_in_ring(m_ring_buffer, sizeof(m_ring_buffer))
{}

void DeframerComponentImpl ::init(const NATIVE_INT_TYPE instance) {
    DeframerComponentBase::init(instance);
}

DeframerComponentImpl ::~DeframerComponentImpl() {}

void DeframerComponentImpl ::setup(DeframingProtocol& protocol) {
    FW_ASSERT(m_protocol == nullptr);
    m_protocol = &protocol;
    protocol.setup(*this);
}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DeframerComponentImpl ::cmdResponseIn_handler(NATIVE_INT_TYPE portNum,
                                                   FwOpcodeType opcode,
                                                   U32 cmdSeq,
                                                   const Fw::CmdResponse& response) {
  // Nothing to do
}

void DeframerComponentImpl ::framedIn_handler(const NATIVE_INT_TYPE portNum,
                                              Fw::Buffer& recvBuffer,
                                              const Drv::RecvStatus& recvStatus) {
    if (Drv::RecvStatus::RECV_OK == recvStatus.e) {
        processBuffer(recvBuffer);
    }
    framedDeallocate_out(0, recvBuffer);
}

void DeframerComponentImpl ::schedIn_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    Fw::Buffer buffer(m_poll_buffer, sizeof(m_poll_buffer));
    // Call read poll if it is hooked up
    if (isConnected_framedPoll_OutputPort(0)) {
        Drv::PollStatus status = framedPoll_out(0, buffer);
        if (status == Drv::PollStatus::POLL_OK) {
            processBuffer(buffer);
        }
    }
}

Fw::Buffer DeframerComponentImpl ::allocate(const U32 size)  {
    return bufferAllocate_out(0, size);
}

void DeframerComponentImpl ::route(Fw::Buffer& data) {
    // Read the packet type from the data buffer
    I32 packet_type = static_cast<I32>(Fw::ComPacket::FW_PACKET_UNKNOWN);
    Fw::SerializeBufferBase& serial = data.getSerializeRepr();
    serial.setBuffLen(data.getSize());
    // Serialized packet type is explicitly an I32 (4 bytes)
    Fw::SerializeStatus status = serial.deserialize(packet_type);
    if (status != Fw::FW_SERIALIZE_OK) {
        // In the case that the deserialize was unsuccessful we should deallocate the request
        bufferDeallocate_out(0, data);
        return;
    }

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
            if (isConnected_bufferOut_OutputPort(0)) {
                // Shift the buffer to ignore the packet type
                data.setData(data.getData() + sizeof(packet_type));
                data.setSize(data.getSize() - sizeof(packet_type));
                bufferOut_out(0, data);
            }
            break;
        }
        default:
            // In the case that we do not know the packet type, we should deallocate the request
            bufferDeallocate_out(0, data);
            return;
    }
}

void DeframerComponentImpl ::processRing() {
    FW_ASSERT(m_protocol != nullptr);
    // Maximum limit to the loop as at least one byte is process per iteration unless needed > remaining size
    const U32 loop_limit = m_in_ring.get_capacity() + 1;

    // Inner-loop, process ring buffer looking for at least the header
    U32 i = 0;
    for (i = 0; (m_in_ring.get_remaining_size() > 0) and (i < loop_limit); i++) {
        const U32 remaining = m_in_ring.get_remaining_size();
        U32 needed = 0; // Needed is an out-only variable, and should be reset each loop
        DeframingProtocol::DeframingStatus status = m_protocol->deframe(m_in_ring, needed);
        FW_ASSERT(needed != 0); //Deframing protocol must always set needed to a non-zero value
        FW_ASSERT(remaining == m_in_ring.get_remaining_size()); // Deframing protocol must not consume data only view it
        // Successful deframing consumes messages
        if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
            m_in_ring.rotate(needed);
        }
        // Break on the condition that more is needed
        else if (status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
            // Deframing protocol reported inconsistent "more is needed" and needed size
            FW_ASSERT(needed > m_in_ring.get_remaining_size(), needed, m_in_ring.get_remaining_size());
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

void DeframerComponentImpl ::processBuffer(Fw::Buffer& buffer) {
    U32 i = 0;
    U32 buffer_offset = 0; // Max buffer size is U32
    // Note: max iteration bounded by processing 1 byte per iteration
    for (i = 0; (i < (buffer.getSize() + 1)) and (buffer_offset < buffer.getSize()); i++) {
        U32 remaining = buffer.getSize() - buffer_offset;
        NATIVE_UINT_TYPE ser_size = (remaining >= m_in_ring.get_remaining_size(true))
                                        ? m_in_ring.get_remaining_size(true)
                                        : static_cast<NATIVE_UINT_TYPE>(remaining);
        m_in_ring.serialize(buffer.getData() + buffer_offset, ser_size);
        buffer_offset = buffer_offset + ser_size;
        processRing();
    }
    // Note: this assert can trip when the processRing function failed to process data
    // or when the circular buffer is not large enough to hold a complete message. Both
    // are hard-failures
    FW_ASSERT(i <= buffer.getSize(), buffer.getSize());
}

}  // end namespace Svc
