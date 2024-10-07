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

#include <limits>
#include <Drv/TcpClient/TcpClientComponentImpl.hpp>
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"


namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TcpClientComponentImpl::TcpClientComponentImpl(const char* const compName)
    : TcpClientComponentBase(compName),
      SocketComponentHelper() {}

SocketIpStatus TcpClientComponentImpl::configure(const char* hostname,
                                                 const U16 port,
                                                 const U32 send_timeout_seconds,
                                                 const U32 send_timeout_microseconds,
                                                 FwSizeType buffer_size) {

    // Check that ensures the configured buffer size fits within the limits fixed-width type, U32                                                
    FW_ASSERT(buffer_size <= std::numeric_limits<U32>::max(), static_cast<FwAssertArgType>(buffer_size));                                                   
    m_allocation_size = buffer_size; // Store the buffer size
    (void)startup();
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
    return allocate_out(0, static_cast<U32>(m_allocation_size));
}

void TcpClientComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
    Drv::RecvStatus recvStatus = RecvStatus::RECV_ERROR;
    if (status == SOCK_SUCCESS) {
        recvStatus = RecvStatus::RECV_OK;
    }
    else if (status == SOCK_NO_DATA_AVAILABLE) {
        recvStatus = RecvStatus::RECV_NO_DATA;
    }
    else {
        recvStatus = RecvStatus::RECV_ERROR;
    }
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
    Drv::SocketIpStatus status = send(fwBuffer.getData(), fwBuffer.getSize());
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

}  // end namespace Drv
