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
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/StringUtils.hpp>

#ifdef TGT_OS_TYPE_VXWORKS
    #include <socket.h>
    #include <inetLib.h>
    #include <fioLib.h>
    #include <hostLib.h>
    #include <ioLib.h>
    #include <vxWorks.h>
    #include <sockLib.h>
    #include <taskLib.h>
    #include <sysLib.h>
    #include <errnoLib.h>
    #include <cstring>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h>
#else
    #error OS not supported for IP Socket Communications
#endif

#include <cstring>
#include <new>

namespace Drv {

struct SocketState {
    struct sockaddr_in m_addr_send;  //!< UDP server address, maybe unused
    struct sockaddr_in m_addr_recv;  //!< UDP server address, maybe unused

    SocketState() {
        ::memset(&m_addr_send, 0, sizeof(m_addr_send));
        ::memset(&m_addr_recv, 0, sizeof(m_addr_recv));
    }
};

UdpSocket::UdpSocket() : IpSocket(), m_state(new(std::nothrow) SocketState), m_recv_port(0) {
    FW_ASSERT(m_state != nullptr);
}

UdpSocket::~UdpSocket() {
    FW_ASSERT(m_state);
    delete m_state;
}

SocketIpStatus UdpSocket::configureSend(const char* const hostname, const U16 port, const U32 timeout_seconds, const U32 timeout_microseconds) {
    //Timeout is for the send, so configure send will work with the base class
    FW_ASSERT(port != 0, port); // Send cannot be on port 0
    FW_ASSERT(hostname != nullptr);
    return this->IpSocket::configure(hostname, port, timeout_seconds, timeout_microseconds);
}

SocketIpStatus UdpSocket::configureRecv(const char* hostname, const U16 port) {
    FW_ASSERT(this->isValidPort(port));
    FW_ASSERT(hostname != nullptr);
    this->m_recv_port = port;
    (void) Fw::StringUtils::string_copy(this->m_recv_hostname, hostname, static_cast<FwSizeType>(SOCKET_MAX_HOSTNAME_SIZE));
    return SOCK_SUCCESS;
}

U16 UdpSocket::getRecvPort() {
    U16 port = this->m_recv_port;
    return port;
}


SocketIpStatus UdpSocket::bind(NATIVE_INT_TYPE fd) {
    struct sockaddr_in address;
    FW_ASSERT(-1 != fd);

    // Set up the address port and name
    address.sin_family = AF_INET;
    address.sin_port = htons(this->m_recv_port);
    // OS specific settings
#if defined TGT_OS_TYPE_VXWORKS || TGT_OS_TYPE_DARWIN
    address.sin_len = static_cast<U8>(sizeof(struct sockaddr_in));
#endif

    // First IP address to socket sin_addr
    if (IpSocket::addressToIp4(m_recv_hostname, &address.sin_addr) != SOCK_SUCCESS) {
        return SOCK_INVALID_IP_ADDRESS;
    };
    // UDP (for receiving) requires bind to an address to the socket
    if (::bind(fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        return SOCK_FAILED_TO_BIND;
    }

    socklen_t size = sizeof(address);
    if (::getsockname(fd, reinterpret_cast<struct sockaddr *>(&address), &size) == -1) {
        return SOCK_FAILED_TO_READ_BACK_PORT;
    }

    FW_ASSERT(sizeof(this->m_state->m_addr_recv) == sizeof(address), sizeof(this->m_state->m_addr_recv), sizeof(address));
    memcpy(&this->m_state->m_addr_recv, &address, sizeof(this->m_state->m_addr_recv));

    return SOCK_SUCCESS;
}

SocketIpStatus UdpSocket::openProtocol(NATIVE_INT_TYPE& fd) {
    SocketIpStatus status = SOCK_SUCCESS;
    NATIVE_INT_TYPE socketFd = -1;
    struct sockaddr_in address;

    U16 port = this->m_port;

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
        if ((status = IpSocket::addressToIp4(m_hostname, &(address.sin_addr))) != SOCK_SUCCESS) {
            ::close(socketFd);
            return status;
        };

        // Now apply timeouts
        if ((status = IpSocket::setupTimeouts(socketFd)) != SOCK_SUCCESS) {
            ::close(socketFd);
            return status;
        }
        FW_ASSERT(sizeof(this->m_state->m_addr_send) == sizeof(address), sizeof(this->m_state->m_addr_send),
                  sizeof(address));
        memcpy(&this->m_state->m_addr_send, &address, sizeof(this->m_state->m_addr_send));
    }

    // When we are setting up for receiving as well, then we must bind to a port
    if ((status = this->bind(socketFd)) != SOCK_SUCCESS) {
        ::close(socketFd);
        return status; // Not closing FD as it is still a valid send FD
    }
    U16 recv_port = this->m_recv_port;
    // Log message for UDP
    if (port == 0) {
        Fw::Logger::log("Setup to receive udp at %s:%hu\n", m_recv_hostname,
                           recv_port);
    } else {
        Fw::Logger::log("Setup to receive udp at %s:%hu and send to %s:%hu\n",
                           m_recv_hostname,
                           recv_port,
                           m_hostname,
                           port);
    }
    FW_ASSERT(status == SOCK_SUCCESS, status);
    fd = socketFd;
    return status;
}

I32 UdpSocket::sendProtocol(NATIVE_INT_TYPE fd, const U8* const data, const U32 size) {
    FW_ASSERT(this->m_state->m_addr_send.sin_family != 0); // Make sure the address was previously setup
    return static_cast<I32>(::sendto(fd, data, size, SOCKET_IP_SEND_FLAGS,
                    reinterpret_cast<struct sockaddr *>(&this->m_state->m_addr_send), sizeof(this->m_state->m_addr_send)));
}

I32 UdpSocket::recvProtocol(NATIVE_INT_TYPE fd, U8* const data, const U32 size) {
    FW_ASSERT(this->m_state->m_addr_recv.sin_family != 0); // Make sure the address was previously setup
    return static_cast<I32>(::recvfrom(fd, data, size, SOCKET_IP_RECV_FLAGS, nullptr, nullptr));
}

}  // namespace Drv
