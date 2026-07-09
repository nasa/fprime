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
    void from_saDecryptOut_handler(FwIndexType portNum,
                                   U16 securityAssociationIndex,
                                   Fw::Buffer& data,
                                   const ComCfg::FrameContext& context) override;

    //! Override recording the invoked port number
    void from_saDecryptReturnOut_handler(FwIndexType portNum,
                                         Fw::Buffer& data,
                                         const ComCfg::FrameContext& context) override;

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports (auto-generated; unused in favor of connectPortsCustom)
    void connectPorts();

    //! Connect ports, leaving saDecryptOut[UNCONNECTED_PORT] unconnected
    void connectPortsCustom();

    //! Initialize components
    void initComponents();

  public:
    //! Return a pool buffer pointer not currently outstanding, or nullptr if all are in use
    U8* getFreePoolBuffer();

    //! Return true if the SA appears in the configured map
    bool isMappedSa(U16 sa) const;

    //! Return the map index of a random entry routed to a connected port
    FwSizeType pickConnectedEntry() const;

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    SdlsSaRouter component;

    //! Shadow state for rule-based testing
    SdlsSaRouterTestState shadow;

    //! Port number of the last from_saDecryptOut invocation
    FwIndexType m_lastSaDecryptOutPort = -1;

    //! Port number of the last from_saDecryptReturnOut invocation
    FwIndexType m_lastSaDecryptReturnOutPort = -1;

    //! Pool of buffers used as outstanding decrypted data
    U8 m_pool[SdlsCfg::SaRouterMaxOutstandingBuffers][TEST_BUFFER_SIZE];

    //! SAs from the compile-time routing map (SdlsCfg::SaMap defaults)
    U16 m_mapSas[SdlsCfg::SaRouterMapEntryCount];

    //! Expected downstream port for each map entry
    FwIndexType m_mapPorts[SdlsCfg::SaRouterMapEntryCount];

  public:
    // ----------------------------------------------------------------------
    // Rule Based Testing
    // ----------------------------------------------------------------------

    //! Rules for the decryptIn port (SA routing)
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, KnownSa);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, UnknownSa);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, Route, UnknownPort);

    //! Rules for the decrypted data and buffer return paths
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, DecryptData);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, DecryptReturn);
    FW_RBT_DEFINE_RULE(SdlsSaRouterTester, DataFlow, BufferReturn);
};

}  // namespace Ccsds

}  // namespace Svc

#endif
