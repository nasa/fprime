// ======================================================================
// \title  TcpServerSocket.cpp
// \author mstarch
// \brief  cpp file for TcpServerSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <Drv/Ip/TcpServerSocket.hpp>
#include <Fw/Logger/Logger.hpp>
#include <FpConfig.hpp>


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

namespace Drv {

TcpServerSocket::TcpServerSocket() : IpSocket(), m_base_fd(-1) {}

SocketIpStatus TcpServerSocket::startup() {
    NATIVE_INT_TYPE serverFd = -1;
    struct sockaddr_in address;
    this->close();

    // Acquire a socket, or return error
    if ((serverFd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return SOCK_FAILED_TO_GET_SOCKET;
    }
    // Set up the address port and name
    address.sin_family = AF_INET;
    address.sin_port = htons(this->m_port);

    // OS specific settings
#if defined TGT_OS_TYPE_VXWORKS || TGT_OS_TYPE_DARWIN
    address.sin_len = static_cast<U8>(sizeof(struct sockaddr_in));
#endif
    // First IP address to socket sin_addr
    if (IpSocket::addressToIp4(m_hostname, &(address.sin_addr)) != SOCK_SUCCESS) {
        ::close(serverFd);
        return SOCK_INVALID_IP_ADDRESS;
    };

    // TCP requires bind to an address to the socket
    if (::bind(serverFd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(serverFd);
        return SOCK_FAILED_TO_BIND;
    }
    m_base_fd = serverFd;
    Fw::Logger::logMsg("Listening for single client at %s:%hu\n", reinterpret_cast<POINTER_CAST>(m_hostname), m_port);
    // TCP requires listening on a the socket. Second argument prevents queueing of anything more than a single client.
    if (::listen(serverFd, 0) < 0) {
        ::close(serverFd);
        return SOCK_FAILED_TO_LISTEN; // What we have here is a failure to communicate
    }
    return SOCK_SUCCESS;
}

void TcpServerSocket::shutdown() {
    (void)::shutdown(this->m_base_fd, SHUT_RDWR);
    (void)::close(this->m_base_fd);
    m_base_fd = -1;
    this->close();
}

SocketIpStatus TcpServerSocket::openProtocol(NATIVE_INT_TYPE& fd) {
    NATIVE_INT_TYPE clientFd = -1;
    // TCP requires accepting on a the socket to get the client socket file descriptor.
    if ((clientFd = ::accept(m_base_fd, nullptr, nullptr)) < 0) {
        return SOCK_FAILED_TO_ACCEPT; // What we have here is a failure to communicate
    }
    // Setup client send timeouts
    if (IpSocket::setupTimeouts(clientFd) != SOCK_SUCCESS) {
        ::close(clientFd);
        return SOCK_FAILED_TO_SET_SOCKET_OPTIONS;
    }
    Fw::Logger::logMsg("Accepted client at %s:%hu\n", reinterpret_cast<POINTER_CAST>(m_hostname), m_port);
    fd = clientFd;
    return SOCK_SUCCESS;
}

I32 TcpServerSocket::sendProtocol(const U8* const data, const U32 size) {
    return ::send(this->m_fd, data, size, SOCKET_IP_SEND_FLAGS);
}

I32 TcpServerSocket::recvProtocol(U8* const data, const U32 size) {
    return ::recv(this->m_fd, data, size, SOCKET_IP_RECV_FLAGS);
}

}  // namespace Drv
