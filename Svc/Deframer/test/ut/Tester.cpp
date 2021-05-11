// ====================================================================== 
// \title  Deframer.hpp
// \author janamian
// \brief  cpp file for Deframer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------
  Tester::MockDeframer::MockDeframer(Tester& parent) : m_parent(parent) {}

  Tester::MockDeframer::DeframingStatus Tester::MockDeframer::deframe(
                  Types::CircularBuffer& ring_buffer, U32& needed) {
    if ((ring_buffer.get_capacity()) <= ring_buffer.get_remaining_size()){
        m_parent.m_status = DEFRAMING_INVALID_SIZE;
        return DEFRAMING_INVALID_SIZE;

    } else if (needed < ring_buffer.get_remaining_size()) {
        needed++;
        m_parent.m_status = DEFRAMING_MORE_NEEDED;
        return DEFRAMING_MORE_NEEDED;

    } else if (needed == ring_buffer.get_remaining_size()) {
        Fw::Buffer buf = m_interface->allocate(needed);
        ring_buffer.peek(buf.getData(), needed, 0);
        m_interface->route(buf);
        m_parent.m_status = DEFRAMING_STATUS_SUCCESS;
        m_parent.m_remaining_size += ring_buffer.get_remaining_size();
        return DEFRAMING_STATUS_SUCCESS;

    } else {
        return DEFRAMING_INVALID_SIZE;
    }
  }

  Tester ::
    Tester(void) : 
      DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Deframer"),
      m_mock(*this),
      m_status(DeframingProtocol::DeframingStatus::DEFRAMING_MAX_STATUS),
      m_remaining_size(0)
  {
    this->initComponents();
    this->connectPorts();
    component.setup(this->m_mock);
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------
    void Tester ::test_incoming_frame(U32 buffer_size, 
                                      U32 expected_size) {
        U8 data[buffer_size];
        ::memset(data, 0, buffer_size);
        Fw::Buffer recvBuffer(data, buffer_size);
        Drv::RecvStatus recvStatus = Drv::RecvStatus::RECV_OK;
        invoke_to_framedIn(0, recvBuffer, recvStatus);
        ASSERT_EQ(m_remaining_size, buffer_size);
    }

    void Tester ::test_route(Fw::ComPacket::ComPacketType packet_type) {
        const U32 buffer_size = sizeof(U32) * 3;
        const U32 token_size =  sizeof(U32) * 2;
        U8 buffer[buffer_size] = {};
        buffer[token_size] = (static_cast<U32>(packet_type) >> 24) & 0xFF;
        Fw::Buffer recvBuffer(buffer, buffer_size);
        Drv::RecvStatus recvStatus = Drv::RecvStatus::RECV_OK;
        m_has_port_out = false;
        invoke_to_framedIn(0, recvBuffer, recvStatus);
        ASSERT_TRUE(m_has_port_out);
    }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_comOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    this->pushFromPortEntry_comOut(data, context);
  }

  void Tester ::
    from_bufferOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->m_has_port_out = true;
    this->pushFromPortEntry_bufferOut(fwBuffer);
    // Have to clean up memory as in a normal mode, file downlink doesn't require deallocation
    delete[] (fwBuffer.getData() - sizeof(I32));
  }

  Fw::Buffer Tester ::
    from_bufferAllocate_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_bufferAllocate(size);
    Fw::Buffer buffer(new U8[size], size);
    m_buffer = buffer;
    return buffer;
  }

  void Tester ::
    from_bufferDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->m_has_port_out = true;
    delete[] fwBuffer.getData();
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
  }

  void Tester ::
    from_framedDeallocate_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_framedDeallocate(fwBuffer);
  }

  Drv::PollStatus Tester ::
    from_framedPoll_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &pollBuffer
    )
  {
    this->pushFromPortEntry_framedPoll(pollBuffer);
    // TODO: Return a value
    return Drv::POLL_OK;
  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // framedIn
    this->connect_to_framedIn(
        0,
        this->component.get_framedIn_InputPort(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );

    // comOut
    this->component.set_comOut_OutputPort(
        0, 
        this->get_from_comOut(0)
    );

    // bufferOut
    this->component.set_bufferOut_OutputPort(
        0, 
        this->get_from_bufferOut(0)
    );

    // bufferAllocate
    this->component.set_bufferAllocate_OutputPort(
        0, 
        this->get_from_bufferAllocate(0)
    );

    // bufferDeallocate
    this->component.set_bufferDeallocate_OutputPort(
        0, 
        this->get_from_bufferDeallocate(0)
    );

    // framedDeallocate
    this->component.set_framedDeallocate_OutputPort(
        0, 
        this->get_from_framedDeallocate(0)
    );

    // framedPoll
    this->component.set_framedPoll_OutputPort(
        0, 
        this->get_from_framedPoll(0)
    );




  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
