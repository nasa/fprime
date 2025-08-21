// ======================================================================
// \title  IpSocket.cpp
// \author mstarch, crsmith
// \brief  cpp file for IpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <sys/time.h>
#include <Drv/Ip/IpSocket.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <cstring>

// This implementation has primarily implemented to isolate
// the socket interface from the F' Fw::Buffer class.
// There is a macro in VxWorks (m_data) that collides with
// the m_data member in Fw::Buffer.

#ifdef TGT_OS_TYPE_VXWORKS
#include <errnoLib.h>
#include <fioLib.h>
#include <hostLib.h>
#include <inetLib.h>
#include <ioLib.h>
#include <sockLib.h>
#include <socket.h>
#include <sysLib.h>
#include <taskLib.h>
#include <vxWorks.h>
#include <cstring>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#else
#error OS not supported for IP Socket Communications
#endif

namespace Drv {

IpSocket::IpSocket() : m_timeoutSeconds(0), m_timeoutMicroseconds(0), m_port(0) {
    ::memset(m_hostname, 0, sizeof(m_hostname));
}

SocketIpStatus IpSocket::configure(const char* const hostname,
                                   const U16 port,
                                   const U32 timeout_seconds,
                                   const U32 timeout_microseconds) {
    FW_ASSERT(timeout_microseconds < 1000000, static_cast<FwAssertArgType>(timeout_microseconds));
    FW_ASSERT(this->isValidPort(port), static_cast<FwAssertArgType>(port));
    FW_ASSERT(hostname != nullptr);
    this->m_timeoutSeconds = timeout_seconds;
    this->m_timeoutMicroseconds = timeout_microseconds;
    this->m_port = port;
    (void)Fw::StringUtils::string_copy(this->m_hostname, hostname, static_cast<FwSizeType>(SOCKET_MAX_HOSTNAME_SIZE));
    return SOCK_SUCCESS;
}

bool IpSocket::isValidPort(U16 port) {
    return true;
}

SocketIpStatus IpSocket::setupTimeouts(int socketFd) {
// Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
    // No timeouts set on Vxworks
#else
    // Set timeout socket option
    struct timeval timeout;
    timeout.tv_sec = static_cast<time_t>(this->m_timeoutSeconds);
    timeout.tv_usec = static_cast<suseconds_t>(this->m_timeoutMicroseconds);
    // set socket write to timeout after 1 sec
    if (setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)) < 0) {
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
    int ip = inet_addr(address);
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

void IpSocket::close(const SocketDescriptor& socketDescriptor) {
    (void)::close(socketDescriptor.fd);
}

void IpSocket::shutdown(const SocketDescriptor& socketDescriptor) {
    errno = 0;
    int status = ::shutdown(socketDescriptor.fd, SHUT_RDWR);
    // If shutdown fails, go straight to the hard-shutdown
    if (status != 0) {
        this->close(socketDescriptor);
    }
}

SocketIpStatus IpSocket::open(SocketDescriptor& socketDescriptor) {
    SocketIpStatus status = SOCK_SUCCESS;
    errno = 0;
    // Open a TCP socket for incoming commands, and outgoing data if not using UDP
    status = this->openProtocol(socketDescriptor);
    if (status != SOCK_SUCCESS) {
        socketDescriptor.fd = -1;
        return status;
    }
    return status;
}

SocketIpStatus IpSocket::send(const SocketDescriptor& socketDescriptor, const U8* const data, const FwSizeType size) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(size > 0);

    FwSizeType total = 0;
    FwSignedSizeType sent = 0;
    // Attempt to send out data and retry as necessary
    for (FwSizeType i = 0; (i < SOCKET_MAX_ITERATIONS) && (total < size); i++) {
        errno = 0;
        // Send using my specific protocol
        sent = this->sendProtocol(socketDescriptor, data + total, size - total);
        // Error is EINTR or timeout just try again
        if (((sent == -1) && (errno == EINTR)) || (sent == 0)) {
            continue;
        }
        // Error bad file descriptor is a close along with reset
        else if ((sent == -1) && ((errno == EBADF) || (errno == ECONNRESET))) {
            return SOCK_DISCONNECTED;
        }
        // Error returned, and it wasn't an interrupt nor a disconnect
        else if (sent == -1) {
            return SOCK_SEND_ERROR;
        }
        FW_ASSERT(sent > 0, static_cast<FwAssertArgType>(sent));
        total += static_cast<FwSizeType>(sent);
    }
    // Failed to retry enough to send all data
    if (total < size) {
        return SOCK_INTERRUPTED_TRY_AGAIN;
    }
    // Ensure we sent everything
    FW_ASSERT(total == size, static_cast<FwAssertArgType>(total), static_cast<FwAssertArgType>(size));
    return SOCK_SUCCESS;
}

SocketIpStatus IpSocket::recv(const SocketDescriptor& socketDescriptor, U8* data, FwSizeType& req_read) {
    // TODO: Uncomment FW_ASSERT for socketDescriptor.fd once we fix TcpClientTester to not pass in uninitialized
    // socketDescriptor
    //  FW_ASSERT(socketDescriptor.fd != -1, static_cast<FwAssertArgType>(socketDescriptor.fd));
    FW_ASSERT(data != nullptr);

    FwSignedSizeType bytes_received_or_status;  // Stores the return value from recvProtocol

    // Loop primarily for EINTR. Other conditions should lead to an earlier exit.
    for (FwSizeType i = 0; i < SOCKET_MAX_ITERATIONS; i++) {
        errno = 0;
        // Pass the current value of req_read (max buffer size) to recvProtocol.
        // recvProtocol returns bytes read or -1 on error.
        bytes_received_or_status = this->recvProtocol(socketDescriptor, data, req_read);

        if (bytes_received_or_status > 0) {
            // Successfully read data
            req_read = static_cast<FwSizeType>(bytes_received_or_status);
            return SOCK_SUCCESS;
        } else if (bytes_received_or_status == 0) {
            // Handle zero return based on protocol-specific behavior
            req_read = 0;
            return this->handleZeroReturn();
        } else {  // bytes_received_or_status == -1, an error occurred
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // Non-blocking socket would block, or SO_RCVTIMEO timeout occurred.
                req_read = 0;
                return SOCK_NO_DATA_AVAILABLE;
            } else if ((errno == ECONNRESET) || (errno == EBADF)) {
                // Connection reset or bad file descriptor.
                req_read = 0;
                return SOCK_DISCONNECTED;  // Or a more specific error like SOCK_READ_ERROR
            } else {
                // Other socket read error.
                req_read = 0;
                return SOCK_READ_ERROR;
            }
        }
    }
    // If the loop completes, it means SOCKET_MAX_ITERATIONS of EINTR occurred.
    req_read = 0;
    return SOCK_INTERRUPTED_TRY_AGAIN;
}

SocketIpStatus IpSocket::handleZeroReturn() {
    // For TCP (which IpSocket primarily serves as a base for, or when not overridden),
    // a return of 0 from ::recv means the peer has performed an orderly shutdown.
    return SOCK_DISCONNECTED;
}

}  // namespace Drv
