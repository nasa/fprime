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
Tester::MockDeframer::MockDeframer(Tester& parent) : m_parent(parent), m_status(DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {}

Tester::MockDeframer::DeframingStatus Tester::MockDeframer::deframe(Types::CircularBuffer& ring_buffer, U32& needed) {
    needed = ring_buffer.get_remaining_size();
    if (m_status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
        needed = ring_buffer.get_remaining_size() + 1; // Obey the rules
    }
    return m_status;
}

void Tester::MockDeframer::test_interface(Fw::ComPacket::ComPacketType com_packet_type) {
    U8 chars[4];
    m_interface->allocate(3042);
    Fw::Buffer buffer(chars, sizeof(chars));
    buffer.getSerializeRepr().serialize(com_packet_type);
    m_interface->route(buffer);
}


Tester ::Tester(void)
    : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Deframer"),
      m_mock(*this) {
    this->initComponents();
    this->connectPorts();
    component.setup(this->m_mock);
}

Tester ::~Tester(void) {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void Tester ::test_incoming_frame(Tester::MockDeframer::DeframingStatus status) {
    U32 buffer_size = 512;
    U8 data[buffer_size];
    ::memset(data, 0, buffer_size);
    Fw::Buffer recvBuffer(data, buffer_size);
    m_mock.m_status = status;
    Drv::RecvStatus recvStatus = Drv::RecvStatus::RECV_OK;
    invoke_to_framedIn(0, recvBuffer, recvStatus);
    // Check remaining size
    if (status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
        ASSERT_EQ(component.m_in_ring.get_remaining_size(), buffer_size);
    } else if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
        ASSERT_EQ(component.m_in_ring.get_remaining_size(), 0);
    } else {
        ASSERT_EQ(component.m_in_ring.get_remaining_size(), 0);
    }
    ASSERT_from_framedDeallocate(0, recvBuffer);
}

void Tester ::test_com_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_COMMAND);
    ASSERT_from_comOut_SIZE(1);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void Tester ::test_buffer_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_FILE);
    ASSERT_from_comOut_SIZE(0);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(0);
}

void Tester ::test_unknown_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_UNKNOWN);
    ASSERT_from_comOut_SIZE(0);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_comOut_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comOut(data, context);
}

void Tester ::from_bufferOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_bufferOut(fwBuffer);
}

Fw::Buffer Tester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_bufferAllocate(size);
    Fw::Buffer buffer(nullptr, size);
    return buffer;
}

void Tester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void Tester ::from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_framedDeallocate(fwBuffer);
}

Drv::PollStatus Tester ::from_framedPoll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& pollBuffer) {
    this->pushFromPortEntry_framedPoll(pollBuffer);
    // TODO: Return a value
    return Drv::POLL_OK;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts(void) {
    // framedIn
    this->connect_to_framedIn(0, this->component.get_framedIn_InputPort(0));

    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));

    // comOut
    this->component.set_comOut_OutputPort(0, this->get_from_comOut(0));

    // bufferOut
    this->component.set_bufferOut_OutputPort(0, this->get_from_bufferOut(0));

    // bufferAllocate
    this->component.set_bufferAllocate_OutputPort(0, this->get_from_bufferAllocate(0));

    // bufferDeallocate
    this->component.set_bufferDeallocate_OutputPort(0, this->get_from_bufferDeallocate(0));

    // framedDeallocate
    this->component.set_framedDeallocate_OutputPort(0, this->get_from_framedDeallocate(0));

    // framedPoll
    this->component.set_framedPoll_OutputPort(0, this->get_from_framedPoll(0));
}

void Tester ::initComponents(void) {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
