// ======================================================================
// \title  UdpReceiverImpl.cpp
// \author tcanham
// \brief  cpp file for UdpReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Svc/UdpReceiver/UdpReceiverComponentImpl.hpp>
#include <FpConfig.hpp>
#include <sys/types.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <Os/TaskString.hpp>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__)
#define DEBUG_PRINT(x,...)

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  UdpReceiverComponentImpl ::
    UdpReceiverComponentImpl(
        const char *const compName
    ) : UdpReceiverComponentBase(compName),
        m_fd(-1),
        m_packetsReceived(0),
        m_bytesReceived(0),
        m_packetsDropped(0),
        m_decodeErrors(0),
        m_firstSeq(true),
        m_currSeq(0)
  {

  }

  void UdpReceiverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    UdpReceiverComponentBase::init(instance);
  }

  UdpReceiverComponentImpl ::
    ~UdpReceiverComponentImpl()
  {
      if (this->m_fd != -1) {
          close(this->m_fd);
      }
  }

  void UdpReceiverComponentImpl::open(
          const char* port
          ) {

      //create a UDP socket
      this->m_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (-1 == this->m_fd) {
          Fw::LogStringArg arg(strerror(errno));
          this->log_WARNING_HI_UR_SocketError(arg);
      }

      sockaddr_in saddr;
      // zero out the structure
      memset(&saddr, 0, sizeof(saddr));

      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(atoi(port));
      saddr.sin_addr.s_addr = htonl(INADDR_ANY);

      //bind socket to port
      NATIVE_INT_TYPE status = bind(this->m_fd , (struct sockaddr*)&saddr, sizeof(saddr));
      if (-1 == status) {
          Fw::LogStringArg arg(strerror(errno));
          this->log_WARNING_HI_UR_BindError(arg);
          close(this->m_fd);
          this->m_fd = -1;
      } else {
          this->log_ACTIVITY_HI_UR_PortOpened(atoi(port));
      }
  }

  void UdpReceiverComponentImpl::startThread(
          NATIVE_UINT_TYPE priority, /*!< read task priority */
          NATIVE_UINT_TYPE stackSize, /*!< stack size */
          NATIVE_UINT_TYPE affinity /*!< cpu affinity */
          ) {
      Os::TaskString name(this->getObjName());
      Os::Task::TaskStatus stat = this->m_socketTask.start(
              name,
              0,
              priority,
              stackSize,
              UdpReceiverComponentImpl::workerTask,
              this,
              affinity);
      FW_ASSERT(Os::Task::TASK_OK == stat,stat);
  }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void UdpReceiverComponentImpl ::
    Sched_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      this->tlmWrite_UR_BytesReceived(this->m_bytesReceived);
      this->tlmWrite_UR_PacketsReceived(this->m_packetsReceived);
      this->tlmWrite_UR_PacketsDropped(this->m_packetsDropped);
  }

  void UdpReceiverComponentImpl::workerTask(void* ptr) {
      UdpReceiverComponentImpl *compPtr = static_cast<UdpReceiverComponentImpl*>(ptr);
      while (true) {
          compPtr->doRecv();
      }
  }

  void UdpReceiverComponentImpl::doRecv() {

      // wait for data from the socket
      NATIVE_INT_TYPE psize = recvfrom(
              this->m_fd,
              this->m_recvBuff.getBuffAddr(),
              this->m_recvBuff.getBuffCapacity(),
              MSG_WAITALL,
              0,
              0);
      if (-1 == psize) {
          if (errno != EINTR) {
              Fw::LogStringArg arg(strerror(errno));
              this->log_WARNING_HI_UR_RecvError(arg);
          }
          return;
      }
      // reset buffer for deserialization
      Fw::SerializeStatus stat = this->m_recvBuff.setBuffLen(psize);
      FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, stat);

      // get sequence number
      U8 seqNum;
      stat = this->m_recvBuff.deserialize(seqNum);
      // check for deserialization error or port number too high
      if (stat != Fw::FW_SERIALIZE_OK) {
          this->log_WARNING_HI_UR_DecodeError(DECODE_SEQ,stat);
          this->m_decodeErrors++;
          return;
      }

      // track sequence number
      if (this->m_firstSeq) {
          // first time, set tracked sequence number equal to the one received
          this->m_currSeq = seqNum;
          this->m_firstSeq = false;
      } else {
          // make sure sequence number has gone up by one
          if (seqNum != ++this->m_currSeq) {
              // will only be right if it rolls over only once, but better than nothing
              U8 diff = seqNum - this->m_currSeq;
              this->m_packetsDropped += diff;
              // send EVR
              this->log_WARNING_HI_UR_DroppedPacket(diff);
              // reset to current sequence
              this->m_currSeq = seqNum;
          }
      }

      // get port number
      U8 portNum;
      stat = this->m_recvBuff.deserialize(portNum);
      // check for deserialization error or port number too high
      if (stat != Fw::FW_SERIALIZE_OK or portNum > this->getNum_PortsOut_OutputPorts()) {
          this->log_WARNING_HI_UR_DecodeError(DECODE_PORT,stat);
          this->m_decodeErrors++;
          return;
      }
      // get buffer for port

      stat = this->m_recvBuff.deserialize(this->m_portBuff);
      if (stat != Fw::FW_SERIALIZE_OK) {
          this->log_WARNING_HI_UR_DecodeError(DECODE_BUFFER,stat);
          this->m_decodeErrors++;
          return;
      }

      // call output port
      DEBUG_PRINT("Calling port %d with %d bytes.\n",portNum,this->m_portBuff.getBuffLength());
      if (this->isConnected_PortsOut_OutputPort(portNum)) {

          Fw::SerializeStatus stat = this->PortsOut_out(portNum,this->m_portBuff);

          // If had issues deserializing the data, then report it:
          if (stat != Fw::FW_SERIALIZE_OK) {

              this->log_WARNING_HI_UR_DecodeError(PORT_SEND,stat);
              this->m_decodeErrors++;
          }

          this->m_packetsReceived++;
          this->m_bytesReceived += psize;

      }
  }

#ifdef BUILD_UT
  UdpReceiverComponentImpl::UdpSerialBuffer& UdpReceiverComponentImpl::UdpSerialBuffer::operator=(const UdpReceiverComponentImpl::UdpSerialBuffer& other) {
      this->resetSer();
      this->serialize(other.getBuffAddr(),other.getBuffLength(),true);
      return *this;
  }

  UdpReceiverComponentImpl::UdpSerialBuffer::UdpSerialBuffer(
          const Fw::SerializeBufferBase& other) : Fw::SerializeBufferBase() {
      FW_ASSERT(sizeof(this->m_buff)>= other.getBuffLength(),sizeof(this->m_buff),other.getBuffLength());
      memcpy(this->m_buff,other.getBuffAddr(),other.getBuffLength());
      this->setBuffLen(other.getBuffLength());
  }

  UdpReceiverComponentImpl::UdpSerialBuffer::UdpSerialBuffer(
          const UdpReceiverComponentImpl::UdpSerialBuffer& other) : Fw::SerializeBufferBase() {
      FW_ASSERT(sizeof(this->m_buff)>= other.getBuffLength(),sizeof(this->m_buff),other.getBuffLength());
      memcpy(this->m_buff,other.m_buff,other.getBuffLength());
      this->setBuffLen(other.getBuffLength());
  }

  UdpReceiverComponentImpl::UdpSerialBuffer::UdpSerialBuffer(): Fw::SerializeBufferBase() {

  }

#endif



} // end namespace Svc
