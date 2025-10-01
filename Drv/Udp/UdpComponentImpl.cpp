// ======================================================================
// \title  UdpComponentImpl.cpp
// \author mstarch
// \brief  cpp file for UdpComponentImpl component implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/Udp/UdpComponentImpl.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <config/IpCfg.hpp>
#include <limits>
#include "Fw/Types/Assert.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

UdpComponentImpl::UdpComponentImpl(const char* const compName) : UdpComponentBase(compName) {}

SocketIpStatus UdpComponentImpl::configureSend(const char* hostname,
                                               const U16 port,
                                               const U32 send_timeout_seconds,
                                               const U32 send_timeout_microseconds) {
    return m_socket.configureSend(hostname, port, send_timeout_seconds, send_timeout_microseconds);
}

SocketIpStatus UdpComponentImpl::configureRecv(const char* hostname, const U16 port, FwSizeType buffer_size) {
    m_allocation_size = buffer_size;  // Store the buffer size
    return m_socket.configureRecv(hostname, port);
}

UdpComponentImpl::~UdpComponentImpl() {}

U16 UdpComponentImpl::getRecvPort() {
    return this->m_socket.getRecvPort();
}

// ----------------------------------------------------------------------
// Implementations for socket read task virtual methods
// ----------------------------------------------------------------------

IpSocket& UdpComponentImpl::getSocketHandler() {
    return m_socket;
}

Fw::Buffer UdpComponentImpl::getBuffer() {
    return allocate_out(0, m_allocation_size);
}

void UdpComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
    Drv::ByteStreamStatus recvStatus = ByteStreamStatus::OTHER_ERROR;
    if (status == SOCK_SUCCESS) {
        recvStatus = ByteStreamStatus::OP_OK;
    } else if (status == SOCK_NO_DATA_AVAILABLE) {
        recvStatus = ByteStreamStatus::RECV_NO_DATA;
    } else {
        recvStatus = ByteStreamStatus::OTHER_ERROR;
    }
    this->recv_out(0, buffer, recvStatus);
}

void UdpComponentImpl::connected() {
    if (isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::ByteStreamStatus UdpComponentImpl::send_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    Drv::SocketIpStatus status = send(fwBuffer.getData(), fwBuffer.getSize());
    Drv::ByteStreamStatus returnStatus;
    switch (status) {
        case SOCK_INTERRUPTED_TRY_AGAIN:
            returnStatus = ByteStreamStatus::SEND_RETRY;
            break;
        case SOCK_DISCONNECTED:
            returnStatus = ByteStreamStatus::SEND_RETRY;
            break;
        case SOCK_SUCCESS:
            returnStatus = ByteStreamStatus::OP_OK;
            break;
        default:
            returnStatus = ByteStreamStatus::OTHER_ERROR;
            break;
    }
    return returnStatus;
}

void UdpComponentImpl::recvReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->deallocate_out(0, fwBuffer);
}

}  // end namespace Drv
