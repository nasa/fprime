// ======================================================================
// \title  Deframer.hpp
// \author janamian, bocchino
// \brief  cpp file for Deframer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cstring>

#include "DeframerTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------
DeframerTester::MockDeframer::MockDeframer(DeframerTester& parent) : m_status(DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {}

DeframerTester::MockDeframer::DeframingStatus DeframerTester::MockDeframer::deframe(Types::CircularBuffer& ring_buffer, U32& needed) {
    needed = ring_buffer.get_allocated_size();
    if (m_status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
        needed = ring_buffer.get_allocated_size() + 1; // Obey the rules
    }
    return m_status;
}

void DeframerTester::MockDeframer::test_interface(Fw::ComPacket::ComPacketType com_packet_type) {
    const FwPacketDescriptorType descriptorType = com_packet_type;
    U8 chars[sizeof descriptorType];
    m_interface->allocate(3042);
    Fw::Buffer buffer(chars, sizeof(chars));
    buffer.getSerializeRepr().serialize(descriptorType);
    m_interface->route(buffer);
}


DeframerTester ::DeframerTester(ConnectStatus::t bufferOutStatus)
    : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Deframer"),
      m_mock(*this),
      bufferOutStatus(bufferOutStatus) {
    this->initComponents();
    this->connectPorts();
    component.setup(this->m_mock);
}

DeframerTester ::~DeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void DeframerTester ::test_incoming_frame(DeframerTester::MockDeframer::DeframingStatus status) {
    U32 buffer_size = 512;
    U8 data[buffer_size];
    ::memset(data, 0, buffer_size);
    Fw::Buffer recvBuffer(data, buffer_size);
    m_mock.m_status = status;
    Drv::RecvStatus recvStatus = Drv::RecvStatus::RECV_OK;
    invoke_to_framedIn(0, recvBuffer, recvStatus);
    // Check remaining size
    if (status == DeframingProtocol::DEFRAMING_MORE_NEEDED) {
        ASSERT_EQ(component.m_inRing.get_allocated_size(), buffer_size);
    } else if (status == DeframingProtocol::DEFRAMING_STATUS_SUCCESS) {
        ASSERT_EQ(component.m_inRing.get_allocated_size(), 0);
    } else {
        ASSERT_EQ(component.m_inRing.get_allocated_size(), 0);
    }
    ASSERT_from_framedDeallocate(0, recvBuffer);
}

void DeframerTester ::test_com_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_COMMAND);
    ASSERT_from_comOut_SIZE(1);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void DeframerTester ::test_file_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_FILE);
    ASSERT_from_comOut_SIZE(0);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(0);
}

void DeframerTester ::test_unknown_interface() {
    m_mock.test_interface(Fw::ComPacket::FW_PACKET_UNKNOWN);
    ASSERT_from_comOut_SIZE(0);
    ASSERT_from_bufferAllocate(0, 3042);
    ASSERT_from_bufferOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void DeframerTester ::testCommandResponse() {
    const U32 portNum = 0;
    const U32 opcode = 0;
    const U32 cmdSeq = 0;
    const Fw::CmdResponse cmdResp(Fw::CmdResponse::OK);
    this->invoke_to_cmdResponseIn(portNum, opcode, cmdSeq, cmdResp);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

void DeframerTester ::testCommandPacketTooLarge() {
    // Allocate a large packet buffer
    enum {
        BUFFER_SIZE = 2 * FW_COM_BUFFER_MAX_SIZE
    };
    U8 bytes[BUFFER_SIZE];
    ::memset(bytes, 0, sizeof bytes);
    Fw::Buffer buffer(bytes, sizeof bytes);
    // Serialize the packet type
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    const FwPacketDescriptorType descriptorType =
        Fw::ComPacket::FW_PACKET_COMMAND;
    const Fw::SerializeStatus status =
        serialRepr.serialize(descriptorType);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // Call the route method
    this->component.route(buffer);
    // Assert buffer deallocated, no packet output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void DeframerTester ::testPacketBufferTooSmall() {
    // Allocate a small packet buffer
    U8 byte = 0;
    Fw::Buffer buffer(&byte, sizeof byte);
    // Call the route method
    this->component.route(buffer);
    // Assert buffer deallocated, no packet output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void DeframerTester ::testBufferOutUnconnected() {
    ASSERT_EQ(this->bufferOutStatus, ConnectStatus::UNCONNECTED);
    enum {
        BUFFER_SIZE = 256
    };
    U8 bytes[BUFFER_SIZE];
    ::memset(bytes, 0, sizeof bytes);
    Fw::Buffer buffer(bytes, sizeof bytes);
    // Serialize the packet type
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    const FwPacketDescriptorType descriptorType =
        Fw::ComPacket::FW_PACKET_FILE;
    const Fw::SerializeStatus status =
        serialRepr.serialize(descriptorType);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // Call the route method
    this->component.route(buffer);
    // Assert buffer deallocated, no packet output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void DeframerTester ::from_comOut_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comOut(data, context);
}

void DeframerTester ::from_bufferOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_bufferOut(fwBuffer);
}

Fw::Buffer DeframerTester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_bufferAllocate(size);
    Fw::Buffer buffer(nullptr, size);
    return buffer;
}

void DeframerTester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void DeframerTester ::from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    this->pushFromPortEntry_framedDeallocate(fwBuffer);
}

Drv::PollStatus DeframerTester ::from_framedPoll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& pollBuffer) {
    this->pushFromPortEntry_framedPoll(pollBuffer);
    return Drv::PollStatus::POLL_OK;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void DeframerTester ::connectPorts() {
    // bufferAllocate
    this->component.set_bufferAllocate_OutputPort(0, this->get_from_bufferAllocate(0));

    // bufferDeallocate
    this->component.set_bufferDeallocate_OutputPort(0, this->get_from_bufferDeallocate(0));

    // bufferOut
    if (this->bufferOutStatus == ConnectStatus::CONNECTED) {
        this->component.set_bufferOut_OutputPort(0, this->get_from_bufferOut(0));
    }

    // cmdResponseIn
    this->connect_to_cmdResponseIn(0, this->component.get_cmdResponseIn_InputPort(0));

    // comOut
    this->component.set_comOut_OutputPort(0, this->get_from_comOut(0));

    // framedDeallocate
    this->component.set_framedDeallocate_OutputPort(0, this->get_from_framedDeallocate(0));

    // framedIn
    this->connect_to_framedIn(0, this->component.get_framedIn_InputPort(0));

    // framedPoll
    this->component.set_framedPoll_OutputPort(0, this->get_from_framedPoll(0));

    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));
}

void DeframerTester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
