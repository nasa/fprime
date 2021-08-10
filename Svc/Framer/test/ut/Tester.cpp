// ======================================================================
// \title  Framer.hpp
// \author mstarch
// \brief  cpp file for Framer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000

namespace Svc {

Tester::MockFramer::MockFramer(Tester& parent) : m_parent(parent) {}

void Tester::MockFramer::frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) {
    Fw::Buffer buffer((U8*)data, size);
    m_parent.check_last_buffer(buffer);
    Fw::Buffer allocated = m_interface->allocate(size);
    m_interface->send(allocated);
}

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    :
      FramerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Framer"),
      m_mock(*this),
      m_framed(false),
      m_returned(false)

{
    this->initComponents();
    this->connectPorts();
    component.setup(this->m_mock);
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::test_incoming(U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        Fw::ComBuffer com;
        m_buffer.set(com.getBuffAddr(), com.getBuffLength());
        m_framed = false;
        invoke_to_comIn(0, com, 0);
        ASSERT_TRUE(m_framed);
    }
}

void Tester ::test_buffer(U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        Fw::Buffer buffer(new U8[3412], 3412);
        m_framed = false;
        m_returned = false;
        m_buffer = buffer;
        invoke_to_bufferIn(0, buffer);
        ASSERT_TRUE(m_framed);
        ASSERT_TRUE(m_returned);
    }
}

void Tester ::check_last_buffer(Fw::Buffer buffer) {
    ASSERT_EQ(buffer, m_buffer);
}

void Tester  ::check_not_freed() {
    ASSERT_FALSE(m_returned);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
    m_returned = true;
    delete[] fwBuffer.getData();
}

Fw::Buffer Tester ::from_framedAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_framedAllocate(size);
    Fw::Buffer buffer(new U8[size], size);
    m_buffer = buffer;
    return buffer;
}

Drv::SendStatus Tester ::from_framedOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    this->pushFromPortEntry_framedOut(sendBuffer);
    this->check_last_buffer(sendBuffer);
    delete[] sendBuffer.getData();
    m_framed = true;
    return Drv::SendStatus::SEND_OK;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
    // comIn
    this->connect_to_comIn(0, this->component.get_comIn_InputPort(0));

    // bufferIn
    this->connect_to_bufferIn(0, this->component.get_bufferIn_InputPort(0));

    // bufferDeallocate
    this->component.set_bufferDeallocate_OutputPort(0, this->get_from_bufferDeallocate(0));

    // framedAllocate
    this->component.set_framedAllocate_OutputPort(0, this->get_from_framedAllocate(0));

    // framedOut
    this->component.set_framedOut_OutputPort(0, this->get_from_framedOut(0));

    // timeGet
    this->component.set_timeGet_OutputPort(0, this->get_from_timeGet(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
