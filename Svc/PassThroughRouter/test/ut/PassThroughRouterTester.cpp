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

    ASSERT_from_allPacketsOut_SIZE(1);  // one packet emitted
}

void PassThroughRouterTester::testAllPacketsReturn() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    this->invoke_to_allPacketsReturnIn(0, buffer);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, ComCfg::FrameContext());
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

}  // namespace Svc
