// ======================================================================
// \title  UartFramer.hpp
// \author tcanham
// \brief  cpp file for UartFramer test harness implementation class
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

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      UartFramerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("UartFramer")
    ,m_currBuff(0)
    ,m_emptyAlloc(false)
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

  void Tester ::
    initialization()
  {

    this->component.allocate(NUM_INIT_BUFFERS,10);
    ASSERT_EQ(NUM_INIT_BUFFERS,this->m_currBuff);
    // two port calls; allocate and pass to UART driver
    ASSERT_FROM_PORT_HISTORY_SIZE(NUM_INIT_BUFFERS*2);
    // allocation calls
    ASSERT_from_DeframerAllocate_SIZE(NUM_INIT_BUFFERS);
    // UART driver calls
    ASSERT_from_readBufferSend_SIZE(NUM_INIT_BUFFERS);

  }

  void Tester ::
    packetSend()
  {

    // do initialization
    this->initialization();
    this->clearHistory();

    // send a packet
    Fw::Buffer sent(reinterpret_cast<U8*>(1),0);
    this->invoke_to_Framer(0,sent);
    // verify forwarded to driver
    // UART + deallocate
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    // Buffer deallocation
    ASSERT_from_FramerDeallocate_SIZE(1);
    // Send buffer to UART
    ASSERT_from_serialSend_SIZE(1);

  }

  void Tester ::
    packetReceive()
  {

    // do initialization
    this->initialization();
    this->clearHistory();

    // receive a packet
    Fw::Buffer received(reinterpret_cast<U8*>(1),0);
    Drv::SerialReadStatus stat = Drv::SerialReadStatus::SER_OK;
    this->invoke_to_serialRecv(0,received,stat);
    // verify forwarded to driver
    // send to deframer + allocate a new packet + send to UART
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    // packet sent to deframer
    ASSERT_from_Deframer_SIZE(1);
    // verify good status to deframer
    Drv::RecvStatus retStat = Drv::RecvStatus::RECV_OK;
    ASSERT_from_Deframer(0,received,retStat);
    // Buffer allocation for UART pool
    ASSERT_from_DeframerAllocate_SIZE(1);
    // Return buffer to UART for receive pool
    ASSERT_from_readBufferSend_SIZE(1);

  }

  void Tester ::
    packetReceiveError()
  {

    // do initialization
    this->initialization();
    this->clearHistory();

    // receive a packet
    Fw::Buffer received(reinterpret_cast<U8*>(1),0);
    Drv::SerialReadStatus stat = Drv::SerialReadStatus::SER_PARITY_ERR;
    this->invoke_to_serialRecv(0,received,stat);
    // verify forwarded to driver
    // send to deframer + allocate a new packet + send to UART
    ASSERT_FROM_PORT_HISTORY_SIZE(3);
    // packet sent to deframer
    ASSERT_from_Deframer_SIZE(1);
    // verify good status to deframer
    Drv::RecvStatus retStat = Drv::RecvStatus::RECV_ERROR;
    ASSERT_from_Deframer(0,received,retStat);
    // Buffer allocation for UART pool
    ASSERT_from_DeframerAllocate_SIZE(1);
    // Return buffer to UART for receive pool
    ASSERT_from_readBufferSend_SIZE(1);

  }

  void Tester ::
    packetAllocationError()
  {

    // do initialization
    this->initialization();
    this->clearHistory();

    // receive a packet
    Fw::Buffer received(reinterpret_cast<U8*>(1),0);
    Drv::SerialReadStatus stat = Drv::SerialReadStatus::SER_OK;
    // set flag to return empty buffer
    this->m_emptyAlloc = true;
    this->invoke_to_serialRecv(0,received,stat);
    // verify forwarded to driver
    // send to deframer + allocate a new packet + send to UART
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    // packet sent to deframer
    ASSERT_from_Deframer_SIZE(1);
    // verify good status to deframer
    Drv::RecvStatus retStat = Drv::RecvStatus::RECV_OK;
    ASSERT_from_Deframer(0,received,retStat);
    // Buffer allocation for UART pool
    ASSERT_from_DeframerAllocate_SIZE(1);
    // Shouldn't try to give a buffer to the UART if one wasn't allocated
    ASSERT_from_readBufferSend_SIZE(0);
    // Should have seen error event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BuffErr_SIZE(1);

  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_Deframer_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &recvBuffer,
        const Drv::RecvStatus &recvStatus
    )
  {
    this->pushFromPortEntry_Deframer(recvBuffer, recvStatus);
    this->m_currBuff--;
  }

  Fw::Buffer Tester ::
    from_DeframerAllocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_DeframerAllocate(size);
    if (not this->m_emptyAlloc) {
      this->m_buffPool[this->m_currBuff].setSize(size);
      this->m_buffPool[this->m_currBuff].setData(reinterpret_cast<U8*>(1));
      this->m_currBuff++;
      return this->m_buffPool[this->m_currBuff-1];
    } else {
      Fw::Buffer empty(reinterpret_cast<U8*>(1),0);
      return empty;
    }
  }

  void Tester ::
    from_FramerDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_FramerDeallocate(fwBuffer);
  }

  void Tester ::
    from_readBufferSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_readBufferSend(fwBuffer);
  }

  void Tester ::
    from_serialSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
    this->pushFromPortEntry_serialSend(serBuffer);
  }

  void Tester::textLogIn(const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }


  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // Framer
    this->connect_to_Framer(
        0,
        this->component.get_Framer_InputPort(0)
    );

    // serialRecv
    this->connect_to_serialRecv(
        0,
        this->component.get_serialRecv_InputPort(0)
    );

    // Deframer
    this->component.set_Deframer_OutputPort(
        0,
        this->get_from_Deframer(0)
    );

    // DeframerAllocate
    this->component.set_DeframerAllocate_OutputPort(
        0,
        this->get_from_DeframerAllocate(0)
    );

    // FramerDeallocate
    this->component.set_FramerDeallocate_OutputPort(
        0,
        this->get_from_FramerDeallocate(0)
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

    // Time
    this->component.set_Time_OutputPort(
        0,
        this->get_from_Time(0)
    );

    // readBufferSend
    this->component.set_readBufferSend_OutputPort(
        0,
        this->get_from_readBufferSend(0)
    );

    // serialSend
    this->component.set_serialSend_OutputPort(
        0,
        this->get_from_serialSend(0)
    );




  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Drv
