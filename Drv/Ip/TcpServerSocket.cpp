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
#include <Fw/Types/Assert.hpp>
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

TcpServerSocket::TcpServerSocket() : IpSocket() {}

U16 TcpServerSocket::getListenPort() {
    U16 port = this->m_port;
    return port;
}

SocketIpStatus TcpServerSocket::startup(SocketDescriptor& socketDescriptor) {
    PlatformIntType serverFd = -1;
    struct sockaddr_in address;
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

    socklen_t size = sizeof(address);
    if (::getsockname(serverFd, reinterpret_cast<struct sockaddr *>(&address), &size) == -1) {
        ::close(serverFd);
        return SOCK_FAILED_TO_READ_BACK_PORT;
    }
    // TCP requires listening on the socket. Since we only expect a single client, set the TCP backlog (second argument) to 1 to prevent queuing of multiple clients.
    if (::listen(serverFd, 1) < 0) {
        ::close(serverFd);
        return SOCK_FAILED_TO_LISTEN; // What we have here is a failure to communicate
    }
    Fw::Logger::log("Listening for single client at %s:%hu\n", m_hostname, m_port);
    FW_ASSERT(serverFd != -1);
    socketDescriptor.serverFd = serverFd;
    this->m_port = ntohs(address.sin_port);
    return SOCK_SUCCESS;
}

void TcpServerSocket::terminate(const SocketDescriptor& socketDescriptor) {
    (void)::close(socketDescriptor.serverFd);
}

SocketIpStatus TcpServerSocket::openProtocol(SocketDescriptor& socketDescriptor) {
    PlatformIntType clientFd = -1;
    PlatformIntType serverFd = socketDescriptor.serverFd;

    // Check for not started yet, may be true in the case of start-up reconnect attempts
    if (serverFd == -1) {
        return SOCK_NOT_STARTED;
    }

    // TCP requires accepting on the socket to get the client socket file descriptor.
    clientFd = ::accept(serverFd, nullptr, nullptr);
    if (clientFd < 0) {
        return SOCK_FAILED_TO_ACCEPT; // What we have here is a failure to communicate
    }
    // Setup client send timeouts
    if (IpSocket::setupTimeouts(clientFd) != SOCK_SUCCESS) {
        ::close(clientFd);
        return SOCK_FAILED_TO_SET_SOCKET_OPTIONS;
    }

    Fw::Logger::log("Accepted client at %s:%hu\n", m_hostname, m_port);
    socketDescriptor.fd = clientFd;
    return SOCK_SUCCESS;
}

I32 TcpServerSocket::sendProtocol(const SocketDescriptor& socketDescriptor, const U8* const data, const U32 size) {
    return static_cast<I32>(::send(socketDescriptor.fd, data, size, SOCKET_IP_SEND_FLAGS));
}

I32 TcpServerSocket::recvProtocol(const SocketDescriptor& socketDescriptor, U8* const data, const U32 size) {
    I32 size_buf;
    // recv will return 0 if the client has done an orderly shutdown
    size_buf = static_cast<I32>(::recv(socketDescriptor.fd, data, size, SOCKET_IP_RECV_FLAGS));
    return size_buf;
}

}  // namespace Drv
