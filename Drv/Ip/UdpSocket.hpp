// ======================================================================
// \title  UdpSocket.hpp
// \author mstarch
// \brief  hpp file for UdpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef DRV_IP_UDPSOCKET_HPP_
#define DRV_IP_UDPSOCKET_HPP_

#include <FpConfig.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <IpCfg.hpp>

namespace Drv {

/**
 * \brief a structure used to hold the encapsulated socket state to prevent namespace collision
 */
struct SocketState;

/**
 * \brief Helper for setting up Udp using Berkley sockets as a client
 *
 * Certain IP headers have conflicting definitions with the m_data member of various types in fprime. UdpSocket
 * separates the ip setup from the incoming Fw::Buffer in the primary component class preventing this collision.
 */
class UdpSocket : public IpSocket {
  public:
    /**
     * \brief Constructor for client socket udp implementation
     */
    UdpSocket();
    /**
     * \brief to cleanup state created at instantiation
     */
    virtual ~UdpSocket();

    /**
     * \brief configure the udp socket for outgoing transmissions
     *
     * Configures the UDP handler to use the given hostname and port for outgoing transmissions. Incoming hostname
     * and port are configured using the `configureRecv` function call for UDP as it requires separate host/port pairs
     * for outgoing and incoming transmissions. Hostname DNS translation is left up to the caller and thus hostname must
     * be an IP address in dot-notation of the form "x.x.x.x". Port cannot be set to 0 as dynamic port assignment is not
     * supported.  It is possible to configure the UDP port as a single-direction send port only.
     *
     * Note: delegates to `IpSocket::configure`
     *
     * \param hostname: socket uses for outgoing transmissions. Must be of form x.x.x.x
     * \param port: port socket uses for outgoing transmissions. Must NOT be 0.
     * \param send_timeout_seconds: send timeout seconds portion
     * \param send_timeout_microseconds: send timeout microseconds portion. Must be less than 1000000
     * \return status of configure
     */
    SocketIpStatus configureSend(const char* hostname, const U16 port, const U32 send_timeout_seconds,
                                 const U32 send_timeout_microseconds);

    /**
     * \brief configure the udp socket for incoming transmissions
     *
     * Configures the UDP handler to use the given hostname and port for incoming transmissions. Outgoing hostname
     * and port are configured using the `configureSend` function call for UDP as it requires separate host/port pairs
     * for outgoing and incoming transmissions. Hostname DNS translation is left up to the caller and thus hostname must
     * be an IP address in dot-notation of the form "x.x.x.x". Port cannot be set to 0 as dynamic port assignment is not
     * supported. It is possible to configure the UDP port as a single-direction receive port only.
     *
     * \param hostname: socket uses for incoming transmissions. Must be of form x.x.x.x
     * \param port: port socket uses for incoming transmissions. Must NOT be 0.
     * \return status of configure
     */
    SocketIpStatus configureRecv(const char* hostname, const U16 port);

  PROTECTED:

    /**
     * \brief bind the UDP to a port such that it can receive packets at the previously configured port
     * \param fd: socket file descriptor used in bind
     * \return status of the bind
     */
    SocketIpStatus bind(NATIVE_INT_TYPE fd);
    /**
     * \brief udp specific implementation for opening a socket.
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
    SocketState* m_state; //!< State storage
    U16 m_recv_port;  //!< IP address port used
    char m_recv_hostname[SOCKET_MAX_HOSTNAME_SIZE];  //!< Hostname to supply
};
}  // namespace Drv

#endif /* DRV_IP_UDPSOCKET_HPP_ */
