// ======================================================================
// \title  UdpSenderImpl.cpp
// \author tcanham
// \brief  cpp file for UdpSender component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Svc/UdpSender/UdpSenderComponentImpl.hpp>
#include <FpConfig.hpp>
#include <sys/types.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__)
#define DEBUG_PRINT(x,...)

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UdpSenderComponentImpl ::
    UdpSenderComponentImpl(
        const char *const compName
    ) : UdpSenderComponentBase(compName),
        m_fd(-1),
        m_packetsSent(0),
        m_bytesSent(0),
        m_seq(0)
  {

  }

  void UdpSenderComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE msgSize,
        const NATIVE_INT_TYPE instance
    )
  {
    UdpSenderComponentBase::init(queueDepth, msgSize, instance);
  }

  UdpSenderComponentImpl ::
    ~UdpSenderComponentImpl()
  {
      if (this->m_fd != -1) {
          close(this->m_fd);
      }
  }

  void UdpSenderComponentImpl::open(
          const char* addr,  /*!< server address */
          const char* port /*!< server port */
  ) {

      FW_ASSERT(addr);
      FW_ASSERT(port);
      // open UDP connection
      this->m_fd = socket(AF_INET, SOCK_DGRAM, 0);
      if (-1 == this->m_fd) {
          Fw::LogStringArg arg(strerror(errno));
          this->log_WARNING_HI_US_SocketError(arg);
          return;
      }

      /* fill in the server's address and data */
      memset(&m_servAddr, 0, sizeof(m_servAddr));
      m_servAddr.sin_family = AF_INET;
      m_servAddr.sin_port = htons(atoi(port));
      inet_aton(addr , &m_servAddr.sin_addr);

      Fw::LogStringArg arg(addr);
      this->log_ACTIVITY_HI_US_PortOpened(arg,atoi(port));

  }



  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UdpSenderComponentImpl ::
    Sched_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      this->tlmWrite_US_BytesSent(this->m_bytesSent);
      this->tlmWrite_US_PacketsSent(this->m_packetsSent);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void UdpSenderComponentImpl ::
    PortsIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
      // return if we never successfully created the socket
      if (-1 == this->m_fd) {
          return;
      }

      DEBUG_PRINT("PortsIn_handler: %d\n",portNum);
      Fw::SerializeStatus stat;
      m_sendBuff.resetSer();

      // serialize sequence number
      stat = m_sendBuff.serialize(this->m_seq++);
      FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,stat);
      // serialize port call
      stat = m_sendBuff.serialize(static_cast<U8>(portNum));
      FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,stat);
      // serialize port arguments buffer
      stat = m_sendBuff.serialize(Buffer);
      FW_ASSERT(Fw::FW_SERIALIZE_OK == stat,stat);
      // send on UDP socket
      DEBUG_PRINT("Sending %d bytes\n",m_sendBuff.getBuffLength());
      ssize_t sendStat = sendto(this->m_fd,
              m_sendBuff.getBuffAddr(),
              m_sendBuff.getBuffLength(),
              0,
              reinterpret_cast<struct sockaddr *>(&m_servAddr),
              sizeof(m_servAddr));
      if (-1 == sendStat) {
          Fw::LogStringArg arg(strerror(errno));
          this->log_WARNING_HI_US_SendError(arg);
      } else {
          FW_ASSERT((int)m_sendBuff.getBuffLength() == sendStat,(int)m_sendBuff.getBuffLength(),sendStat,portNum);
          this->m_packetsSent++;
          this->m_bytesSent += sendStat;
      }
  }

#ifdef BUILD_UT
  UdpSerialBuffer& UdpSenderComponentImpl::UdpSerialBuffer::operator=(const Svc::UdpSenderComponentImpl::UdpSerialBuffer& other) {
      this->resetSer();
      this->serialize(other.getBuffAddr(),other.getBuffLength(),true);
      return *this;
  }

  UdpSenderComponentImpl::UdpSerialBuffer::UdpSerialBuffer(
          const Fw::SerializeBufferBase& other) : Fw::SerializeBufferBase() {
      FW_ASSERT(sizeof(this->m_buff)>= other.getBuffLength(),sizeof(this->m_buff),other.getBuffLength());
      memcpy(this->m_buff,other.getBuffAddr(),other.getBuffLength());
      this->setBuffLen(other.getBuffLength());
  }

  UdpSenderComponentImpl::UdpSerialBuffer::UdpSerialBuffer(
          const UdpSenderComponentImpl::UdpSerialBuffer& other) : Fw::SerializeBufferBase() {
      FW_ASSERT(sizeof(this->m_buff)>= other.getBuffLength(),sizeof(this->m_buff),other.getBuffLength());
      memcpy(this->m_buff,other.m_buff,other.getBuffLength());
      this->setBuffLen(other.getBuffLength());
  }

  UdpSenderComponentImpl::UdpSerialBuffer::UdpSerialBuffer(): Fw::SerializeBufferBase() {

  }

#endif


} // end namespace Svc
