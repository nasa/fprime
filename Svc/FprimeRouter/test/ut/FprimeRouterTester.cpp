// ======================================================================
// \title  FprimeRouterTester.cpp
// \author thomas-bc
// \brief  cpp file for FprimeRouter component test harness implementation class
// ======================================================================

#include "FprimeRouterTester.hpp"

namespace Svc {

// Out-of-line definition required because TEST_VC_ID is ODR-used (bound by
// reference in ASSERT_EQ comparisons below)
const U8 FprimeRouterTester::TEST_VC_ID;

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
    // A buffer that was never handed off (no saved context) still returns, but
    // its context is empty and a BufferContextNotFound event is emitted.
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    this->invoke_to_fileBufferReturnIn(0, buffer);
    ASSERT_from_dataReturnOut_SIZE(1);  // buffer should be returned via dataReturnOut
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EVENTS_BufferContextNotFound_SIZE(1);
    ComCfg::FrameContext defaultCtx;
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context.get_vcId(), defaultCtx.get_vcId());
}

void FprimeRouterTester ::testFileContextRoundTrip() {
    // Send a file packet; the router hands the buffer off on fileOut and remembers
    // the context. The same buffer returns on fileBufferReturnIn and the original
    // context (vcId) must be restored on dataReturnOut.
    Fw::Buffer sentBuffer = this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_FILE);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);  // not returned yet

    // Return the same buffer the router emitted on fileOut
    Fw::Buffer returned = this->fromPortHistory_fileOut->at(0).fwBuffer;
    this->invoke_to_fileBufferReturnIn(0, returned);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context.get_vcId(), FprimeRouterTester::TEST_VC_ID);
    ASSERT_EVENTS_SIZE(0);  // no degrade events
}

void FprimeRouterTester ::testMultiBufferContextRoundTrip() {
    // Two file buffers outstanding at once, each with a distinct vcId, returned
    // out of order. Each must have its own context restored.
    const U8 vcIdA = 5;
    const U8 vcIdB = 9;

    U8 dataA[sizeof(FwPacketDescriptorType)];
    U8 dataB[sizeof(FwPacketDescriptorType)];
    Fw::Buffer bufferA(dataA, sizeof(dataA));
    Fw::Buffer bufferB(dataB, sizeof(dataB));

    ComCfg::FrameContext ctxA;
    ctxA.set_apid(static_cast<ComCfg::Apid::T>(Fw::ComPacketType::FW_PACKET_FILE));
    ctxA.set_vcId(vcIdA);
    ComCfg::FrameContext ctxB;
    ctxB.set_apid(static_cast<ComCfg::Apid::T>(Fw::ComPacketType::FW_PACKET_FILE));
    ctxB.set_vcId(vcIdB);

    this->invoke_to_dataIn(0, bufferA, ctxA);
    this->invoke_to_dataIn(0, bufferB, ctxB);
    ASSERT_from_fileOut_SIZE(2);

    // Return B first, then A (out of order)
    this->invoke_to_fileBufferReturnIn(0, bufferB);
    this->invoke_to_fileBufferReturnIn(0, bufferA);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context.get_vcId(), vcIdB);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(1).context.get_vcId(), vcIdA);
    ASSERT_EVENTS_SIZE(0);
}

void FprimeRouterTester ::testContextTableFull(Fw::ComPacketType packetType) {
    // Fill the table with the maximum number of outstanding buffers, then send one
    // more. The overflow send must emit the table-full event for the port the packet
    // routes to (fileOut for FILE, unknownDataOut for UNKNOWN), and the overflow
    // buffer's context degrades to empty (default vcId) on return.
    const FwSizeType tableSize = ComCfg::RouterBufferContextTableSize;

    // Use a heap array of buffers so their data pointers stay distinct and alive
    U8* blocks = new U8[(tableSize + 1) * sizeof(FwPacketDescriptorType)];
    Fw::Buffer* buffers = new Fw::Buffer[tableSize + 1];
    for (FwSizeType i = 0; i < tableSize + 1; i++) {
        buffers[i] = Fw::Buffer(blocks + (i * sizeof(FwPacketDescriptorType)), sizeof(FwPacketDescriptorType));
        ComCfg::FrameContext ctx;
        ctx.set_apid(static_cast<ComCfg::Apid::T>(packetType));
        ctx.set_vcId(FprimeRouterTester::TEST_VC_ID);
        this->invoke_to_dataIn(0, buffers[i], ctx);
    }
    // Exactly one overflow event for the (tableSize+1)th send, on the routed port
    if (packetType == Fw::ComPacketType::FW_PACKET_FILE) {
        ASSERT_EVENTS_FileOutContextTableFull_SIZE(1);
    } else {
        ASSERT_EVENTS_UnknownDataOutContextTableFull_SIZE(1);
    }

    // The overflow buffer (last) returns with an empty/default context
    this->invoke_to_fileBufferReturnIn(0, buffers[tableSize]);
    const U8 lastReturnIdx = static_cast<U8>(this->fromPortHistory_dataReturnOut->size() - 1);
    ComCfg::FrameContext defaultCtx;
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(lastReturnIdx).context.get_vcId(), defaultCtx.get_vcId());
    ASSERT_EVENTS_BufferContextNotFound_SIZE(1);  // overflow buffer was never in the table

    delete[] buffers;
    delete[] blocks;
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

Fw::Buffer FprimeRouterTester::mockReceivePacketType(Fw::ComPacketType packetType) {
    const FwPacketDescriptorType descriptorType = packetType;
    static U8 data[sizeof(FwPacketDescriptorType)];
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    context.set_apid(static_cast<ComCfg::Apid::T>(descriptorType));
    context.set_vcId(FprimeRouterTester::TEST_VC_ID);
    this->invoke_to_dataIn(0, buffer, context);
    return buffer;
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
