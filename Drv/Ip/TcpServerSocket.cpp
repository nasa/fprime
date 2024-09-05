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

TcpServerSocket::TcpServerSocket() : IpSocket(), m_base_fd(-1) {}

U16 TcpServerSocket::getListenPort() {
    this->m_lock.lock();
    U16 port = this->m_port;
    this->m_lock.unlock();
    return port;
}

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
    this->m_lock.lock();
    address.sin_port = htons(this->m_port);
    this->m_lock.unlock();

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
    U16 port = ntohs(address.sin_port);
    Fw::Logger::log("Listening for single client at %s:%hu\n", m_hostname, port);
    // TCP requires listening on the socket. Since we only expect a single client, set the TCP backlog (second argument) to 1 to prevent queuing of multiple clients. 
    if (::listen(serverFd, 1) < 0) {
        ::close(serverFd);
        return SOCK_FAILED_TO_LISTEN; // What we have here is a failure to communicate
    }

    this->m_lock.lock();
    m_base_fd = serverFd;
    m_port = port;
    this->m_lock.unLock();

    return this->IpSocket::startup();
}

void TcpServerSocket::shutdown() {
    this->m_lock.lock();
    if (this->m_base_fd != -1) {
        (void)::shutdown(this->m_base_fd, SHUT_RDWR);
        (void)::close(this->m_base_fd);
        this->m_base_fd = -1;
    }
    this->m_lock.unLock();
    this->IpSocket::shutdown();
}

SocketIpStatus TcpServerSocket::openProtocol(NATIVE_INT_TYPE& fd) {
    NATIVE_INT_TYPE clientFd = -1;
    NATIVE_INT_TYPE serverFd = -1;

    // Check started before allowing open
    if (not this->isStarted()) {
        return SOCK_NOT_STARTED;
    }

    this->m_lock.lock();
    serverFd = this->m_base_fd;
    this->m_lock.unLock();

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
    fd = clientFd;
    return SOCK_SUCCESS;
}

I32 TcpServerSocket::sendProtocol(const U8* const data, const U32 size) {
    return static_cast<I32>(::send(this->m_fd, data, size, SOCKET_IP_SEND_FLAGS));
}

I32 TcpServerSocket::recvProtocol(U8* const data, const U32 size) {
    return static_cast<I32>(::recv(this->m_fd, data, size, SOCKET_IP_RECV_FLAGS));
}

}  // namespace Drv
