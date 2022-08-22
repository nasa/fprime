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

#include <Svc/Framer/Framer.hpp>
#include "Fw/Logger/Logger.hpp"
#include <FpConfig.hpp>
#include "Utils/Hash/Hash.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Framer ::Framer(const char* const compName) :
  FramerComponentBase(compName),
  FramingProtocolInterface(),
  m_protocol(nullptr)
{

}

void Framer ::init(const NATIVE_INT_TYPE instance) {
    FramerComponentBase::init(instance);
}

Framer ::~Framer() {}

void Framer ::setup(FramingProtocol& protocol) {
    FW_ASSERT(m_protocol == nullptr);
    m_protocol = &protocol;
    protocol.setup(*this);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Framer ::comIn_handler(
    const NATIVE_INT_TYPE portNum,
    Fw::ComBuffer& data,
    U32 context
) {
    FW_ASSERT(m_protocol != nullptr);
    m_protocol->frame(
        data.getBuffAddr(),
        data.getBuffLength(),
        Fw::ComPacket::FW_PACKET_UNKNOWN
    );
}

void Framer ::bufferIn_handler(
    const NATIVE_INT_TYPE portNum,
    Fw::Buffer& fwBuffer
) {
    FW_ASSERT(m_protocol != nullptr);
    m_protocol->frame(
        fwBuffer.getData(),
        fwBuffer.getSize(),
        Fw::ComPacket::FW_PACKET_FILE
    );
    bufferDeallocate_out(0, fwBuffer);
}

void Framer ::send(Fw::Buffer& outgoing) {
    const Drv::SendStatus sendStatus = framedOut_out(0, outgoing);
    if (sendStatus.e != Drv::SendStatus::SEND_OK) {
        // Note: if there is a data sending problem, an EVR likely wouldn't 
        // make it down. Log the issue in hopes that
        // someone will see it.
        Fw::Logger::logMsg(
            "[ERROR] Failed to send framed data: %d\n",
            sendStatus.e
        );
    }
}

Fw::Buffer Framer ::allocate(const U32 size) {
    return framedAllocate_out(0, size);
}

}  // end namespace Svc
