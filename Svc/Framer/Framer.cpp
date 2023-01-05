// ======================================================================
// \title  Framer.cpp
// \author mstarch
// \brief  cpp file for Framer component implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <FpConfig.hpp>
#include <Svc/Framer/Framer.hpp>
#include "Fw/Logger/Logger.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Framer ::Framer(const char* const compName)
    : FramerComponentBase(compName), FramingProtocolInterface(), m_protocol(nullptr), m_frame_sent(false) {}

void Framer ::init(const NATIVE_INT_TYPE instance) {
    FramerComponentBase::init(instance);
}

Framer ::~Framer() {}

void Framer ::setup(FramingProtocol& protocol) {
    FW_ASSERT(this->m_protocol == nullptr);
    this->m_protocol = &protocol;
    protocol.setup(*this);
}

void Framer ::handle_framing(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) {
    FW_ASSERT(this->m_protocol != nullptr);
    this->m_frame_sent = false;  // Clear the flag to detect if frame was sent
    this->m_protocol->frame(data, size, packet_type);
    // If no frame was sent, Framer has the obligation to report success
    if (this->isConnected_comStatusOut_OutputPort(0) && (!this->m_frame_sent)) {
        Fw::Success status = Fw::Success::SUCCESS;
        this->comStatusOut_out(0, status);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Framer ::comIn_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->handle_framing(data.getBuffAddr(), data.getBuffLength(), Fw::ComPacket::FW_PACKET_UNKNOWN);
}

void Framer ::bufferIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->handle_framing(fwBuffer.getData(), fwBuffer.getSize(), Fw::ComPacket::FW_PACKET_FILE);
    // Deallocate the buffer after it was processed by the framing protocol
    this->bufferDeallocate_out(0, fwBuffer);
}

void Framer ::comStatusIn_handler(const NATIVE_INT_TYPE portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

// ----------------------------------------------------------------------
// Framing protocol implementations
// ----------------------------------------------------------------------

void Framer ::send(Fw::Buffer& outgoing) {
    FW_ASSERT(!this->m_frame_sent); // Prevent multiple sends per-packet
    const Drv::SendStatus sendStatus = this->framedOut_out(0, outgoing);
    if (sendStatus.e != Drv::SendStatus::SEND_OK) {
        // Note: if there is a data sending problem, an EVR likely wouldn't
        // make it down. Log the issue in hopes that
        // someone will see it.
        Fw::Logger::logMsg("[ERROR] Failed to send framed data: %d\n", sendStatus.e);
    }
    this->m_frame_sent = true;  // A frame was sent
}

Fw::Buffer Framer ::allocate(const U32 size) {
    return this->framedAllocate_out(0, size);
}

}  // end namespace Svc
