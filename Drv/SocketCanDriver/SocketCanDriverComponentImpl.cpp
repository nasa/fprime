// ======================================================================
// \title  SocketCanDriverComponentImpl.cpp
// \author csommer
// \brief  cpp file for SocketCanDriver component implementation class
//
// \copyright
// Copyright 2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
// Includes for SocketCAN
#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN || TGT_OS_TYPE_RTEMS
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <linux/can.h>
    #include <linux/can/raw.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
#else
    #error OS not supported for SocketCAN Communications
#endif

#include <Drv/SocketCanDriver/SocketCanDriverComponentImpl.hpp>
#include <Drv/SocketCanDriver/SocketCanDriverCfg.hpp>
#include <Fw/Logger/Logger.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Os/Log.hpp>

extern "C" {
    #include <stdio.h>
};

namespace Drv 
{
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SocketCanDriverComponentImpl ::
    SocketCanDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
#endif
        U32 timeout_seconds,
        U32 timeout_microseconds
    ) :
#if FW_OBJECT_NAMES == 1
      SocketCanDriverComponentBase(compName)
#else
      SocketCanDriverComponentBase(void)
#endif
    ,
    m_buffer(0xcafe, 0xcafe, reinterpret_cast<U64>(m_backing_data), sizeof(m_buffer)),
    m_socketFd(-1),
    m_interface(""),
    m_mask(0),
    m_filter(0),
    // Configuration values
    m_timeout_seconds(timeout_seconds),
    m_timeout_microseconds(timeout_microseconds)
  { 
  }

  void SocketCanDriverComponentImpl :: init(const NATIVE_INT_TYPE instance)
  {
      SocketCanDriverComponentBase::init(instance);
  }

  SocketCanDriverComponentImpl :: ~SocketCanDriverComponentImpl(void)
  {
  }

  SocketCanDriverComponentImpl::SocketCanStatus SocketCanDriverComponentImpl :: open() 
  {   
      // Close open sockets, to force a re-open
      (void) close(m_socketFd);
      m_socketFd = -1;

      NATIVE_INT_TYPE socketFd = -1;
      struct sockaddr_can addr;
      struct ifreq ifr;

      // Clear existing file descriptors
      (void) close(m_socketFd);
      m_socketFd = -1;

      /* Open raw SocketCAN */
      if((socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW))==-1)
      {
        (void) close(socketFd);
        Fw::Logger::logMsg("[CAN] socket %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
        return FAILED_TO_GET_SOCKET;
      }

      /* Set given interface as interface */
      strcpy(ifr.ifr_name, m_interface);
      if(ioctl(socketFd, SIOCGIFINDEX, &ifr)<0)
      {
        (void) close(socketFd);
        Fw::Logger::logMsg("[CAN] ioctl SIOCGIFINDEX: %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
        return FAILED_TO_GET_INTERFACE;
      }

      /* Setup address for bind */
      addr.can_family = AF_CAN;
      addr.can_ifindex = ifr.ifr_ifindex;

      /* Bind the socket to the given interface */
      if(bind(socketFd, (struct sockaddr *)&addr, sizeof(addr))==-1)
      {
        (void) close(socketFd);
        Fw::Logger::logMsg("[CAN] bind: %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
        return FAILED_TO_BIND_INTERFACE;
      }

      /* Sets filter
       * <received_can_id> & mask == can_filter & mask */
      struct can_filter rfilter[1];
      rfilter[0].can_id   = m_filter;
      rfilter[0].can_mask = m_mask;
      if(setsockopt(socketFd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter))==-1)
      {
        (void) close(socketFd);
        Fw::Logger::logMsg("[CAN] setsockopt %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
        return FAILED_TO_SET_FILTER_OPTIONS;
      }

      // Set timeout socket option
      struct timeval timeout;
      timeout.tv_sec  = m_timeout_seconds;
      timeout.tv_usec = m_timeout_microseconds;
      if (setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
      {
          (void) close(socketFd);
          Fw::Logger::logMsg("[CAN] setsockopt %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
          return FAILED_TO_SET_TIMEOUT_OPTIONS;
      }

      // Set the member socket variable
      m_socketFd = socketFd;

      Fw::Logger::logMsg("[CAN] Connected to %s with mask 0x%08X and filter 0x%08X\n", 
          reinterpret_cast<POINTER_CAST>(m_interface),
          m_mask,
          m_filter);
      
      return SocketCanDriverComponentImpl::SUCCESS;
  }

  void SocketCanDriverComponentImpl :: readTask(void* pointer) 
  {
      FW_ASSERT(pointer != NULL);
      SocketCanDriverComponentImpl::SocketCanStatus status = SocketCanDriverComponentImpl::SUCCESS;
      SocketCanDriverComponentImpl* self = reinterpret_cast<SocketCanDriverComponentImpl*>(pointer);
      do 
      {
          // Open a network connection, if it has not already been open
          if (self->m_socketFd == -1 && self->open() != SocketCanDriverComponentImpl::SUCCESS && not self->m_stop) 
          {
              Os::Task::delay(CAN_PRE_CONNECTION_RETRY_INTERVAL_MS);
          }
          // If the network connection is open, read from it
          if (self->m_socketFd != -1) 
          {
              status = self->receive();
              if (status != SocketCanDriverComponentImpl::SUCCESS &&
                  status != SocketCanDriverComponentImpl::INTERRUPTED_TRY_AGAIN) 
              {
                  (void) close(self->m_socketFd);
                  self->m_socketFd = -1;
              }
          }
      }
      // As long as not told to stop, and we are successful interrupted or ordered to retry, keep receiving
      while(not self->m_stop && (status == SocketCanDriverComponentImpl::SUCCESS ||
            status == SocketCanDriverComponentImpl::INTERRUPTED_TRY_AGAIN || CAN_RECONNECT_AUTOMATICALLY != 0));
  }

  SocketCanDriverComponentImpl::SocketCanStatus SocketCanDriverComponentImpl :: receive() 
  {
      U8* data = reinterpret_cast<U8*>(m_buffer.getdata());
      SocketCanDriverComponentImpl::SocketCanStatus status = SocketCanDriverComponentImpl::SUCCESS;
      
      // Attempt to recv out data
      I32 recd = read(m_socketFd, data, sizeof(struct can_frame));

      Fw::Logger::logMsg("[CAN] Receiving data\n");

      // Error returned, and it wasn't an interrupt, nor a reset
      if (recd == -1 && errno != EINTR && errno != ECONNRESET) 
      {
          Fw::Logger::logMsg("[CAN] recv failed: %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
          status = READ_ERROR; // Stop recv task on error
      }
      // Error is EINTR, just try again
      else if (recd == -1 && errno == EINTR) 
      {
          status = INTERRUPTED_TRY_AGAIN;
      }
      // Zero bytes read means a closed socket
      else if (recd == 0 || errno == ECONNRESET) 
      {
          status = READ_DISCONNECTED;
      }
      else if (recd != sizeof(struct can_frame)) 
      {
          Fw::Logger::logMsg("[CAN] read incomplete: expected 16, received %d\n", recd);
          status = READ_INCOMPLETE;
      }
      else
      {
          m_buffer.setsize(recd);
          // Expected to be a guarded or sync output to ensure processing immediately
          canRecv_out(0, m_buffer);
      }
      return status;
  }

  void SocketCanDriverComponentImpl :: startSocketTask(
        NATIVE_INT_TYPE priority, 
        NATIVE_INT_TYPE stack, 
        const char* interface,
        U32 mask,
        U32 filter, 
        NATIVE_INT_TYPE cpuAffinity
  )
  {
    Fw::EightyCharString name(interface);
     
    // Do not restart task
    if (not m_recvTask.isStarted()) 
    {
      m_interface = interface;
      m_mask = mask;
      m_filter = filter;

      // Start by opening the socket
      if (m_socketFd == -1) 
      {
          open();
      }
      Os::Task::TaskStatus stat = m_recvTask.start(name, 0, priority, stack, SocketCanDriverComponentImpl::readTask, this, cpuAffinity);
      FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
    }
  }

  void SocketCanDriverComponentImpl :: exitSocketTask()
  {
      this->m_stop = true;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SocketCanDriverComponentImpl :: canSend_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer &fwBuffer)
  {
      U32 total = 0;
      U32 size = fwBuffer.getsize();
      U8* data = reinterpret_cast<U8*>(fwBuffer.getdata());

      // Prevent transmission before connection, or after a disconnect
      if (m_socketFd == -1) 
      {
          return;
      }

      // Verify that the data to send corresponds indeed to a can_frame (16 bytes even with dlc=0)
      if(size != sizeof(struct can_frame))
      {
          Fw::Logger::logMsg("[CAN] Data to send error, expected %d, got %d\n", sizeof(struct can_frame), size);
          return;
      }

      // Attempt to send out data
      for (U32 i = 0; i < CAN_MAX_SEND_ITERATIONS && total < size; i++)
      {
          I32 sent = 0;
            
          /* Write the data on the socket */
          sent = write(m_socketFd, data, size);

          // Error is EINTR or ENOBUFS, just try again
          if (sent == -1 && (errno == EINTR || errno == ENOBUFS))
          {
              continue;
          }
          // Error returned, and it wasn't an interrupt
          else if (sent == -1 && errno != EINTR)
          {
              Fw::Logger::logMsg("[CAN] Send failed: %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
              break;
          }
          // Successful write
          else 
          {
              total += sent;
          }
      }
      Fw::Logger::logMsg("[CAN] Done writing\n");

  }
} // end namespace Drv
