// ======================================================================
// \title  TcpServerComponentImpl.cpp
// \author mstarch
// \brief  cpp file for TcpServerComponentImpl component implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/TcpServer/TcpServerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TcpServerComponentImpl::TcpServerComponentImpl(const char* const compName)
    : ByteStreamDriverModelComponentBase(compName),
      SocketReadTask() {}

void TcpServerComponentImpl::init(const NATIVE_INT_TYPE instance) {
    ByteStreamDriverModelComponentBase::init(instance);
}

SocketIpStatus TcpServerComponentImpl::configure(const char* hostname,
                                                 const U16 port,
                                                 const U32 send_timeout_seconds,
                                                 const U32 send_timeout_microseconds) {
    return m_socket.configure(hostname, port, send_timeout_seconds, send_timeout_microseconds);
}

TcpServerComponentImpl::~TcpServerComponentImpl(void) {}

SocketIpStatus TcpServerComponentImpl::startup() {
    return  this->m_socket.startup();
}

void TcpServerComponentImpl::shutdown() {
    this->m_socket.shutdown();
}

// ----------------------------------------------------------------------
// Implementations for socket read task virtual methods
// ----------------------------------------------------------------------

IpSocket& TcpServerComponentImpl::getSocketHandler() {
    return m_socket;
}

Fw::Buffer TcpServerComponentImpl::getBuffer() {
    return allocate_out(0, 1024);
}

void TcpServerComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
    this->recv_out(0, buffer, (status == SOCK_SUCCESS) ? RECV_OK : RECV_ERROR);
}

void TcpServerComponentImpl::connected() {
    if (isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }

}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus TcpServerComponentImpl::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    Drv::SocketIpStatus status = m_socket.send(fwBuffer.getData(), fwBuffer.getSize());
    // Always return the buffer
    deallocate_out(0, fwBuffer);
    if ((status == SOCK_DISCONNECTED) || (status == SOCK_INTERRUPTED_TRY_AGAIN)) {
        return SEND_RETRY;
    } else if (status != SOCK_SUCCESS) {
        return SEND_ERROR;
    }
    return SEND_OK;
}

Drv::PollStatus TcpServerComponentImpl::poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(0); // It is an error to call this handler on IP drivers
    return Drv::POLL_ERROR;
}

}  // end namespace Drv
