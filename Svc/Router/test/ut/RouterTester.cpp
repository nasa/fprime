// ======================================================================
// \title  RouterTester.cpp
// \author chammard
// \brief  cpp file for Router component test harness implementation class
// ======================================================================

#include "RouterTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

RouterTester ::RouterTester() : RouterGTestBase("RouterTester", RouterTester::MAX_HISTORY_SIZE), component("Router") {
    this->initComponents();
    this->connectPorts();
}

RouterTester ::~RouterTester() {}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

void RouterTester ::testRouteComInterface() {
    this->mockReceivePacketType(Fw::ComPacket::FW_PACKET_COMMAND);
    ASSERT_from_commandOut_SIZE(1);
    ASSERT_from_fileOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void RouterTester ::testRouteFileInterface() {
    this->mockReceivePacketType(Fw::ComPacket::FW_PACKET_FILE);
    ASSERT_from_commandOut_SIZE(0);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_bufferDeallocate_SIZE(0);
}

void RouterTester ::testRouteUnknownPacket() {
    this->mockReceivePacketType(Fw::ComPacket::FW_PACKET_UNKNOWN);
    ASSERT_from_commandOut_SIZE(0);
    ASSERT_from_fileOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(1);
}

void RouterTester ::testCommandResponse() {
    const U32 opcode = 0;
    const U32 cmdSeq = 0;
    const Fw::CmdResponse cmdResp(Fw::CmdResponse::OK);
    this->invoke_to_cmdResponseIn(0, opcode, cmdSeq, cmdResp);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Test Helper
// ----------------------------------------------------------------------

void RouterTester::mockReceivePacketType(Fw::ComPacket::ComPacketType packetType) {
    const FwPacketDescriptorType descriptorType = packetType;
    U8 data[sizeof descriptorType];
    Fw::Buffer buffer(data, sizeof(data));
    buffer.getSerializeRepr().serialize(descriptorType);
    this->invoke_to_bufferIn(0, buffer);
}

}  // namespace Svc
