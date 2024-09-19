// ======================================================================
// \title  TcpServerComponentImpl.hpp
// \author mstarch
// \brief  hpp file for TcpServerComponentImpl component implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TcpServerComponentImpl_HPP
#define TcpServerComponentImpl_HPP

#include <IpCfg.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Drv/Ip/SocketComponentHelper.hpp>
#include <Drv/Ip/TcpServerSocket.hpp>
#include "Drv/TcpServer/TcpServerComponentAc.hpp"

namespace Drv {

class TcpServerComponentImpl : public TcpServerComponentBase, public SocketComponentHelper {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    /**
     * \brief construct the TcpServer component.
     * \param compName: name of this component
     */
    TcpServerComponentImpl(const char* const compName);

    /**
     * \brief Destroy the component
     */
    ~TcpServerComponentImpl();

    // ----------------------------------------------------------------------
    // Helper methods to start and stop socket
    // ----------------------------------------------------------------------

    /**
     * \brief Configures the TcpClient settings but does not open the connection
     *
     * The TcpClientComponent needs to connect to a remote TCP server. This call configures the hostname, port and
     * send timeouts for that socket connection. This call should be performed on system startup before recv or send
     * are called. Note: hostname must be a dot-notation IP address of the form "x.x.x.x". DNS translation is left up
     * to the user.
     *
     * \param hostname: ip address of remote tcp server in the form x.x.x.x
     * \param port: port of remote tcp server
     * \param send_timeout_seconds: send timeout seconds component. Defaults to: SOCKET_TIMEOUT_SECONDS
     * \param send_timeout_microseconds: send timeout microseconds component. Must be less than 1000000. Defaults to:
     * SOCKET_TIMEOUT_MICROSECONDS
     * \return status of the configure
     */
    SocketIpStatus configure(const char* hostname,
                             const U16 port,
                             const U32 send_timeout_seconds = SOCKET_SEND_TIMEOUT_SECONDS,
                             const U32 send_timeout_microseconds = SOCKET_SEND_TIMEOUT_MICROSECONDS);

    /**
     * \brief get the port being listened on
     *
     * Most useful when listen was configured to use port "0", this will return the port used for listening after a port
     * has been determined. Will return 0 if the connection has not been setup.
     *
     * \return receive port
     */
    U16 getListenPort();

  PROTECTED:
    // ----------------------------------------------------------------------
    // Implementations for socket read task virtual methods
    // ----------------------------------------------------------------------

    /**
     * \brief returns a reference to the socket handler
     *
     * Gets a reference to the current socket handler in order to operate generically on the IpSocket instance. Used for
     * receive, and open calls. This socket handler will be a TcpClient.
     *
     * \return IpSocket reference
     */
    IpSocket& getSocketHandler();

    /**
     * \brief returns a buffer to fill with data
     *
     * Gets a reference to a buffer to fill with data. This allows the component to determine how to provide a
     * buffer and the socket read task just fills said buffer.
     *
     * \return Fw::Buffer to fill with data
     */
    Fw::Buffer getBuffer();

    /**
     * \brief sends a buffer to be filled with data
     *
     * Sends the buffer gotten by getBuffer that has now been filled with data. This is used to delegate to the
     * component how to send back the buffer. Ignores buffers with error status error.
     *
     * \return Fw::Buffer filled with data to send out
     */
    void sendBuffer(Fw::Buffer buffer, SocketIpStatus status);

    /**
     * \brief called when the IPv4 system has been connected
    */
    void connected();


  PRIVATE:

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    /**
     * \brief Send data out of the TcpClient
     *
     * Passing data to this port will send data from the TcpClient to whatever TCP server this component has connected
     * to. Should the socket not be opened or was disconnected, then this port call will return SEND_RETRY and critical
     * transmissions should be retried. SEND_ERROR indicates an unresolvable error. SEND_OK is returned when the data
     * has been sent.
     *
     * Note: this component delegates the reopening of the socket to the read thread and thus the caller should retry
     * after the read thread has attempted to reopen the port but does not need to reopen the port manually.
     *
     * \param portNum: fprime port number of the incoming port call
     * \param fwBuffer: buffer containing data to be sent
     * \return SEND_OK on success, SEND_RETRY when critical data should be retried and SEND_ERROR upon error
     */
    Drv::SendStatus send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer);

    Drv::TcpServerSocket m_socket; //!< Socket implementation
};

}  // end namespace Drv

#endif // end TcpServerComponentImpl
