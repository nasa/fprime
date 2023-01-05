// ======================================================================
// \title  IpSocket.hpp
// \author mstarch
// \brief  hpp file for IpSocket core implementation classes
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef DRV_IP_IPHELPER_HPP_
#define DRV_IP_IPHELPER_HPP_

#include <FpConfig.hpp>
#include <IpCfg.hpp>
#include <Os/Mutex.hpp>

namespace Drv {
/**
 * \brief Status enumeration for socket return values
 */
enum SocketIpStatus {
    SOCK_SUCCESS = 0,                        //!< Socket operation successful
    SOCK_FAILED_TO_GET_SOCKET = -1,          //!< Socket open failed
    SOCK_FAILED_TO_GET_HOST_IP = -2,         //!< Host IP lookup failed
    SOCK_INVALID_IP_ADDRESS = -3,            //!< Bad IP address supplied
    SOCK_FAILED_TO_CONNECT = -4,             //!< Failed to connect socket
    SOCK_FAILED_TO_SET_SOCKET_OPTIONS = -5,  //!< Failed to configure socket
    SOCK_INTERRUPTED_TRY_AGAIN = -6,         //!< Interrupted status for retries
    SOCK_READ_ERROR = -7,                    //!< Failed to read socket
    SOCK_DISCONNECTED = -8,                  //!< Failed to read socket with disconnect
    SOCK_FAILED_TO_BIND = -9,                //!< Failed to bind to socket
    SOCK_FAILED_TO_LISTEN = -10,             //!< Failed to listen on socket
    SOCK_FAILED_TO_ACCEPT = -11,             //!< Failed to accept connection
    SOCK_SEND_ERROR = -13,                   //!< Failed to send after configured retries
};

/**
 * \brief Helper base-class for setting up Berkley sockets
 *
 * Certain IP headers have conflicting definitions with the m_data member of various types in fprime. TcpHelper
 * separates the ip setup from the incoming Fw::Buffer in the primary component class preventing this collision.
 */
class IpSocket {
  public:
    IpSocket();
    virtual ~IpSocket(){};
    /**
     * \brief configure the ip socket with host and transmission timeouts
     *
     * Configures the IP handler (Tcp, Tcp server, and Udp) to use the given hostname and port. When multiple ports are
     * used for send/receive these settings affect the send direction (as is the case for udp). Hostname DNS translation
     * is left up to the caller and thus hostname must be an IP address in dot-notation of the form "x.x.x.x". Port
     * cannot be set to 0 as dynamic port assignment is not supported.
     *
     * Note: for UDP sockets this is equivalent to `configureSend` and only sets up the transmission direction of the
     * socket.  A separate call to `configureRecv` is required to receive on the socket and should be made before the
     * `open` call has been made.
     *
     * \param hostname: socket uses for outgoing transmissions (and incoming when tcp). Must be of form x.x.x.x
     * \param port: port socket uses for outgoing transmissions (and incoming when tcp). Must NOT be 0.
     * \param send_timeout_seconds: send timeout seconds portion
     * \param send_timeout_microseconds: send timeout microseconds portion. Must be less than 1000000
     * \return status of configure
     */
    SocketIpStatus configure(const char* hostname, const U16 port, const U32 send_timeout_seconds,
                             const U32 send_timeout_microseconds);

    /**
     * \brief check if IP socket has previously been opened
     *
     * Check if this IpSocket has previously been opened. In the case of Udp this will check for outgoing transmissions
     * and (if configured) incoming transmissions as well. This does not guarantee errors will not occur when using this
     * socket as the remote component may have disconnected.
     *
     * \return true if socket is open, false otherwise
     */
    bool isOpened();

    /**
     * \brief open the IP socket for communications
     *
     * This will open the IP socket for communication. This method error checks and validates properties set using the
     * `configure` method.  Tcp sockets will open bidirectional communication assuming the `configure` function was
     * previously called. Udp sockets allow `configureRecv` and `configure`/`configureSend` calls to configure for
     * each direction separately and may be operated in a single-direction or bidirectional mode. This call returns a
     * status of SOCK_SEND means the port is ready for transmissions and any other status should be treated as an error
     * with the socket not capable of sending nor receiving. This method will properly close resources on any
     * unsuccessful status.
     *
     * In the case of server components (TcpServer) this function will block until a client has connected.
     *
     * Note: delegates to openProtocol for protocol specific implementation
     * \return status of open
     */
    SocketIpStatus open();
    /**
     * \brief send data out the IP socket from the given buffer
     *
     * Sends data out of the IpSocket. This outgoing transmission will be retried several times if the transmission
     * fails to send all the data. Retries are globally configured in the `SocketIpDriverCfg.hpp` header. Should the
     * socket be unavailable, SOCK_DISCONNECTED is returned and the socket should be reopened using the `open` call.
     * This can happen even when the socket has already been opened should a transmission error/closure be detected.
     * Unless an error is received, all data will have been transmitted.
     *
     * Note: delegates to `sendProtocol` to send the data
     *
     * \param data: pointer to data to send
     * \param size: size of data to send
     * \return status of the send, SOCK_DISCONNECTED to reopen, SOCK_SUCCESS on success, something else on error
     */
    SocketIpStatus send(const U8* const data, const U32 size);
    /**
     * \brief receive data from the IP socket from the given buffer
     *
     * Receives data from the IpSocket. Should the socket be unavailable, SOCK_DISCONNECTED will be returned and
     * the socket should be reopened using the `open` call. This can happen even when the socket has already been opened
     * should a transmission error/closure be detected. Since this blocks until data is available, it will retry as long
     * as EINTR is set and less than a max number of iterations has passed. This function will block to receive data and
     * will retry (up to a configured set of retries) as long as EINTR is returned.
     *
     * Note: delegates to `recvProtocol` to send the data
     *
     * \param data: pointer to data to fill with received data
     * \param size: maximum size of data buffer to fill
     * \return status of the send, SOCK_DISCONNECTED to reopen, SOCK_SUCCESS on success, something else on error
     */
    SocketIpStatus recv(U8* const data, I32& size);
    /**
     * \brief closes the socket
     *
     * Closes the socket opened by the open call. In this case of the TcpServer, this does NOT close server's listening
     * port (call `shutdown`) but will close the active client connection.
     */
    void close();

  PROTECTED:

    /**
     * \brief setup the socket timeout properties of the opened outgoing socket
     * \param socketFd: file descriptor to setup
     * \return status of timeout setup
    */
    SocketIpStatus setupTimeouts(NATIVE_INT_TYPE socketFd);

    /**
     * \brief converts a given address in dot form x.x.x.x to an ip address. ONLY works for IPv4.
     * \param address: address to convert
     * \param ip4: IPv4 representation structure to fill
     * \return: status of conversion
     */
    static SocketIpStatus addressToIp4(const char* address, void* ip4);
    /**
     * \brief Protocol specific open implementation, called from open.
     * \param fd: (output) file descriptor opened. Only valid on SOCK_SUCCESS. Otherwise will be invalid
     * \return status of open
     */
    virtual SocketIpStatus openProtocol(NATIVE_INT_TYPE& fd) = 0;
    /**
     * \brief Protocol specific implementation of send.  Called directly with retry from send.
     * \param data: data to send
     * \param size: size of data to send
     * \return: size of data sent, or -1 on error.
     */
    virtual I32 sendProtocol(const U8* const data, const U32 size) = 0;

    /**
     * \brief Protocol specific implementation of recv.  Called directly with error handling from recv.
     * \param data: data pointer to fill
     * \param size: size of data buffer
     * \return: size of data received, or -1 on error.
     */
    virtual I32 recvProtocol( U8* const data, const U32 size) = 0;

    Os::Mutex m_lock;
    NATIVE_INT_TYPE m_fd;
    U32 m_timeoutSeconds;
    U32 m_timeoutMicroseconds;
    U16 m_port;  //!< IP address port used
    bool m_open; //!< Have we successfully opened
    char m_hostname[SOCKET_MAX_HOSTNAME_SIZE];  //!< Hostname to supply
};
}  // namespace Drv

#endif /* DRV_SOCKETIPDRIVER_SOCKETHELPER_HPP_ */
