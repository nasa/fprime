// ======================================================================
// \title  Framer.hpp
// \author mstarch, bocchino
// \brief  cpp file for Framer test harness implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000

namespace Svc {

Tester::MockFramer::MockFramer(Tester& parent) : m_parent(parent), m_do_not_send(false) {}

void Tester::MockFramer::frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) {
    // When testing without the send case, disable all mock functions
    if (!m_do_not_send) {
        Fw::Buffer buffer(const_cast<U8*>(data), size);
        m_parent.check_last_buffer(buffer);
        Fw::Buffer allocated = m_interface->allocate(size);
        m_interface->send(allocated);
    }
}

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : FramerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Framer"),
      m_mock(*this),
      m_framed(false),
      m_sent(false),
      m_returned(false),
      m_sendStatus(Drv::SendStatus::SEND_OK)

{
    this->initComponents();
    this->connectPorts();
    component.setup(this->m_mock);
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::test_com(U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        Fw::ComBuffer com;
        m_buffer.set(com.getBuffAddr(), com.getBuffLength());
        m_framed = false;
        m_sent = false;
        m_returned = false;
        invoke_to_comIn(0, com, 0);
        ASSERT_TRUE(m_framed);
        if (m_sendStatus == Drv::SendStatus::SEND_OK) {
            ASSERT_TRUE(m_sent);
        } else {
            ASSERT_FALSE(m_sent);
        }
        ASSERT_FALSE(m_returned);
    }
}

void Tester ::test_buffer(U32 iterations) {
    for (U32 i = 0; i < iterations; i++) {
        Fw::Buffer buffer(new U8[3412], 3412);
        m_framed = false;
        m_sent = false;
        m_returned = false;
        m_buffer = buffer;
        invoke_to_bufferIn(0, buffer);
        ASSERT_TRUE(m_framed);
        if (m_sendStatus == Drv::SendStatus::SEND_OK) {
            ASSERT_TRUE(m_sent);
        } else {
            ASSERT_FALSE(m_sent);
        }
        ASSERT_TRUE(m_returned);
    }
}

void Tester ::test_status_pass_through() {
    // Check not always success
    Fw::Success status = Fw::Success::FAILURE;
    invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut(0, status);

    // Check a success
    status = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut(1, status);
}

void Tester ::test_no_send_status() {
    Fw::Success status = Fw::Success::SUCCESS;
    m_mock.m_do_not_send = true;
    // Send com buffer and check no send and a status
    Fw::ComBuffer com;
    invoke_to_comIn(0, com, 0);
    ASSERT_from_framedOut_SIZE(0);
    ASSERT_from_comStatusOut(0, status);

    Fw::Buffer buffer(new U8[3412], 3412);
    invoke_to_bufferIn(0, buffer);
    ASSERT_from_framedOut_SIZE(0);
    ASSERT_from_comStatusOut(0, status);
    clearFromPortHistory();

    // Make sure it still does pass-through
    test_status_pass_through();
}

void Tester ::check_last_buffer(Fw::Buffer buffer) {
    ASSERT_EQ(buffer, m_buffer);
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
    if (m_sendStatus == Drv::SendStatus::SEND_OK) {
        m_sent = true;
    }
    return m_sendStatus;
}

void Tester ::from_comStatusOut_handler(const NATIVE_INT_TYPE portNum, Fw::Success& condition) {
    this->pushFromPortEntry_comStatusOut(condition);
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

    // comStatusIn
    this->connect_to_comStatusIn(0, this->component.get_comStatusIn_InputPort(0));

    // comStatusOut
    this->component.set_comStatusOut_OutputPort(0, this->get_from_comStatusOut(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

void Tester ::setSendStatus(Drv::SendStatus sendStatus) {
    m_sendStatus = sendStatus;
}

}  // end namespace Svc
