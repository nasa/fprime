// ======================================================================
// \title  SocketCanDriverComponentImpl.hpp
// \author csommer
// \brief  hpp file for SocketCanDriver component implementation class
//
// \copyright
// Copyright 2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SocketCanDriver_HPP
#define SocketCanDriver_HPP

#include <Fw/Types/BasicTypes.hpp>
#include "Drv/SocketCanDriver/SocketCanDriverComponentAc.hpp"
#include <Drv/SocketCanDriver/SocketCanDriverCfg.hpp>

namespace Drv {

  class SocketCanDriverComponentImpl :
    public SocketCanDriverComponentBase
  {
    public:

      enum SocketCanStatus {
          SUCCESS = 0,
          FAILED_TO_GET_SOCKET = -1,
          FAILED_TO_GET_INTERFACE = -2,
          FAILED_TO_BIND_INTERFACE = -3,
          FAILED_TO_SET_FILTER_OPTIONS = -4,
          FAILED_TO_SET_TIMEOUT_OPTIONS = -5,
          INTERRUPTED_TRY_AGAIN = -6,
          READ_ERROR = -7,
          READ_DISCONNECTED = -8,
          READ_INCOMPLETE = -9
      };
      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SocketCanDriver
      //!
      SocketCanDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#endif
        ,
        const U32 timeout_seconds = SOCKETCAN_TIMEOUT_SECONDS,          /*!< Timeout(S). Default: from configuration HPP*/
        const U32 timeout_microseconds = SOCKETCAN_TIMEOUT_MICROSECONDS /*!< Timeout(uS). Default: from configuration HPP*/
      );

      //! Initialize object SocketCanDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SocketCanDriver
      //!
      ~SocketCanDriverComponentImpl(void);

      //! Open up the socket port, ready for communications
      //!
      SocketCanStatus open();

      //! The task required to read from the socket
      //!
      static void readTask(void* ptr);

      //! Receive on setup port
      //! return: true to stop the task, false otherwise
      SocketCanStatus receive();

      //! Start the socket task
      //!
      void startSocketTask(
              NATIVE_INT_TYPE priority,                 //!< Priority of the task to start
              NATIVE_INT_TYPE stack,                    //!< Stack size for the task to start
              const char* interface,                    //!< CAN interface to connect to
              U32 mask = SOCKETCAN_DEFAULT_MASK,        //!< CAN mask to use on received identifiers
              U32 filter = SOCKETCAN_DEFAULT_FILTER,    //!< CAN filter to use on received identifiers
              NATIVE_INT_TYPE cpuAffinity = -1          //!< CPU affinity of the task to start
      );

      //! Set the stop flag on the thread's loop such that it will shutdown promptly
      //!
      void exitSocketTask();

    PRIVATE:

      SocketCanStatus openProtocol(NATIVE_INT_TYPE protocol, bool isInput = true);

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for downlink
      //!
      void canSend_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer &fwBuffer);
      
      Os::Task m_recvTask;                          //!< Os::Task to start for receiving data
      Fw::Buffer m_buffer;                          //!< Fw::Buffer used to pass data
      NATIVE_INT_TYPE m_socketFd;                   //!< In/out SocketCAN file descriptor
      U8 m_backing_data[CAN_MAX_RECV_BUFFER_SIZE];  //!< Buffer used to store data
      bool m_stop;                                  //!< Stop the receiving port
      const char* m_interface;                      //!< Interface to use (can0, can1, etc.)


      // Configuration values
      U32 m_mask;
      U32 m_filter;
      const U32 m_timeout_seconds;
      const U32 m_timeout_microseconds;

    };

} // end namespace Svc

#endif
