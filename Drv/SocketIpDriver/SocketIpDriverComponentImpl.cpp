// ======================================================================
// \title  SocketIpDriverComponentImpl.cpp
// \author mstarch
// \brief  cpp file for SocketIpDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
// Includes for the IP layer
#ifdef TGT_OS_TYPE_VXWORKS
    #include <hostLib.h>
    #include <ioLib.h>
    #include <vxWorks.h>
    #include <sockLib.h>
    #include <fioLib.h>
    #include <taskLib.h>
    #include <sysLib.h>
    #include <errnoLib.h>
#elif defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
#else
    #error OS not supported for IP Socket Communications
#endif

#include <Drv/SocketIpDriver/SocketIpDriverComponentImpl.hpp>
#include <Drv/SocketIpDriver/SocketIpDriverCfg.hpp>
#include <Fw/Logger/Logger.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Os/Log.hpp>

extern "C" {
    #include <stdio.h>
};

namespace Drv {

  //!< Storage for our keep-alive data
  const char KEEPALIVE_CONST[] = KEEPALIVE_DATA;
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SocketIpDriverComponentImpl ::
    SocketIpDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
#endif
        bool send_udp,
        U32 timeout_seconds,
        U32 timeout_microseconds
    ) :
#if FW_OBJECT_NAMES == 1
      SocketIpDriverComponentBase(compName)
#else
      SocketIpDriverComponentBase(void)
#endif
    ,
    m_buffer(0xbeef, 0xbeef, reinterpret_cast<U64>(m_backing_data), sizeof(m_buffer)),
    m_socketInFd(-1),
    m_socketOutFd(-1),
    m_hostname(""),
    m_port(0),
    m_stop(false),
    // Configuration values
    m_send_udp(send_udp),
    m_timeout_seconds(timeout_seconds),
    m_timeout_microseconds(timeout_microseconds)
  { }

  void SocketIpDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    SocketIpDriverComponentBase::init(instance);
  }

  SocketIpDriverComponentImpl ::
    ~SocketIpDriverComponentImpl(void)
  {}

  SocketIpDriverComponentImpl::SocketIpStatus SocketIpDriverComponentImpl :: open() {
      SocketIpDriverComponentImpl::SocketIpStatus status = SocketIpDriverComponentImpl::SUCCESS;
      // Only the input (TCP) socket needs closing
      (void) close(m_socketInFd); // Close open sockets, to force a re-open
      m_socketInFd = -1;
      // Open a TCP socket for incoming commands, and outgoing data if not using UDP
      if ((status = openProtocol(SOCK_STREAM)) != SocketIpDriverComponentImpl::SUCCESS) {
          return status;
      }
      // If we need UDP sending, attempt to open UDP
      if (m_send_udp && m_socketOutFd == -1 && (status = openProtocol(SOCK_DGRAM, false)) != SocketIpDriverComponentImpl::SUCCESS) {
          (void) close(m_socketInFd);
          return status;
      }
      // Not sending UDP, reuse our input TCP socket
      else if (!m_send_udp) {
          m_socketOutFd = m_socketInFd;
      }
      // Coding error, if not successful here
      FW_ASSERT(status == SocketIpDriverComponentImpl::SUCCESS);
      return status;
  }

  SocketIpDriverComponentImpl::SocketIpStatus SocketIpDriverComponentImpl :: openProtocol(
      NATIVE_INT_TYPE protocol,
      bool isInput
  ) {
      NATIVE_INT_TYPE socketFd = -1;
      struct sockaddr_in address;

      // Clear existing file descriptors
      if (isInput) {
          (void) close(m_socketInFd);
          m_socketInFd = -1;
      } else {
          (void) close(m_socketOutFd);
          m_socketOutFd = -1;
      }
      // Acquire a socket, or return error
      if ((socketFd = socket(AF_INET, protocol, 0)) == -1) {
          return SocketIpDriverComponentImpl::FAILED_TO_GET_SOCKET;
      }
      // Set timeout socket option
      struct timeval timeout;
      timeout.tv_sec = m_timeout_seconds;
      timeout.tv_usec = m_timeout_microseconds;
      // set socket write to timeout after 1 sec
      if (setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
          (void) close(socketFd);
          return FAILED_TO_SET_SOCKET_OPTIONS;
      }
      // Set up the address port and name
      address.sin_family = AF_INET;
      address.sin_port = htons(m_port);
#if defined TGT_OS_TYPE_VXWORKS || TGT_OS_TYPE_DARWIN
      address.sin_len = static_cast<U8>(sizeof(struct sockaddr_in));
#endif
// Get the IP address from host
#ifdef TGT_OS_TYPE_VXWORKS
      if ((address.sin_addr.s_addr = hostGetByName(m_hostname)) == -1) {
          close(socketFd);
          return FAILED_TO_GET_HOST_IP;
      }
#else
      // Get possible IP addresses
      struct hostent *host_entry;
      if ((host_entry = gethostbyname(m_hostname)) == NULL || host_entry->h_addr_list[0] == NULL) {
          close(socketFd);
          return FAILED_TO_GET_HOST_IP;
      }
      // First IP address to socket sin_addr
      if (inet_pton(address.sin_family, m_hostname, &(address.sin_addr)) < 0) {
          close(socketFd);
          return INVALID_IP_ADDRESS;
      };
#endif
      // If TCP, connect to the socket to allow for communication
      if (protocol != SOCK_DGRAM && connect(socketFd, reinterpret_cast<struct sockaddr *>(&address),
              sizeof(address)) < 0) {
          close(socketFd);
          return FAILED_TO_CONNECT;
      }
      // Store the UDP address for later
      else if (protocol == SOCK_DGRAM) {
          FW_ASSERT(sizeof(m_udpAddr) == sizeof(address), sizeof(m_udpAddr), sizeof(address));
          memcpy(&m_udpAddr, &address, sizeof(m_udpAddr));
      }

      // Set the member socket variable
      if (isInput) {
          m_socketInFd = socketFd;
      } else {
          m_socketOutFd = socketFd;
      }
      Fw::Logger::logMsg("Connected to %s:%hu for %s using %s\n", reinterpret_cast<POINTER_CAST>(m_hostname), m_port,
                         reinterpret_cast<POINTER_CAST>(isInput ? "uplink" : "downlink"),
                         reinterpret_cast<POINTER_CAST>((protocol == SOCK_DGRAM) ? "udp" : "tcp"));
      return SocketIpDriverComponentImpl::SUCCESS;
  }

  void SocketIpDriverComponentImpl :: readTask(void* pointer) {
      FW_ASSERT(pointer != NULL);
      SocketIpDriverComponentImpl::SocketIpStatus status = SocketIpDriverComponentImpl::SUCCESS;
      SocketIpDriverComponentImpl* self = reinterpret_cast<SocketIpDriverComponentImpl*>(pointer);
      do {
          // Open a network connection, if it has not already been open
          if (self->m_socketInFd == -1 && self->open() != SocketIpDriverComponentImpl::SUCCESS && not self->m_stop) {
              Os::Task::delay(PRE_CONNECTION_RETRY_INTERVAL_MS);
          }
          // If the network connection is open, read from it
          if (self->m_socketInFd != -1) {
              status = self->receive();
              if (status != SocketIpDriverComponentImpl::SUCCESS &&
                  status != SocketIpDriverComponentImpl::INTERRUPTED_TRY_AGAIN) {
                  (void) close(self->m_socketInFd);
                  self->m_socketInFd = -1;
              }
          }
      }
      // As long as not told to stop, and we are successful interrupted or ordered to retry, keep receiving
      while(not self->m_stop && (status == SocketIpDriverComponentImpl::SUCCESS ||
            status == SocketIpDriverComponentImpl::INTERRUPTED_TRY_AGAIN || RECONNECT_AUTOMATICALLY != 0));
  }

    SocketIpDriverComponentImpl::SocketIpStatus SocketIpDriverComponentImpl :: receive() {
      U8* data = reinterpret_cast<U8*>(m_buffer.getdata());
      SocketIpDriverComponentImpl::SocketIpStatus status = SocketIpDriverComponentImpl::SUCCESS;
      // Attempt to recv out data
      I32 recd = recv(m_socketInFd, data, MAX_RECV_BUFFER_SIZE, SOCKET_RECV_FLAGS);
      // Error returned, and it wasn't an interrupt, nor a reset
      if (recd == -1 && errno != EINTR && errno != ECONNRESET) {
          Fw::Logger::logMsg("[ERROR] IP recv failed ERRNO: %d\n", errno);
          status = READ_ERROR; // Stop recv task on error
      }
      // Error is EINTR, just try again
      else if (recd == -1 && errno == EINTR) {
          status = INTERRUPTED_TRY_AGAIN;
      }
      // Zero bytes read means a closed socket
      else if (recd == 0 || errno == ECONNRESET) {
          status = READ_DISCONNECTED;
      }
      // Ignore KEEPALIVE data and send out any other data.
      else if (memcmp(data, KEEPALIVE_CONST,
              (recd > static_cast<I32>(sizeof(KEEPALIVE_CONST)) - 1) ? sizeof(KEEPALIVE_CONST) -1 : recd) != 0) {
          m_buffer.setsize(recd);
          // Expected to be a guarded or sync output to ensure processing immediately
          recv_out(0, m_buffer);
      }
      return status;
  }

  void SocketIpDriverComponentImpl :: startSocketTask(
        NATIVE_INT_TYPE priority,
        NATIVE_INT_TYPE stack,
        const char* host,
        U16 port,
        NATIVE_INT_TYPE cpuAffinity
  )
  {
      Fw::EightyCharString name("IpSocketRead");
      // Do not restart task
      if (not m_recvTask.isStarted()) {
          m_hostname = host;
          m_port = port;
          // Start by opening the socket
          if (m_socketInFd == -1) {
              open();
          }
          Os::Task::TaskStatus stat = m_recvTask.start(name, 0, priority, stack,
                                                       SocketIpDriverComponentImpl::readTask, this, cpuAffinity);
          FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
      }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SocketIpDriverComponentImpl ::
    send_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
      U32 total = 0;
      U32 size = fwBuffer.getsize();
      U8* data = reinterpret_cast<U8*>(fwBuffer.getdata());
      // Prevent transmission before connection, or after a disconnect
      if (m_socketOutFd == -1) {
          return;
      }
      // Attempt to send out data
      for (U32 i = 0; i < MAX_SEND_ITERATIONS && total < size; i++) {
          I32 sent = 0;
          // Output to send UDP
          if (m_send_udp) {
              sent = sendto(m_socketOutFd, data + total, size - total, SOCKET_SEND_FLAGS,
                            reinterpret_cast<struct sockaddr *>(&m_udpAddr), sizeof(m_udpAddr));
          }
          // Output to send TCP
          else {
              sent = send(m_socketOutFd, data + total, size - total, SOCKET_SEND_FLAGS);
          }
          // Error is EINTR, just try again
          if (sent == -1 && errno == EINTR) {
              continue;
          }
          // Error bad file descriptor is a close
          else if (sent == -1 && errno == EBADF) {
              Fw::Logger::logMsg("[ERROR] Server disconnected\n");
              (void) close(m_socketOutFd);
              m_socketOutFd = -1;
              break;
          }
          // Error returned, and it wasn't an interrupt
          else if (sent == -1 && errno == EINTR) {
              Fw::Logger::logMsg("[ERROR] IP send failed ERRNO: %d UDP: %d\n", errno, m_send_udp);
              break;
          }
          // Successful write
          else {
              total += sent;
          }
      }
  }
} // end namespace Svc
