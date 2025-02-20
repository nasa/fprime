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

#include <limits>
#include <Drv/TcpServer/TcpServerComponentImpl.hpp>
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"
#include "Fw/Logger/Logger.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

TcpServerComponentImpl::TcpServerComponentImpl(const char* const compName)
    : TcpServerComponentBase(compName) {}

SocketIpStatus TcpServerComponentImpl::configure(const char* hostname,
                                                 const U16 port,
                                                 const U32 send_timeout_seconds,
                                                 const U32 send_timeout_microseconds,
	                                         FwSizeType buffer_size) {

    // Check that ensures the configured buffer size fits within the limits fixed-width type, U32

    FW_ASSERT(buffer_size <= std::numeric_limits<U32>::max(), static_cast<FwAssertArgType>(buffer_size));
    m_allocation_size = buffer_size; // Store the buffer size
				     //
    (void)m_socket.configure(hostname, port, send_timeout_seconds, send_timeout_microseconds);
    return startup();
}

TcpServerComponentImpl::~TcpServerComponentImpl() {}

// ----------------------------------------------------------------------
// Implementations for socket read task virtual methods
// ----------------------------------------------------------------------

U16 TcpServerComponentImpl::getListenPort() {
    return m_socket.getListenPort();
}

IpSocket& TcpServerComponentImpl::getSocketHandler() {
    return m_socket;
}

Fw::Buffer TcpServerComponentImpl::getBuffer() {
    return allocate_out(0, static_cast<U32>(m_allocation_size));
}

void TcpServerComponentImpl::sendBuffer(Fw::Buffer buffer, SocketIpStatus status) {
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

void TcpServerComponentImpl::connected() {
    if (isConnected_ready_OutputPort(0)) {
        this->ready_out(0);
    }
}

bool TcpServerComponentImpl::isStarted() {
    Os::ScopeLock scopedLock(this->m_lock);
    return this->m_descriptor.serverFd != -1;
}

SocketIpStatus TcpServerComponentImpl::startup() {
    Os::ScopeLock scopedLock(this->m_lock);
    Drv::SocketIpStatus status = SOCK_SUCCESS;
    // Prevent multiple startup attempts
    if (this->m_descriptor.serverFd == -1) {
        status = this->m_socket.startup(this->m_descriptor);
    }
    return status;
}

void TcpServerComponentImpl::terminate() {
    Os::ScopeLock scopedLock(this->m_lock);
    this->m_socket.terminate(this->m_descriptor);
    this->m_descriptor.serverFd = -1;
}

void TcpServerComponentImpl::readLoop() {
    Drv::SocketIpStatus status = Drv::SocketIpStatus::SOCK_NOT_STARTED;
    // Keep trying to reconnect until the status is good, told to stop, or reconnection is turned off
    do {
         status = this->startup();
         if (status != SOCK_SUCCESS) {
             Fw::Logger::log("[WARNING] Failed to listen on port %hu with status %d\n", this->getListenPort(), status);
             (void)Os::Task::delay(SOCKET_RETRY_INTERVAL);
             continue;
         }
    }
    while (this->running() && status != SOCK_SUCCESS && this->m_reopen);
    // If start up was successful then perform normal operations
    if (this->running() && status == SOCK_SUCCESS) {
        // Perform the nominal read loop
        SocketComponentHelper::readLoop();
    }
    // Terminate the server
    this->terminate();
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus TcpServerComponentImpl::send_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    Drv::SocketIpStatus status = this->send(fwBuffer.getData(), fwBuffer.getSize());
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
