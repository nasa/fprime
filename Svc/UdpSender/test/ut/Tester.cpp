// ======================================================================
// \title  UdpSender.hpp
// \author tcanham
// \brief  cpp file for UdpSender test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      UdpSenderGTestBase("Tester", MAX_HISTORY_SIZE),
      component("UdpSender")
      ,m_recvFd(-1)
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {
    close(this->m_recvFd);
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
     openTest(const char* server, const char* port)
  {
    this->component.open(server,port);

    // expect successfully opened
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_US_PortOpened_SIZE(1);
  }

  void Tester ::
     sendTest(const char* server, const char* port)
  {
    this->component.open(server,port);

    // expect successfully opened
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_US_PortOpened_SIZE(1);

    // open receive port
    this->udpRecvStart(port);

    this->clearEvents();
    // send a buffer
    Svc::UdpSenderComponentImpl::UdpSerialBuffer buff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,buff.serialize(static_cast<U32>(10)));

    this->invoke_to_PortsIn(1,buff);
    this->component.doDispatch();

    // verify packet count
    ASSERT_EVENTS_SIZE(0);
    // verify byte count
    this->invoke_to_Sched(0,0);
    ASSERT_TLM_US_PacketsSent_SIZE(1);
    ASSERT_TLM_US_PacketsSent(0,1);

    // get data back
    U8 recvBytes[256];
    memset(recvBytes,0xFF,sizeof(recvBytes));
    NATIVE_INT_TYPE bytes = this->udpGet(recvBytes,sizeof(recvBytes));
    ASSERT_EQ(bytes,8);
    Fw::ExternalSerializeBuffer checkBuff(recvBytes,8);
    checkBuff.setBuffLen(8);
    // deserialize data packet
    U8 seq;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,checkBuff.deserialize(seq));
    ASSERT_EQ(0,seq);
    // deserialize port number
    U8 port;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,checkBuff.deserialize(port));
    ASSERT_EQ(1,port);
    // deserialize buffer
    Svc::UdpSenderComponentImpl::UdpSerialBuffer inBuff;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK,checkBuff.deserialize(inBuff));
    ASSERT_EQ(buff,inBuff);


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



  // ----------------------------------------------------------------------
  // Connect serial input ports
  // ----------------------------------------------------------------------
    // PortsIn
    for (NATIVE_INT_TYPE i = 0; i < 10; ++i) {
      this->connect_to_PortsIn(
          i,
          this->component.get_PortsIn_InputPort(i)
      );
    }


  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, 255, INSTANCE
    );
  }

  void Tester::textLogIn(const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }

  void Tester::udpRecvStart(const char* port) {

      sockaddr_in saddr;
      //create a UDP socket
      this->m_recvFd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      EXPECT_NE(-1,this->m_recvFd);

      // zero out the structure
      memset(&saddr, 0, sizeof(saddr));

      saddr.sin_family = AF_INET;
      saddr.sin_port = htons(atoi(port));
      saddr.sin_addr.s_addr = htonl(INADDR_ANY);

      //bind socket to port
      EXPECT_NE(-1,bind(this->m_recvFd , (struct sockaddr*)&saddr, sizeof(saddr) ));
  }

  NATIVE_INT_TYPE Tester::udpGet(U8* buff, NATIVE_UINT_TYPE size) {

      NATIVE_INT_TYPE psize = recvfrom(this->m_recvFd,buff,size,MSG_WAITALL,0,0);
      EXPECT_NE(psize,-1);
      return psize;

  }



} // end namespace Svc
