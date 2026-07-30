// ======================================================================
// \title  SdlsSaRouterTester.hpp
// \author lestarch-autobot
// \brief  hpp file for SdlsSaRouter component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_SdlsSaRouterTester_HPP
#define Svc_Ccsds_SdlsSaRouterTester_HPP

#include "Svc/Ccsds/SdlsSaRouter/SdlsSaRouter.hpp"
#include "Svc/Ccsds/SdlsSaRouter/SdlsSaRouterGTestBase.hpp"
#include "Svc/Ccsds/SdlsSaRouter/test/ut/TestState/TestState.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

namespace Ccsds {

class SdlsSaRouterTester : public SdlsSaRouterGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    //! Size of each test buffer in the pool
    static const FwSizeType TEST_BUFFER_SIZE = 64;

    //! Downstream port left unconnected to exercise the UNKNOWN_PORT status
    static const FwIndexType UNCONNECTED_PORT = SdlsCfg::SaRouterPortCount - 1;

    //! Shadow sentinel marking buffers forwarded by the router itself on routing errors
    static const FwIndexType ROUTER_ERROR_PORT = -1;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object SdlsSaRouterTester
    SdlsSaRouterTester();

    //! Destroy object SdlsSaRouterTester
    ~SdlsSaRouterTester();

  private:
    // ----------------------------------------------------------------------
    // Handler overrides for typed from ports
    // ----------------------------------------------------------------------

    //! Override recording the invoked port number
    void from_saDataOut_handler(FwIndexType portNum,
                                U16 securityAssociationIndex,
                                Fw::Buffer& data,
                                const ComCfg::FrameContext& context) override;

    //! Override recording the invoked port number
    void from_saDataReturnOut_handler(FwIndexType portNum,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports (auto-generated; unused in favor of connectPortsCustom)
    void connectPorts();

    //! Connect ports, leaving saDataOut[UNCONNECTED_PORT] unconnected
    void connectPortsCustom();

    //! Initialize components
    void initComponents();

  public:
    //! Return a pool buffer pointer not currently outstanding, or nullptr if all are in use
    U8* getFreePoolBuffer();

    //! Construct an Fw::Buffer over pool storage with a unique allocation context
    Fw::Buffer makePoolBuffer(U8* storage);

    //! Return true if the SA appears in the configured map
    bool isMappedSa(U16 sa) const;

    //! Return the map index of a random entry routed to a connected port
    FwSizeType pickConnectedEntry() const;

  public:
    // ----------------------------------------------------------------------
    // Directed test scenarios
    // ----------------------------------------------------------------------

    //! Fill the outstanding table, then verify a routing-error drop on dataIn
    void testTableFullDataIn();

    //! Verify an untracked buffer on dataReturnIn is returned upstream
    void testUntrackedDataReturn();

    //! Fill the outstanding table, then verify a drop on saDataIn returns downstream
    void testTableFullSaDataIn();

    //! Saturate the outstanding table via saDataIn using every pool buffer
    void fillOutstandingTable();

    //! Route a mapped SA with the tester looping saDataOut back into saDataIn
    //! synchronously, mirroring a passive crypto component (deadlocks if the
    //! downstream inputs regress to guarded)
    void testSynchronousCryptoLoopback();

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SdlsSaRouter component;

    //! Shadow state for rule-based testing
    SdlsSaRouterTestState shadow;

    //! Port number of the last from_saDataOut invocation
    FwIndexType m_lastSaDataOutPort = -1;

    //! Port number of the last from_saDataReturnOut invocation
    FwIndexType m_lastSaDataReturnOutPort = -1;

    //! When true, from_saDataOut_handler loops the data back into saDataIn synchronously
    bool m_synchronousLoopback = false;

    //! Pool of buffers used as outstanding processed data
    U8 m_pool[SdlsCfg::SaRouterMaxOutstandingBuffers][TEST_BUFFER_SIZE] = {};

    //! SAs from the compile-time routing map (SdlsCfg::SaMap defaults)
    U16 m_mapSas[SdlsCfg::SaRouterMapEntryCount];

    //! Expected downstream port for each map entry
    FwIndexType m_mapPorts[SdlsCfg::SaRouterMapEntryCount];

  public:
    // ----------------------------------------------------------------------
    // Rule Based Testing
    // ----------------------------------------------------------------------

    //! Rules for the dataIn port (SA routing)
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, KnownSa);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, UnknownSa);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, UnknownPort);

    //! Rules for the processed data and buffer return paths
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, ProcessedData);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, ProcessedDataReturn);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, BufferReturn);
};

}  // namespace Ccsds

}  // namespace Svc

#endif
