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

    // Maximum size of histories storing events, telemetry, and port outputs.
    // Must exceed FprimeRouterCfg::BufferContextTableSize so the table-full test can
    // send more buffers than the table holds without overflowing port history.
    static const FwSizeType MAX_HISTORY_SIZE = FprimeRouterCfg::BufferContextTableSize + 5;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Sentinel virtual channel id used to verify context is preserved across the file round-trip
    static const U8 TEST_VC_ID = 3;

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

    //! Test buffer return via fileBufferReturnIn
    void testBufferReturn();

    //! Invoke the command response input port
    void testCommandResponse();

    //! A file buffer's context is saved on fileOut and restored on fileBufferReturnIn
    void testFileContextRoundTrip();

    //! Multiple outstanding file buffers each get their own context restored,
    //! even when returned out of order
    void testMultiBufferContextRoundTrip();

    //! When the context table is full, a handed-off buffer emits the table-full
    //! event for its routed port and its context degrades to empty on return.
    //! \param packetType FW_PACKET_FILE (fileOut path) or FW_PACKET_UNKNOWN (unknownDataOut path)
    void testContextTableFull(Fw::ComPacketType packetType);

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

    //! Mock the reception of a packet of a specific type.
    //! \return the buffer that was sent in on dataIn (the router emits the same
    //! handle on fileOut/unknownDataOut for connected handoff paths).
    Fw::Buffer mockReceivePacketType(Fw::ComPacketType packetType);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FprimeRouter component;
};

}  // namespace Svc

#endif
