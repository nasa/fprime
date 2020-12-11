// ======================================================================
// \title  TcpClientComponentImpl.cpp
// \author mstarch
// \brief  cpp file for TcpClientComponentImpl component implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/TcpClient/TcpClientComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"


namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TcpClientComponentImpl::TcpClientComponentImpl(const char* const compName)
    : ByteStreamDriverModelComponentBase(compName),
      SocketReadTask(),
      m_buffer(m_backing_data, sizeof(m_backing_data)) {}

void TcpClientComponentImpl::init(const NATIVE_INT_TYPE instance) {
    ByteStreamDriverModelComponentBase::init(instance);
}

SocketIpStatus TcpClientComponentImpl::configure(const char* hostname,
                                                 const U16 port,
                                                 const U32 send_timeout_seconds,
                                                 const U32 send_timeout_microseconds) {
    return m_socket.configure(hostname, port, send_timeout_seconds, send_timeout_microseconds);
}

TcpClientComponentImpl::~TcpClientComponentImpl(void) {}

// ----------------------------------------------------------------------
// Implementations for socket read task virtual methods
// ----------------------------------------------------------------------

IpSocket& TcpClientComponentImpl::getSocketHandler() {
    return m_socket;
}

Fw::Buffer TcpClientComponentImpl::getBuffer() {
    return m_buffer;
}

void TcpClientComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
    if (status == SOCK_SUCCESS) {
        this->recv_out(0, buffer, RECV_OK);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus TcpClientComponentImpl::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    Drv::SocketIpStatus status = m_socket.send(fwBuffer.getData(), fwBuffer.getSize());
    if ((status == SOCK_DISCONNECTED) || (status == SOCK_INTERRUPTED_TRY_AGAIN)) {
        return SEND_RETRY;
    } else if (status != SOCK_SUCCESS) {
        return SEND_ERROR;
    }
    return SEND_OK;
}

Drv::PollStatus TcpClientComponentImpl::poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(0); // It is an error to call this handler on IP drivers
    return Drv::POLL_ERROR;
}

}  // end namespace Drv
