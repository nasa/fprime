// ======================================================================
// \title  UdpReceiver.hpp
// \author tcanham
// \brief  cpp file for UdpReceiver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include <sys/types.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      UdpReceiverGTestBase("Tester", MAX_HISTORY_SIZE),
      component("UdpReceiver")
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester::openTest(const char* port) {

      this->component.open("50000");

  }

  void Tester::recvTest(const char* port) {

      this->component.open("50000");

      // send a packet

      this->sendPacket(20,"127.0.0.1","50000",26,3);

      // retrieve the packet

      this->m_sentVal = 0;
      this->m_sentPort = 0;
      this->component.doRecv();

      // verify the port call

      EXPECT_EQ(20u,this->m_sentVal);
      EXPECT_EQ(3,this->m_sentPort);

  }


  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_PortsOut_handler(
        FwIndexType portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
      this->m_sentPort = portNum;
      EXPECT_EQ(Fw::FW_SERIALIZE_OK,Buffer.deserialize(this->m_sentVal));

  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // Sched
    this->connect_to_Sched(
        0,
        this->component.get_Sched_InputPort(0)
    );

    // Tlm
    this->component.set_Tlm_OutputPort(
        0,
        this->get_from_Tlm(0)
    );

    // Time
    this->component.set_Time_OutputPort(
        0,
        this->get_from_Time(0)
    );

    // Log
    this->component.set_Log_OutputPort(
        0,
        this->get_from_Log(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0,
        this->get_from_LogText(0)
    );

    // Output serial ports
    for (FwIndexType port = 0; port < UdpReceiverComponentImpl::NUM_PORTSOUT_OUTPUT_PORTS; port++) {
        this->component.set_PortsOut_OutputPort(port,this->get_from_PortsOut(port));
    }


  // ----------------------------------------------------------------------
  // Connect serial output ports
  // ----------------------------------------------------------------------
    for (NATIVE_INT_TYPE i = 0; i < 10; ++i) {
      this->component.set_PortsOut_OutputPort(
          i,
          this->get_from_PortsOut(i)
      );
    }



  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

  void Tester::sendPacket(U32 val, const char* addr, const char* port, NATIVE_UINT_TYPE seq, NATIVE_UINT_TYPE portNum) {
      // open UDP connection
      NATIVE_INT_TYPE fd = socket(AF_INET, SOCK_DGRAM, 0);
      EXPECT_NE(fd,-1);

      /* fill in the server's address and data */
      struct sockaddr_in sockAddr;
      memset(&sockAddr, 0, sizeof(sockAddr));
      sockAddr.sin_family = AF_INET;
      sockAddr.sin_port = htons(atoi(port));
      inet_aton(addr , &sockAddr.sin_addr);


      UdpReceiverComponentImpl::UdpSerialBuffer buff;

      // serialize sequence number
      EXPECT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize(static_cast<U8>(seq)));
      // serialize port call
      EXPECT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize(static_cast<U8>(portNum)));

      UdpReceiverComponentImpl::UdpSerialBuffer portBuff;
      EXPECT_EQ(Fw::FW_SERIALIZE_OK,portBuff.serialize(val));

      EXPECT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize(portBuff));
      // send on UDP socket
      ssize_t sendStat = sendto(fd,
              buff.getBuffAddr(),
              buff.getBuffLength(),
              0,
              reinterpret_cast<struct sockaddr *>(&sockAddr),
              sizeof(sockAddr));
      EXPECT_NE(-1,sendStat);
      EXPECT_EQ(buff.getBuffLength(),sendStat);

      close(fd);
  }

  void Tester::textLogIn(const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }

} // end namespace Svc
