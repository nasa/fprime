// ======================================================================
// \title  PassThroughRouterTester.cpp
// \author kessler
// \brief  cpp file for PassThroughRouter component test harness implementation class
// ======================================================================

#include "PassThroughRouterTester.hpp"
#include "PassThroughRouterGTestBase.hpp"
#include "Svc/FprimeRouter/FprimeRouterGTestBase.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

PassThroughRouterTester

    ::PassThroughRouterTester()
    : PassThroughRouterGTestBase("PassThroughRouterTester", PassThroughRouterTester::MAX_HISTORY_SIZE),
      component("PassThroughRouter") {
    this->initComponents();
    this->connectPorts();
}

PassThroughRouterTester::~PassThroughRouterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void PassThroughRouterTester::testRouteAPacket() {
    this->mockReceivePacketType(Fw::ComPacketType::FW_PACKET_COMMAND);

    ASSERT_from_bufferAllocate_SIZE(1);     // one buffer allocation
    ASSERT_EVENTS_AllocationError_SIZE(0);  // no allocation errors
    ASSERT_from_allPacketsOut_SIZE(1);      // one packet emitted
    ASSERT_from_dataReturnOut_SIZE(1);      // data ownership should always be returned
}

void PassThroughRouterTester::testAllocationFailure() {
    this->m_forceAllocationError = true;
    this->mockReceivePacketType(Fw::ComPacketType::INVALID_UNINITIALIZED);

    ASSERT_from_bufferAllocate_SIZE(1);     // one buffer allocation
    ASSERT_EVENTS_AllocationError_SIZE(1);  // one allocation error
    ASSERT_from_allPacketsOut_SIZE(0);      // no packets emitted when allocation error occurs
    ASSERT_from_dataReturnOut_SIZE(1);      // data ownership should always be returned
}

void PassThroughRouterTester::testBufferReturn() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    this->invoke_to_allPacketsReturnIn(0, buffer);
    ASSERT_from_bufferDeallocate_SIZE(1);  // buffer should be deallocated
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getData(), data);
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getSize(), sizeof(data));
}

// ----------------------------------------------------------------------
// Test Helper
// ----------------------------------------------------------------------

void PassThroughRouterTester::mockReceivePacketType(Fw::ComPacketType packetType) {
    U8 data[sizeof(packetType)] = {};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    context.set_apid(packetType);
    this->invoke_to_dataIn(0, buffer, context);
}

// ----------------------------------------------------------------------
// Port handler overrides
// ----------------------------------------------------------------------

Fw::Buffer PassThroughRouterTester::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    this->pushFromPortEntry_bufferAllocate(size);
    if (this->m_forceAllocationError) {
        this->m_buffer.setData(nullptr);
        this->m_buffer.setSize(0);
    } else {
        this->m_buffer.setData(this->m_buffer_slot);
        this->m_buffer.setSize(size);
        ::memset(this->m_buffer.getData(), 0, size);
    }
    return this->m_buffer;
}

}  // namespace Svc
