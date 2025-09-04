// ======================================================================
// \title  UdpSocket.cpp
// \author mstarch
// \brief  cpp file for UdpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <Drv/Ip/UdpSocket.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>

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
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cerrno>
#include <cstring>
#include <new>

namespace Drv {

UdpSocket::UdpSocket() : IpSocket(), m_recv_configured(false) {
    (void)::memset(&m_addr_send, 0, sizeof(m_addr_send));
    (void)::memset(&m_addr_recv, 0, sizeof(m_addr_recv));
}

UdpSocket::~UdpSocket() = default;

SocketIpStatus UdpSocket::configure(const char* const hostname,
                                    const U16 port,
                                    const U32 timeout_seconds,
                                    const U32 timeout_microseconds) {
    FW_ASSERT(0);  // Must use configureSend and/or configureRecv
    return SocketIpStatus::SOCK_INVALID_CALL;
}

SocketIpStatus UdpSocket::configureSend(const char* const hostname,
                                        const U16 port,
                                        const U32 timeout_seconds,
                                        const U32 timeout_microseconds) {
    FW_ASSERT(hostname != nullptr);
    FW_ASSERT(this->isValidPort(port));
    FW_ASSERT(timeout_microseconds < 1000000);
    return IpSocket::configure(hostname, port, timeout_seconds, timeout_microseconds);
}

SocketIpStatus UdpSocket::configureRecv(const char* hostname, const U16 port) {
    FW_ASSERT(hostname != nullptr);
    FW_ASSERT(this->isValidPort(port));
    FW_ASSERT(Fw::StringUtils::string_length(hostname, SOCKET_MAX_HOSTNAME_SIZE) < SOCKET_MAX_HOSTNAME_SIZE);

    // Initialize the receive address structure
    (void)::memset(&m_addr_recv, 0, sizeof(m_addr_recv));
    m_addr_recv.sin_family = AF_INET;
    m_addr_recv.sin_port = htons(port);

    // Convert hostname to IP address
    SocketIpStatus status = IpSocket::addressToIp4(hostname, &m_addr_recv.sin_addr);
    if (status != SOCK_SUCCESS) {
        return status;
    }

    this->m_recv_configured = true;
    return SOCK_SUCCESS;
}

U16 UdpSocket::getRecvPort() {
    return ntohs(this->m_addr_recv.sin_port);
}

SocketIpStatus UdpSocket::bind(const int fd) {
    FW_ASSERT(fd != -1);
    struct sockaddr_in address = this->m_addr_recv;

    // OS specific settings
#if defined TGT_OS_TYPE_VXWORKS || TGT_OS_TYPE_DARWIN
    address.sin_len = static_cast<U8>(sizeof(struct sockaddr_in));
#endif
    // UDP (for receiving) requires bind to an address to the socket
    if (::bind(fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        return SOCK_FAILED_TO_BIND;
    }

    socklen_t size = sizeof(address);
    if (::getsockname(fd, reinterpret_cast<struct sockaddr*>(&address), &size) == -1) {
        return SOCK_FAILED_TO_READ_BACK_PORT;
    }

    // Update m_addr_recv with the actual port assigned (for ephemeral port support)
    this->m_addr_recv.sin_port = address.sin_port;

    return SOCK_SUCCESS;
}

SocketIpStatus UdpSocket::openProtocol(SocketDescriptor& socketDescriptor) {
    if (this->m_port == 0 && !this->m_recv_configured) {
        return SOCK_INVALID_CALL;  // Neither send nor receive is configured
    }

    SocketIpStatus status = SOCK_SUCCESS;
    int socketFd = -1;

    // Initialize address structure to zero before use
    struct sockaddr_in address;
    (void)::memset(&address, 0, sizeof(address));

    U16 port = this->m_port;
    U16 recv_port = ntohs(this->m_addr_recv.sin_port);

    // Acquire a socket, or return error
    if ((socketFd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return SOCK_FAILED_TO_GET_SOCKET;
    }

    // May not be sending in all cases
    if (port != 0) {
        // Set up the address port and name
        address.sin_family = AF_INET;
        address.sin_port = htons(this->m_port);

        // OS specific settings
#if defined TGT_OS_TYPE_VXWORKS || TGT_OS_TYPE_DARWIN
        address.sin_len = static_cast<U8>(sizeof(struct sockaddr_in));
#endif

        // First IP address to socket sin_addr
        status = IpSocket::addressToIp4(m_hostname, &(address.sin_addr));
        if (status != SOCK_SUCCESS) {
            Fw::Logger::log("Failed to resolve hostname %s: %d\n", m_hostname, static_cast<I32>(status));
            ::close(socketFd);
            return status;
        };

        // Now apply timeouts
        status = this->setupTimeouts(socketFd);
        if (status != SOCK_SUCCESS) {
            ::close(socketFd);
            return status;
        }
        FW_ASSERT(sizeof(this->m_addr_send) == sizeof(address), static_cast<FwAssertArgType>(sizeof(this->m_addr_send)),
                  static_cast<FwAssertArgType>(sizeof(address)));
        (void)memcpy(&this->m_addr_send, &address, sizeof(this->m_addr_send));
    }

    // Only bind if configureRecv was called (including ephemeral)
    if (this->m_recv_configured) {
        status = this->bind(socketFd);

        if (status != SOCK_SUCCESS) {
            (void)::close(socketFd);  // Closing FD as a retry will reopen send side
            return status;
        }
    }

    // Log message for UDP
    char recv_addr[INET_ADDRSTRLEN];
    const char* recv_addr_str = inet_ntop(AF_INET, &(this->m_addr_recv.sin_addr), recv_addr, INET_ADDRSTRLEN);
    if (recv_addr_str == nullptr) {
        (void)Fw::StringUtils::string_copy(recv_addr, "INVALID_ADDR", INET_ADDRSTRLEN);
    }

    if ((port == 0) && (recv_port > 0)) {
        Fw::Logger::log("Setup to only receive udp at %s:%hu\n", recv_addr, recv_port);
    } else if ((port > 0) && (recv_port == 0)) {
        Fw::Logger::log("Setup to only send udp at %s:%hu\n", m_hostname, port);
    } else if ((port > 0) && (recv_port > 0)) {
        Fw::Logger::log("Setup to receive udp at %s:%hu and send to %s:%hu\n", recv_addr, recv_port, m_hostname, port);
    }

    FW_ASSERT(status == SOCK_SUCCESS, static_cast<FwAssertArgType>(status));
    socketDescriptor.fd = socketFd;
    return status;
}

FwSignedSizeType UdpSocket::sendProtocol(const SocketDescriptor& socketDescriptor,
                                         const U8* const data,
                                         const FwSizeType size) {
    FW_ASSERT(this->m_addr_send.sin_family != 0);  // Make sure the address was previously setup
    FW_ASSERT(socketDescriptor.fd >= 0);           // File descriptor should be valid
    FW_ASSERT(data != nullptr);                    // Data pointer should not be null

    return static_cast<FwSignedSizeType>(
        ::sendto(socketDescriptor.fd, data, static_cast<size_t>(size), SOCKET_IP_SEND_FLAGS,
                 reinterpret_cast<struct sockaddr*>(&this->m_addr_send), sizeof(this->m_addr_send)));
}

FwSignedSizeType UdpSocket::recvProtocol(const SocketDescriptor& socketDescriptor,
                                         U8* const data,
                                         const FwSizeType size) {
    FW_ASSERT(this->m_addr_recv.sin_family != 0);  // Make sure the address was previously setup
    FW_ASSERT(socketDescriptor.fd >= 0);           // File descriptor should be valid
    FW_ASSERT(data != nullptr);                    // Data pointer should not be null

    // Initialize sender address structure to zero
    struct sockaddr_in sender_addr;
    (void)::memset(&sender_addr, 0, sizeof(sender_addr));

    socklen_t sender_addr_len = sizeof(sender_addr);
    FwSignedSizeType received = static_cast<FwSignedSizeType>(
        ::recvfrom(socketDescriptor.fd, data, static_cast<size_t>(size), SOCKET_IP_RECV_FLAGS,
                   reinterpret_cast<struct sockaddr*>(&sender_addr), &sender_addr_len));
    // If we have not configured a send port, set it to the source of the last received packet
    if (received >= 0 && this->m_addr_send.sin_port == 0) {
        this->m_addr_send = sender_addr;
        this->m_port = ntohs(sender_addr.sin_port);
        Fw::Logger::log("Configured send port to %hu as specified by the last received packet.\n", this->m_port);
    }
    return received;
}

SocketIpStatus UdpSocket::send(const SocketDescriptor& socketDescriptor, const U8* const data, const FwSizeType size) {
    // Note: socketDescriptor.fd can be -1 in some test cases
    FW_ASSERT((size == 0) || (data != nullptr));

    // Special case for zero-length datagrams in UDP
    if (size == 0) {
        errno = 0;
        FwSignedSizeType sent = this->sendProtocol(socketDescriptor, data, 0);
        if (sent == -1) {
            if (errno == EINTR) {
                // For zero-length datagrams, we'll just try once more if interrupted
                errno = 0;
                sent = this->sendProtocol(socketDescriptor, data, 0);
            }

            if (sent == -1) {
                if ((errno == EBADF) || (errno == ECONNRESET)) {
                    return SOCK_DISCONNECTED;
                } else {
                    return SOCK_SEND_ERROR;
                }
            }
        }
        // For zero-length datagrams in UDP, success is either 0 or a non-negative value
        return SOCK_SUCCESS;
    }

    // For non-zero-length data, delegate to the base class implementation
    return IpSocket::send(socketDescriptor, data, size);
}

SocketIpStatus UdpSocket::handleZeroReturn() {
    // For UDP, a return of 0 from recvfrom means a 0-byte datagram was received.
    // This is a success case for UDP, not a disconnection.
    return SOCK_SUCCESS;
}

}  // namespace Drv
