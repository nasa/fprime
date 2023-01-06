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
    return this->IpSocket::configure(hostname, port, timeout_seconds, timeout_microseconds);
}

SocketIpStatus UdpSocket::configureRecv(const char* hostname, const U16 port) {
    this->m_recv_port = port;
    (void) Fw::StringUtils::string_copy(this->m_recv_hostname, hostname, SOCKET_MAX_HOSTNAME_SIZE);
    return SOCK_SUCCESS;
}


SocketIpStatus UdpSocket::bind(NATIVE_INT_TYPE fd) {
    struct sockaddr_in address;
    FW_ASSERT(-1 != fd);
    FW_ASSERT(0 != m_recv_port);

    // Set up the address port and name
    address.sin_family = AF_INET;
    address.sin_port = htons(m_recv_port);
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
    FW_ASSERT(sizeof(this->m_state->m_addr_recv) == sizeof(address), sizeof(this->m_state->m_addr_recv), sizeof(address));
    memcpy(&this->m_state->m_addr_recv, &address, sizeof(this->m_state->m_addr_recv));
    return SOCK_SUCCESS;
}

SocketIpStatus UdpSocket::openProtocol(NATIVE_INT_TYPE& fd) {
    SocketIpStatus status = SOCK_SUCCESS;
    NATIVE_INT_TYPE socketFd = -1;
    struct sockaddr_in address;

    // Ensure configured for at least send or receive
    if (m_port == 0 && m_recv_port == 0) {
        return SOCK_INVALID_IP_ADDRESS; // Consistent with port = 0 behavior in TCP
    }

    // Acquire a socket, or return error
    if ((socketFd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return SOCK_FAILED_TO_GET_SOCKET;
    }

    // May not be sending in all cases
    if (this->m_port != 0) {
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
    if ((m_recv_port != 0)  && ((status = this->bind(socketFd)) != SOCK_SUCCESS)) {
        ::close(socketFd);
        return status; // Not closing FD as it is still a valid send FD
    }
    const char* actions = (m_recv_port != 0 && m_port != 0) ? "send and receive" : ((m_port != 0) ? "send": "receive");
    Fw::Logger::logMsg("Setup to %s udp to %s:%hu\n", reinterpret_cast<POINTER_CAST>(actions),
                       reinterpret_cast<POINTER_CAST>(m_hostname), m_port);
    FW_ASSERT(status == SOCK_SUCCESS, status);
    fd = socketFd;
    return status;
}

I32 UdpSocket::sendProtocol(const U8* const data, const U32 size) {
    FW_ASSERT(this->m_state->m_addr_send.sin_family != 0); // Make sure the address was previously setup
    return ::sendto(this->m_fd, data, size, SOCKET_IP_SEND_FLAGS,
                    reinterpret_cast<struct sockaddr *>(&this->m_state->m_addr_send), sizeof(this->m_state->m_addr_send));
}

I32 UdpSocket::recvProtocol(U8* const data, const U32 size) {
    FW_ASSERT(this->m_state->m_addr_recv.sin_family != 0); // Make sure the address was previously setup
    return ::recvfrom(this->m_fd, data, size, SOCKET_IP_RECV_FLAGS, nullptr, nullptr);
}

}  // namespace Drv
