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

Tester ::Tester(void)
    : DeframerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Deframer"),
      m_buffer(NULL),
      m_uplink_type(1),
      m_uplink_used(30),
      m_uplink_size(sizeof(FP_FRAME_TOKEN_TYPE) * 2 + sizeof(U32) + m_uplink_used),
      m_uplink_point(0),
      m_garbage(false),
      m_uplink_com_type(Fw::ComPacket::FW_PACKET_COMMAND) {
    this->initComponents();
    this->connectPorts();
    component.setup(protocol);
    update_header_info(0, 0);
}

Tester ::~Tester(void) {}

// ----------------------------------------------------------------------
// Tests State Adjustments
// ----------------------------------------------------------------------

void Tester ::update_header_info(U32 garbage_index, U8 garbage_byte) {
    // Write token types
    for (U32 i = 0; i < sizeof(FP_FRAME_TOKEN_TYPE); i++) {
        m_uplink_data[i] = (FprimeFraming::START_WORD >> ((sizeof(FP_FRAME_TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
        m_uplink_data[i + sizeof(FP_FRAME_TOKEN_TYPE)] =
            (m_uplink_used >> ((sizeof(FP_FRAME_TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
    }
    // Packet type
    m_uplink_data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 0] = (static_cast<U32>(m_uplink_com_type) >> 24) & 0xFF;
    m_uplink_data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 1] = (static_cast<U32>(m_uplink_com_type) >> 16) & 0xFF;
    m_uplink_data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 2] = (static_cast<U32>(m_uplink_com_type) >> 8) & 0xFF;
    m_uplink_data[2 * sizeof(FP_FRAME_TOKEN_TYPE) + 3] = (static_cast<U32>(m_uplink_com_type) >> 0) & 0xFF;

    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    hash.update(m_uplink_data,  2 * sizeof(FP_FRAME_TOKEN_TYPE) + m_uplink_used);
    hash.final(hashBuffer);

    for (U32 i = 0; i < sizeof(U32); i++) {
        m_uplink_data[i + 2 * sizeof(FP_FRAME_TOKEN_TYPE) + m_uplink_used] = hashBuffer.getBuffAddr()[i] & 0xFF;
    }
    if (m_garbage) {
        m_uplink_data[garbage_index] += garbage_byte;
    }
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_comOut_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comOut(data, context);
    for (U32 i = 0; i < data.getBuffLength(); i++) {
        ASSERT_EQ(data.getBuffAddr()[i], m_uplink_data[i + FP_FRAME_HEADER_SIZE]);
    }
}

void Tester ::from_bufferOut_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // this->m_has_port_out = true;
    this->pushFromPortEntry_bufferOut(fwBuffer);
    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        // File uplink strips type before outputting to FileUplink
        ASSERT_EQ(fwBuffer.getData()[i], m_uplink_data[i + FP_FRAME_HEADER_SIZE + sizeof(FP_FRAME_TOKEN_TYPE)]);
    }
    // Have to clean up memory as in a normal mode, file downlink doesn't require deallocation
    delete[](fwBuffer.getData() - sizeof(I32));
}

Fw::Buffer Tester ::from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, U32 size) {
    this->pushFromPortEntry_bufferAllocate(size);
    Fw::Buffer buffer(new U8[size], size);
    m_buffer = buffer.getData();
    return buffer;
}

void Tester ::from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // this->m_has_port_out = true;
    delete[] fwBuffer.getData();
    this->pushFromPortEntry_bufferDeallocate(fwBuffer);
}

void Tester ::from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
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
