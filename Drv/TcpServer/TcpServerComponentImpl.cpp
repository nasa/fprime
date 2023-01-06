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
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"

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

TcpServerComponentImpl::~TcpServerComponentImpl() {}

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
    Drv::RecvStatus recvStatus = (status == SOCK_SUCCESS) ? RecvStatus::RECV_OK : RecvStatus::RECV_ERROR;
    this->recv_out(0, buffer, recvStatus);
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

Drv::PollStatus TcpServerComponentImpl::poll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(0); // It is an error to call this handler on IP drivers
    return PollStatus::POLL_ERROR;
}

}  // end namespace Drv
