// ======================================================================
// \title  FprimeRouterTester.cpp
// \author thomas-bc
// \brief  cpp file for FprimeRouter component test harness implementation class
// ======================================================================

#include "FprimeRouterTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FprimeRouterTester ::FprimeRouterTester(bool disconnect_unknownData_port)
    : FprimeRouterGTestBase("FprimeRouterTester", FprimeRouterTester::MAX_HISTORY_SIZE), component("FprimeRouter") {
    this->initComponents();
    if (disconnect_unknownData_port) {
        this->connectPortsExceptUnknownData();  // hand-coded function connecting all ports except unknownData
    } else {
        this->connectPorts();  // autocoded function connecting all ports
    }
}

FprimeRouterTester ::~FprimeRouterTester() {}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

void FprimeRouterTester ::testRouteComInterface() {
    this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_COMMAND);
    ASSERT_from_commandOut_SIZE(1);      // one command packet emitted
    ASSERT_from_fileOut_SIZE(0);         // no file packet emitted
    ASSERT_from_unknownDataOut_SIZE(0);  // no unknown data emitted
    ASSERT_from_dataReturnOut_SIZE(1);   // data ownership should always be returned
}

void FprimeRouterTester ::testRouteFileInterface() {
    this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_FILE);
    ASSERT_from_commandOut_SIZE(0);      // no command packet emitted
    ASSERT_from_fileOut_SIZE(1);         // one file packet emitted
    ASSERT_from_unknownDataOut_SIZE(0);  // no unknown data emitted
    ASSERT_from_dataReturnOut_SIZE(0);   // data ownership is not returned yet (will come back on fileBufferReturnIn)
}

void FprimeRouterTester ::testRouteUnknownPacket() {
    this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_UNKNOWN);
    ASSERT_from_commandOut_SIZE(0);      // no command packet emitted
    ASSERT_from_fileOut_SIZE(0);         // no file packet emitted
    ASSERT_from_unknownDataOut_SIZE(1);  // one unknown data emitted
    ASSERT_from_dataReturnOut_SIZE(0);   // data ownership is not returned yet (will come back on fileBufferReturnIn)
}

void FprimeRouterTester ::testRouteUnknownPacketUnconnected() {
    this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_UNKNOWN);
    ASSERT_from_commandOut_SIZE(0);      // no command packet emitted
    ASSERT_from_fileOut_SIZE(0);         // no file packet emitted
    ASSERT_from_unknownDataOut_SIZE(0);  // zero unknown data emitted when port is unconnected
    ASSERT_from_dataReturnOut_SIZE(1);   // data ownership should always be returned
}

void FprimeRouterTester ::testBufferReturn() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    this->invoke_to_fileBufferReturnIn(0, buffer);
    ASSERT_from_dataReturnOut_SIZE(1);  // buffer should be returned via dataReturnOut
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
}

void FprimeRouterTester ::testCommandResponse() {
    const U32 opcode = 0;
    const U32 cmdSeq = 0;
    const Fw::CmdResponse cmdResp(Fw::CmdResponse::OK);
    this->invoke_to_cmdResponseIn(0, opcode, cmdSeq, cmdResp);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Test Helper
// ----------------------------------------------------------------------

void FprimeRouterTester::mockReceivePacketType(Fw::ComPacketType packetType) {
    const FwPacketDescriptorType descriptorType = packetType;
    U8 data[sizeof descriptorType];
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    context.set_apid(static_cast<ComCfg::Apid::T>(descriptorType));
    this->invoke_to_dataIn(0, buffer, context);
}

void FprimeRouterTester::connectPortsExceptUnknownData() {
    // Connect special output ports
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));
    this->component.set_logTextOut_OutputPort(0, this->get_from_logTextOut(0));
    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));
    // Connect typed input ports
    this->connect_to_cmdResponseIn(0, this->component.get_cmdResponseIn_InputPort(0));
    this->connect_to_dataIn(0, this->component.get_dataIn_InputPort(0));
    this->connect_to_fileBufferReturnIn(0, this->component.get_fileBufferReturnIn_InputPort(0));
    // Connect typed output ports
    this->component.set_commandOut_OutputPort(0, this->get_from_commandOut(0));
    this->component.set_dataReturnOut_OutputPort(0, this->get_from_dataReturnOut(0));
    this->component.set_fileOut_OutputPort(0, this->get_from_fileOut(0));
}

}  // namespace Svc
