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
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"


namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TcpClientComponentImpl::TcpClientComponentImpl(const char* const compName)
    : ByteStreamDriverModelComponentBase(compName),
      SocketReadTask() {}

void TcpClientComponentImpl::init(const NATIVE_INT_TYPE instance) {
    ByteStreamDriverModelComponentBase::init(instance);
}

SocketIpStatus TcpClientComponentImpl::configure(const char* hostname,
                                                 const U16 port,
                                                 const U32 send_timeout_seconds,
                                                 const U32 send_timeout_microseconds) {
    return m_socket.configure(hostname, port, send_timeout_seconds, send_timeout_microseconds);
}

TcpClientComponentImpl::~TcpClientComponentImpl() {}

// ----------------------------------------------------------------------
// Implementations for socket read task virtual methods
// ----------------------------------------------------------------------

IpSocket& TcpClientComponentImpl::getSocketHandler() {
    return m_socket;
}

Fw::Buffer TcpClientComponentImpl::getBuffer() {
    return allocate_out(0, 1024);
}

void TcpClientComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
    Drv::RecvStatus recvStatus = (status == SOCK_SUCCESS) ? RecvStatus::RECV_OK : RecvStatus::RECV_ERROR;
    this->recv_out(0, buffer, recvStatus);
}

void TcpClientComponentImpl::connected() {
    if (isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }

}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus TcpClientComponentImpl::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    Drv::SocketIpStatus status = m_socket.send(fwBuffer.getData(), fwBuffer.getSize());
    // Only deallocate buffer when the caller is not asked to retry
    if (status == SOCK_INTERRUPTED_TRY_AGAIN) {
        return SendStatus::SEND_RETRY;
    } else if (status != SOCK_SUCCESS) {
        deallocate_out(0, fwBuffer);
        return SendStatus::SEND_ERROR;
    }
    deallocate_out(0, fwBuffer);
    return SendStatus::SEND_OK;
}

Drv::PollStatus TcpClientComponentImpl::poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(0); // It is an error to call this handler on IP drivers
    return PollStatus::POLL_ERROR;
}

}  // end namespace Drv
