// ======================================================================
// \title  TcpClientSocket.hpp
// \author mstarch
// \brief  cpp file for TcpClientSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef DRV_TCPCLIENT_TCPHELPER_HPP_
#define DRV_TCPCLIENT_TCPHELPER_HPP_

#include <FpConfig.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <IpCfg.hpp>

namespace Drv {
/**
 * \brief Helper for setting up Tcp using Berkeley sockets as a client
 *
 * Certain IP headers have conflicting definitions with the m_data member of various types in fprime. TcpClientSocket
 * separates the ip setup from the incoming Fw::Buffer in the primary component class preventing this collision.
 */
class TcpClientSocket : public IpSocket {
  public:
    /**
     * \brief Constructor for client socket tcp implementation
     */
    TcpClientSocket();
  PROTECTED:
    /**
     * \brief Check if the given port is valid for the socket
     *
     * Some ports should be allowed for sockets and disabled on others (e.g. port 0 is a valid tcp server port but not a
     * client. This will check the port and return "true" if the port is valid, or "false" otherwise. In the tcp client
     * implementation, all ports are considered valid except for "0".
     *
     * \param port: port to check
     * \return true if valid, false otherwise
     */
    bool isValidPort(U16 port) override;

    /**
     * \brief Tcp specific implementation for opening a client socket.
     * \param fd: (output) file descriptor opened. Only valid on SOCK_SUCCESS. Otherwise will be invalid
     * \return status of open
     */
    SocketIpStatus openProtocol(NATIVE_INT_TYPE& fd) override;
    /**
     * \brief Protocol specific implementation of send.  Called directly with retry from send.
     * \param data: data to send
     * \param size: size of data to send
     * \return: size of data sent, or -1 on error.
     */
    I32 sendProtocol(const U8* const data, const U32 size) override;
    /**
     * \brief Protocol specific implementation of recv.  Called directly with error handling from recv.
     * \param data: data pointer to fill
     * \param size: size of data buffer
     * \return: size of data received, or -1 on error.
     */
    I32 recvProtocol( U8* const data, const U32 size) override;
};
}  // namespace Drv

#endif /* DRV_TCPCLIENT_TCPHELPER_HPP_ */
