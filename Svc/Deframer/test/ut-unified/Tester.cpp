// ======================================================================
// \title  Tester.cpp
// \author mstarch
// \brief  cpp file for Deframer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

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
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_comOut_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    // Seek to any packet of uplink type
    while ((m_receiving.front().type != Fw::ComPacket::FW_PACKET_COMMAND) &&
           (m_receiving.front().type != Fw::ComPacket::FW_PACKET_FILE)) {
        m_receiving.pop_front();
    }
    // Grab the front item
    UplinkData check = m_receiving.front();
    m_receiving.pop_front();

    // Check for file
    EXPECT_NE(check.type, Fw::ComPacket::FW_PACKET_FILE);
    if (check.type == Fw::ComPacket::FW_PACKET_FILE) {
        return;
    }
    for (U32 i = 0; i < data.getBuffLength(); i++) {
        EXPECT_EQ(data.getBuffAddr()[i], check.data[i + FP_FRAME_HEADER_SIZE]);
    }
    this->pushFromPortEntry_comOut(data, context);
}

void Tester ::from_bufferOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // Seek to any packet of uplink type
    while ((m_receiving.front().type != Fw::ComPacket::FW_PACKET_COMMAND) &&
           (m_receiving.front().type != Fw::ComPacket::FW_PACKET_FILE)) {
        m_receiving.pop_front();
    }
    // Grab the front item
    UplinkData check = m_receiving.front();
    m_receiving.pop_front();

    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        // File uplink strips type before outputting to FileUplink
        ASSERT_EQ(fwBuffer.getData()[i], check.data[i + FP_FRAME_HEADER_SIZE + sizeof(I32)]);
    }
    // Have to clean up memory as in a normal mode, file downlink doesn't require deallocation
    delete[](fwBuffer.getData() - sizeof(I32));
    this->pushFromPortEntry_bufferOut(fwBuffer);
}

Fw::Buffer Tester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_bufferAllocate(size);
    Fw::Buffer buffer(new U8[size], size);
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

void Tester ::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

}  // end namespace Svc
