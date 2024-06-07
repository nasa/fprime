// ======================================================================
// \title  RouterTester.hpp
// \author chammard
// \brief  hpp file for Router component test harness implementation class
// ======================================================================

#ifndef Svc_RouterTester_HPP
#define Svc_RouterTester_HPP

#include "Svc/Router/Router.hpp"
#include "Svc/Router/RouterGTestBase.hpp"

#include <Fw/Com/ComPacket.hpp>

namespace Svc {

class RouterTester : public RouterGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object RouterTester
    RouterTester();

    //! Destroy object RouterTester
    ~RouterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Route a com packet
    void testRouteComInterface();

    //! Route a file packet
    void testRouteFileInterface();

    //! Route a packet of unknown type
    void testRouteUnknownPacket();

    //! Invoke the command response input port
    void testCommandResponse();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Mock the reception of a packet of a specific type
    void mockReceivePacketType(Fw::ComPacket::ComPacketType packetType);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    Router component;
};

}  // namespace Svc

#endif
