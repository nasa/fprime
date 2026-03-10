// ======================================================================
// \title  PassThroughRouterTester.hpp
// \author kessler
// \brief  hpp file for PassThroughRouter component test harness implementation class
// ======================================================================

#ifndef Svc_PassThroughRouterTester_HPP
#define Svc_PassThroughRouterTester_HPP

#include "Svc/PassThroughRouter/PassThroughRouter.hpp"
#include "Svc/PassThroughRouter/PassThroughRouterGTestBase.hpp"

#include <Fw/Com/ComPacket.hpp>

namespace Svc {

class PassThroughRouterTester final : public PassThroughRouterGTestBase {
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

    //! Construct object PassThroughRouterTester
    PassThroughRouterTester();

    //! Destroy object PassThroughRouterTester
    ~PassThroughRouterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Route a packet
    void testRouteAPacket();

    //! Allocation failure
    void testAllocationFailure();

    //! Deallocate a returning buffer
    void testBufferReturn();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

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
    PassThroughRouter component;

    Fw::Buffer m_buffer;  // buffer to be returned by mocked bufferAllocate call
    U8 m_buffer_slot[64];
    bool m_forceAllocationError = false;  // Flag to force allocation error
};

}  // namespace Svc

#endif
