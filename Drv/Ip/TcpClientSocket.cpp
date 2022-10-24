// ======================================================================
// \title  TcpClientSocket.cpp
// \author mstarch
// \brief  cpp file for TcpClientSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/Ip/TcpClientSocket.hpp>
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

#include <cstdio>
#include <cstring>

namespace Drv {

TcpClientSocket::TcpClientSocket() : IpSocket() {}

SocketIpStatus TcpClientSocket::openProtocol(NATIVE_INT_TYPE& fd) {
    NATIVE_INT_TYPE socketFd = -1;
    struct sockaddr_in address;

    // Acquire a socket, or return error
    if ((socketFd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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
        ::close(socketFd);
        return SOCK_INVALID_IP_ADDRESS;
    };

    // Now apply timeouts
    if (IpSocket::setupTimeouts(socketFd) != SOCK_SUCCESS) {
        ::close(socketFd);
        return SOCK_FAILED_TO_SET_SOCKET_OPTIONS;
    }

    // TCP requires connect to the socket to allow for communication
    if (::connect(socketFd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
        ::close(socketFd);
        return SOCK_FAILED_TO_CONNECT;
    }

    fd = socketFd;
    Fw::Logger::logMsg("Connected to %s:%hu as a tcp client\n", reinterpret_cast<POINTER_CAST>(m_hostname), m_port);
    return SOCK_SUCCESS;
}

I32 TcpClientSocket::sendProtocol(const U8* const data, const U32 size) {
    return ::send(this->m_fd, data, size, SOCKET_IP_SEND_FLAGS);
}

I32 TcpClientSocket::recvProtocol(U8* const data, const U32 size) {
    return ::recv(this->m_fd, data, size, SOCKET_IP_RECV_FLAGS);
}

}  // namespace Drv
