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
 * \brief Helper for setting up Tcp using Berkley sockets as a client
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
     * \brief Tcp specific implementation for opening a client socket.
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
};
}  // namespace Drv

#endif /* DRV_TCPCLIENT_TCPHELPER_HPP_ */
