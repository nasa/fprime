// ======================================================================
// \title  SocketIpDriverComponentImpl.hpp
// \author mstarch
// \brief  hpp file for SocketIpDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SocketIpDriver_HPP
#define SocketIpDriver_HPP

#include <Fw/Types/BasicTypes.hpp>
#include "Drv/SocketIpDriver/SocketIpDriverComponentAc.hpp"
#include <Drv/SocketIpDriver/SocketIpDriverCfg.hpp>
// Includes for the IP layer
#ifdef TGT_OS_TYPE_VXWORKS
    #include <inetLib.h>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    #include <arpa/inet.h>
#else
    #error OS not supported for IP Socket Communications
#endif

namespace Drv {

  class SocketIpDriverComponentImpl :
    public SocketIpDriverComponentBase
  {
    public:

      enum SocketIpStatus {
          SUCCESS = 0,
          FAILED_TO_GET_SOCKET = -1,
          FAILED_TO_GET_HOST_IP = -2,
          INVALID_IP_ADDRESS = -3,
          FAILED_TO_CONNECT = -4,
          FAILED_TO_SET_SOCKET_OPTIONS = -5,
          INTERRUPTED_TRY_AGAIN = -6,
          READ_ERROR = -7,
          READ_DISCONNECTED = -8
      };
      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SocketIpDriver
      //!
      SocketIpDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#endif
        ,
        const bool send_udp = SOCKET_SEND_UDP, /*!< Send down using UDP. Default: read from configuration HPP*/
        const U32 timeout_seconds = SOCKET_TIMEOUT_SECONDS, /*!< Timeout(S). Default: from configuration HPP*/
        const U32 timeout_microseconds = SOCKET_TIMEOUT_MICROSECONDS /*!< Timeout(uS). Default: from configuration HPP*/
      );

      //! Initialize object SocketIpDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SocketIpDriver
      //!
      ~SocketIpDriverComponentImpl(void);

      //! Open up the socket port, ready for communications
      //!
      SocketIpStatus open();

      //! The task required to read from the socket
      //!
      static void readTask(void* ptr);

      //! Receive on setup port
      //! return: true to stop the task, false otherwise
      SocketIpStatus receive();

      //! Start the socket task
      //!
      void startSocketTask(
              NATIVE_INT_TYPE priority, //!< Priority of the task to start
              NATIVE_INT_TYPE stack,    //!< Stack size for the task to start
              const char* host,         //!< Hostname to connect to
              U16 port,                 //!< Port to connect to
              NATIVE_INT_TYPE cpuAffinity = -1 //!< CPU affinity of the task to start
      );

    PRIVATE:

      SocketIpStatus openProtocol(NATIVE_INT_TYPE protocol, bool isInput = true);

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for downlink
      //!
      void send_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );
      Os::Task m_recvTask;           //!< Os::Task to start for reciving data
      Fw::Buffer m_buffer;           //!< Fw::Buffer used to pass data
      struct sockaddr_in m_udpAddr;  //!< UDP server address, maybe unused
      NATIVE_INT_TYPE m_socketInFd;  //!< Input file descriptor, always TCP
      NATIVE_INT_TYPE m_socketOutFd; //!< Output file descriptor, always UDP
      const char* m_hostname;        //!< Hostname to supply
      U16 m_port;                    //!< IP address port used
      U8 m_backing_data[MAX_RECV_BUFFER_SIZE]; //!< Buffer used to store data
      bool m_stop; //!< Stop the receiving port

      // Configuration values
      const bool m_send_udp;
      const U32 m_timeout_seconds;
      const U32 m_timeout_microseconds;

    };

} // end namespace Svc

#endif
