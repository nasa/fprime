// ======================================================================
// \title  IpSocket.cpp
// \author mstarch
// \brief  cpp file for IpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <cstring>
#include <Drv/Ip/IpSocket.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <sys/time.h>

// This implementation has primarily implemented to isolate
// the socket interface from the F' Fw::Buffer class.
// There is a macro in VxWorks (m_data) that collides with
// the m_data member in Fw::Buffer.

#ifdef TGT_OS_TYPE_VXWORKS
#include <socket.h>
    #include <inetLib.h>
    #include <fioLib.h>
    #include <hostLib.h>
    #include <ioLib.h>
    #include <vxWorks.h>
    #include <sockLib.h>
    #include <fioLib.h>
    #include <taskLib.h>
    #include <sysLib.h>
    #include <errnoLib.h>
    #include <cstring>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>
#else
#error OS not supported for IP Socket Communications
#endif


namespace Drv {

IpSocket::IpSocket() : m_fd(-1), m_timeoutSeconds(0), m_timeoutMicroseconds(0), m_port(0), m_open(false), m_started(false) {
    ::memset(m_hostname, 0, sizeof(m_hostname));
}

SocketIpStatus IpSocket::configure(const char* const hostname, const U16 port, const U32 timeout_seconds, const U32 timeout_microseconds) {
    FW_ASSERT(timeout_microseconds < 1000000, static_cast<FwAssertArgType>(timeout_microseconds));
    FW_ASSERT(this->isValidPort(port));
    this->m_lock.lock();
    this->m_timeoutSeconds = timeout_seconds;
    this->m_timeoutMicroseconds = timeout_microseconds;
    this->m_port = port;
    (void) Fw::StringUtils::string_copy(this->m_hostname, hostname, SOCKET_MAX_HOSTNAME_SIZE);
    this->m_lock.unlock();
    return SOCK_SUCCESS;
}

bool IpSocket::isValidPort(U16 port) {
    return true;
}

SocketIpStatus IpSocket::setupTimeouts(NATIVE_INT_TYPE socketFd) {
// Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
    // No timeouts set on Vxworks
#else
    // Set timeout socket option
    struct timeval timeout;
    timeout.tv_sec = static_cast<time_t>(this->m_timeoutSeconds);
    timeout.tv_usec = static_cast<suseconds_t>(this->m_timeoutMicroseconds);
    // set socket write to timeout after 1 sec
    if (setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) < 0) {
        return SOCK_FAILED_TO_SET_SOCKET_OPTIONS;
    }
#endif
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::addressToIp4(const char* address, void* ip4) {
    FW_ASSERT(address != nullptr);
    FW_ASSERT(ip4 != nullptr);
    // Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
    NATIVE_INT_TYPE ip = inet_addr(address);
    if (ip == ERROR) {
        return SOCK_INVALID_IP_ADDRESS;
    }
    // from sin_addr, which has one struct
    // member s_addr, which is unsigned int
    *reinterpret_cast<unsigned long*>(ip4) = ip;
#else
    // First IP address to socket sin_addr
    if (not ::inet_pton(AF_INET, address, ip4)) {
        return SOCK_INVALID_IP_ADDRESS;
    };
#endif
    return SOCK_SUCCESS;
}

bool IpSocket::isStarted() {
    bool is_started = false;
    this->m_lock.lock();
    is_started = this->m_started;
    this->m_lock.unLock();
    return is_started;
}

bool IpSocket::isOpened() {
    bool is_open = false;
    this->m_lock.lock();
    is_open = this->m_open;
    this->m_lock.unLock();
    return is_open;
}

void IpSocket::close() {
    this->m_lock.lock();
    if (this->m_fd != -1) {
        (void)::shutdown(this->m_fd, SHUT_RDWR);
        (void)::close(this->m_fd);
        this->m_fd = -1;
    }
    this->m_open = false;
    this->m_lock.unLock();
}

void IpSocket::shutdown() {
    this->close();
    this->m_lock.lock();
    this->m_started = false;
    this->m_lock.unLock();
}

SocketIpStatus IpSocket::startup() {
    this->m_lock.lock();
    this->m_started = true;
    this->m_lock.unLock();
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::open() {
    NATIVE_INT_TYPE fd = -1;
    SocketIpStatus status = SOCK_SUCCESS;
    this->m_lock.lock();
    FW_ASSERT(m_fd == -1 and not m_open); // Ensure we are not opening an opened socket
    this->m_lock.unlock();
    // Open a TCP socket for incoming commands, and outgoing data if not using UDP
    status = this->openProtocol(fd);
    if (status != SOCK_SUCCESS) {
        FW_ASSERT(m_fd == -1); // Ensure we properly kept closed on error
        return status;
    }
    // Lock to update values and "officially open"
    this->m_lock.lock();
    this->m_fd = fd;
    this->m_open = true;
    this->m_lock.unLock();
    return status;
}

SocketIpStatus IpSocket::send(const U8* const data, const U32 size) {
    U32 total = 0;
    I32 sent  = 0;
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    // Prevent transmission before connection, or after a disconnect
    if (fd == -1) {
        return SOCK_DISCONNECTED;
    }
    // Attempt to send out data and retry as necessary
    for (U32 i = 0; (i < SOCKET_MAX_ITERATIONS) && (total < size); i++) {
        // Send using my specific protocol
        sent = this->sendProtocol(data + total, size - total);
        // Error is EINTR or timeout just try again
        if (((sent == -1) && (errno == EINTR)) || (sent == 0)) {
            continue;
        }
        // Error bad file descriptor is a close along with reset
        else if ((sent == -1) && ((errno == EBADF) || (errno == ECONNRESET))) {
            this->close();
            return SOCK_DISCONNECTED;
        }
        // Error returned, and it wasn't an interrupt nor a disconnect
        else if (sent == -1) {
            return SOCK_SEND_ERROR;
        }
        FW_ASSERT(sent > 0, sent);
        total += static_cast<U32>(sent);
    }
    // Failed to retry enough to send all data
    if (total < size) {
        return SOCK_INTERRUPTED_TRY_AGAIN;
    }
    // Ensure we sent everything
    FW_ASSERT(total == size, static_cast<FwAssertArgType>(total), static_cast<FwAssertArgType>(size));
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::recv(U8* data, U32& req_read) {
    I32 size = 0;
    // Check for previously disconnected socket
    this->m_lock.lock();
    NATIVE_INT_TYPE fd = this->m_fd;
    this->m_lock.unlock();
    if (fd == -1) {
        return SOCK_DISCONNECTED;
    }

    // Try to read until we fail to receive data
    for (U32 i = 0; (i < SOCKET_MAX_ITERATIONS) && (size <= 0); i++) {
        // Attempt to recv out data
        size = this->recvProtocol(data, req_read);
        // Error is EINTR, just try again
        if (size == -1 && ((errno == EINTR) || errno == EAGAIN)) {
            continue;
        }
        // Zero bytes read reset or bad ef means we've disconnected
        else if (size == 0 || ((size == -1) && ((errno == ECONNRESET) || (errno == EBADF)))) {
            this->close();
            req_read = static_cast<U32>(size);
            return SOCK_DISCONNECTED;
        }
        // Error returned, and it wasn't an interrupt, nor a disconnect
        else if (size == -1) {
            req_read = static_cast<U32>(size);
            return SOCK_READ_ERROR;  // Stop recv task on error
        }
    }
    req_read = static_cast<U32>(size);
    // Prevent interrupted socket being viewed as success
    if (size == -1) {
        return SOCK_INTERRUPTED_TRY_AGAIN;
    }
    return SOCK_SUCCESS;
}

}  // namespace Drv
