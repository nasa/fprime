// ======================================================================
// \title  FprimeRouterTester.hpp
// \author thomas-bc
// \brief  hpp file for FprimeRouter component test harness implementation class
// ======================================================================

#ifndef Svc_FprimeRouterTester_HPP
#define Svc_FprimeRouterTester_HPP

#include "Svc/FprimeRouter/FprimeRouter.hpp"
#include "Svc/FprimeRouter/FprimeRouterGTestBase.hpp"

#include <Fw/Com/ComPacket.hpp>

namespace Svc {

class FprimeRouterTester : public FprimeRouterGTestBase {
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

    //! Construct object FprimeRouterTester
    //! \param disconnect_unknownData_port if set to true, the unknownData output port will not be connected
    //! in the test harness setup. If false (default), all ports will be connected.
    explicit FprimeRouterTester(bool disconnect_unknownData_port = false);

    //! Destroy object FprimeRouterTester
    ~FprimeRouterTester();

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

    //! Route a packet of unknown type
    void testRouteUnknownPacketUnconnected();

    //! Deallocate a returning buffer
    void testBufferReturn();

    //! Invoke the command response input port
    void testCommandResponse();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect all ports
    void connectPorts();

    //! Connect all ports except unknownDataOut output port
    void connectPortsExceptUnknownData();

    //! Initialize components
    void initComponents();

    //! Mock the reception of a packet of a specific type
    void mockReceivePacketType(Fw::ComPacketType packetType);

    // ----------------------------------------------------------------------
    // Port handler overrides
    // ----------------------------------------------------------------------
    //! Overriding bufferAllocate handler to be able to request a buffer in component tests
    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FprimeRouter component;

    Fw::Buffer m_buffer;  // buffer to be returned by mocked bufferAllocate call
    U8 m_buffer_slot[64];
};

}  // namespace Svc

#endif
