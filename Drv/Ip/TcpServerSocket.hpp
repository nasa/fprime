// ======================================================================
// \title  TcpServerSocket.hpp
// \author mstarch
// \brief  hpp file for TcpServerSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef DRV_TCPSERVER_TCPHELPER_HPP_
#define DRV_TCPSERVER_TCPHELPER_HPP_

#include <FpConfig.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <IpCfg.hpp>

namespace Drv {
/**
 * \brief Helper for setting up Tcp using Berkley sockets as a server
 *
 * Certain IP headers have conflicting definitions with the m_data member of various types in fprime. TcpServerSocket
 * separates the ip setup from the incoming Fw::Buffer in the primary component class preventing this collision.
 */
class TcpServerSocket : public IpSocket {
  public:
    /**
     * \brief Constructor for client socket tcp implementation
     */
    TcpServerSocket();

    /**
     * \brief Opens the server socket and listens, does not block.
     *
     * Opens the server's listening socket such that this server can listen for incoming client requests. Given the
     * nature of this component, only one (1) client can be handled at a time. After this call succeeds, clients may
     * connect. This call does not block, block occurs on `open` while waiting to accept incoming clients.
     * \return status of the server socket setup.
     */
    SocketIpStatus startup();

    /**
     * \brief Shutdown client socket, and listening server socket
     *
     *
     */
    void shutdown();

  PROTECTED:
    /**
     * \brief Tcp specific implementation for opening a client socket connected to this server.
     * \param fd: (output) file descriptor opened. Only valid on SOCK_SUCCESS. Otherwise will be invalid
     * \return status of open
     */
    SocketIpStatus openProtocol(NATIVE_INT_TYPE& fd);
    /**
     * \brief Protocol specific implementation of send.  Called directly with retry from send.
     * \param data: data to send
     * \param size: size of data to send
     * \return: size of data sent, or -1 on error.
     */
    I32 sendProtocol(const U8* const data, const U32 size);
    /**
     * \brief Protocol specific implementation of recv.  Called directly with error handling from recv.
     * \param data: data pointer to fill
     * \param size: size of data buffer
     * \return: size of data received, or -1 on error.
     */
    I32 recvProtocol( U8* const data, const U32 size);
  private:
    NATIVE_INT_TYPE m_base_fd; //!< File descriptor of the listening socket
};
}  // namespace Drv

#endif /* DRV_TCPSERVER_TCPHELPER_HPP_ */
