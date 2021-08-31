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

#include <Drv/SocketIpDriver/SocketIpDriverComponentImpl.hpp>
#include <SocketIpDriverCfg.hpp>
#include <Fw/Logger/Logger.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Fw/Types/Assert.hpp>
#include <Os/TaskString.hpp>
#include <Os/Log.hpp>

namespace Drv {

  //!< Storage for our keep-alive data
  const char KEEPALIVE_CONST[] = KEEPALIVE_DATA;
  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SocketIpDriverComponentImpl ::
    SocketIpDriverComponentImpl(
        const char *const compName
    ) : SocketIpDriverComponentBase(compName),
        m_buffer(m_backing_data, sizeof(m_buffer)),
        m_stop(false)
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

  SocketIpStatus SocketIpDriverComponentImpl :: configure(
          const char* hostname,
          U16 port,
          const bool send_udp,
          const U32 timeout_seconds,
          const U32 timeout_microseconds
          ) {

      return this->m_helper.configure(hostname,port,send_udp,timeout_seconds,timeout_microseconds);
  }

  void SocketIpDriverComponentImpl::readTask(void* pointer) {
      FW_ASSERT(pointer);
      SocketIpStatus status = SOCK_SUCCESS;
      SocketIpDriverComponentImpl* self = reinterpret_cast<SocketIpDriverComponentImpl*>(pointer);
      do {
          // Open a network connection if it has not already been open
          if (not self->m_helper.isOpened()
                  and (self->m_helper.open() != SOCK_SUCCESS)
                  and not self->m_stop) {
              Os::Task::delay(PRE_CONNECTION_RETRY_INTERVAL_MS);
          }

          // If the network connection is open, read from it
          if (self->m_helper.isOpened()) {
              U8* data = self->m_buffer.getData();
              FW_ASSERT(data);
              I32 size = 0;
              status = self->m_helper.recv(data,size);
              if (status != SOCK_SUCCESS &&
                  status != SOCK_INTERRUPTED_TRY_AGAIN) {
                  self->m_helper.close();
              } else {
                  // Ignore KEEPALIVE data and send out any other data.
                  if (memcmp(data, KEEPALIVE_CONST,
                         (size > static_cast<I32>(sizeof(KEEPALIVE_CONST)) - 1) ? sizeof(KEEPALIVE_CONST) -1 : size) != 0) {
                      self->m_buffer.setSize(size);
                      self->recv_out(0, self->m_buffer);
                  }
              }
          }
      }
      // As long as not told to stop, and we are successful interrupted or ordered to retry, keep receiving
      while(not self->m_stop && (status == SOCK_SUCCESS ||
            status == SOCK_INTERRUPTED_TRY_AGAIN || RECONNECT_AUTOMATICALLY != 0));
  }

  void SocketIpDriverComponentImpl::startSocketTask(
        NATIVE_INT_TYPE priority,
        NATIVE_INT_TYPE stack,
        const char* host,
        U16 port,
        NATIVE_INT_TYPE cpuAffinity
  )
  {
      Os::TaskString name("IpSocketRead");
      // Do not restart task
      if (not m_recvTask.isStarted()) {
          this->configure(host,port);
          // Start by opening the socket
          if (not this->m_helper.isOpened()) {
              SocketIpStatus stat = this->m_helper.open();
              if (stat != SOCK_SUCCESS) {
                  Fw::Logger::logMsg("Unable to open socket: %d\n",stat);
              }
          }
          Os::Task::TaskStatus stat = m_recvTask.start(name, 0, priority, stack,
                                                       SocketIpDriverComponentImpl::readTask, this, cpuAffinity);
          FW_ASSERT(Os::Task::TASK_OK == stat, static_cast<NATIVE_INT_TYPE>(stat));
      }
  }

  Os::Task::TaskStatus SocketIpDriverComponentImpl :: joinSocketTask(void** value_ptr) {
      // provide return value of thread if value_ptr is not NULL
      return m_recvTask.join(value_ptr);
  }

  void SocketIpDriverComponentImpl :: exitSocketTask() {
      this->m_stop = true;
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
      U32 size = fwBuffer.getSize();
      U8* data = fwBuffer.getData();
      FW_ASSERT(data);
      this->m_helper.send(data,size);
  }
} // end namespace Svc
