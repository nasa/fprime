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
#include <Fw/Buffer/Buffer.hpp>
#include "Drv/SocketIpDriver/SocketIpDriverComponentAc.hpp"
#include <SocketIpDriverCfg.hpp>
#include <Drv/SocketIpDriver/SocketHelper.hpp>
#include <Drv/SocketIpDriver/SocketIpDriverTypes.hpp>
#include <Os/Task.hpp>

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

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SocketIpDriver
      //!
      SocketIpDriverComponentImpl(
          const char *const compName /*!< The component name*/
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
      SocketIpStatus configure(
              const char* hostname, /*!< Hostname of remote server */
              U16 port, /*!< Port of remote server */
              const bool send_udp = SOCKET_SEND_UDP, /*!< Send down using UDP. Default: read from configuration HPP*/
              const U32 timeout_seconds = SOCKET_TIMEOUT_SECONDS, /*!< Timeout(S). Default: from configuration HPP*/
              const U32 timeout_microseconds = SOCKET_TIMEOUT_MICROSECONDS /*!< Timeout(uS). Default: from configuration HPP*/
              );

      //! The task required to read from the socket
      //!
      static void readTask(void* ptr);

      //! Start the socket task
      //!
      void startSocketTask(
              NATIVE_INT_TYPE priority, //!< Priority of the task to start
              NATIVE_INT_TYPE stack,    //!< Stack size for the task to start
              const char* host,         //!< Hostname to connect to
              U16 port,                 //!< Port to connect to
              NATIVE_INT_TYPE cpuAffinity = -1 //!< CPU affinity of the task to start
      );

      //! Task to join nondetached pthreads 
      //!
      Os::Task::TaskStatus joinSocketTask(void** value_ptr);

      //! Set the stop flag on the thread's loop such that it will shutdown promptly
      //!
      void exitSocketTask();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for downlink
      //!
      void send_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      // socket helper instance
      SocketHelper m_helper;

      Os::Task m_recvTask;           //!< Os::Task to start for receiving data
      Fw::Buffer m_buffer;           //!< Fw::Buffer used to pass data
      U8 m_backing_data[MAX_RECV_BUFFER_SIZE]; //!< Buffer used to store data
      bool m_stop; //!< Stop the receiving port


    };

} // end namespace Svc

#endif
