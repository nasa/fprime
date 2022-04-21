// ======================================================================
// \title  Tester.cpp
// \author mstarch, bocchino
// \brief  cpp file for Deframer test harness implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <limits>

#include "Tester.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Hash/HashBuffer.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10000

namespace Svc {
// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester(bool polling)
    : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Deframer"),
      m_polling(polling)
  {
    this->initComponents();
    this->connectPorts();
    component.setup(protocol);
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::sizeTooLarge() {
    U8 data[FpFrameHeader::SIZE];
    Fw::Buffer buffer(data, sizeof data);
    Fw::SerializeBufferBase& serialRepr = buffer.getSerializeRepr();
    Fw::SerializeStatus status = serialRepr.serialize(FpFrameHeader::START_WORD);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    FpFrameHeader::TokenType size = std::numeric_limits<U32>::max();
    status = serialRepr.serialize(size);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    this->component.processBuffer(buffer);
    // Assert no output
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_comOut_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    // Check for available data
    ASSERT_GT(m_receiving.size(), 0) << "Check-data receiving queue empty" << std::endl;
    // Grab the front item
    const auto& frame= m_receiving.front();
    m_receiving.pop_front();

    ASSERT_EQ(frame.packetType, Fw::ComPacket::FW_PACKET_COMMAND);
    for (U32 i = 0; i < data.getBuffLength(); i++) {
        EXPECT_EQ(data.getBuffAddr()[i], frame.data[i + FpFrameHeader::SIZE]);
    }
    this->pushFromPortEntry_comOut(data, context);
}

void Tester ::from_bufferOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // Check for available data
    ASSERT_GT(m_receiving.size(), 0) << "Check-data receiving queue empty" << std::endl;
    // Grab the front item
    UplinkFrame check = m_receiving.front();
    m_receiving.pop_front();

    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        // File uplink strips type before outputting to FileUplink
        ASSERT_EQ(fwBuffer.getData()[i], check.data[i + FpFrameHeader::SIZE + sizeof(FwPacketDescriptorType)]);
    }
    // Have to clean up memory as in a normal mode, file uplink doesn't require deallocation
    delete[](fwBuffer.getData() - sizeof(FwPacketDescriptorType));
    this->pushFromPortEntry_bufferOut(fwBuffer);
}

Fw::Buffer Tester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_bufferAllocate(size);
    U8 *const data = new U8[size];
    memset(data, 0, size);
    Fw::Buffer buffer(data, size);
    return buffer;
}

void Tester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    delete[] fwBuffer.getData();
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void Tester ::from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    delete[] fwBuffer.getData(); // Allocated in rules
    this->pushFromPortEntry_framedDeallocate(fwBuffer);
}

Drv::PollStatus Tester ::from_framedPoll_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& pollBuffer) {
    this->pushFromPortEntry_framedPoll(pollBuffer);
    U8* incoming = m_incoming_buffer.getData();
    U8* outgoing = pollBuffer.getData();
    // TODO: Check size bound
    // TODO: Incoming should not overrun outgoing
    // TODO: Replace with memcpy
    for (U32 i = 0; i < m_incoming_buffer.getSize(); i++) {
        outgoing[i] = incoming[i];
    }
    pollBuffer.setSize(m_incoming_buffer.getSize());
    delete[] incoming;
    return Drv::PollStatus::POLL_OK;
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {

    // bufferAllocate
    this->component.set_bufferAllocate_OutputPort(0, this->get_from_bufferAllocate(0));

    // bufferDeallocate
    this->component.set_bufferDeallocate_OutputPort(0, this->get_from_bufferDeallocate(0));

    // bufferOut
    this->component.set_bufferOut_OutputPort(0, this->get_from_bufferOut(0));

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

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
