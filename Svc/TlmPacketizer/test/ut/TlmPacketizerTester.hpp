// ======================================================================
// \title  TlmPacketizer/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for TlmPacketizer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef TESTER_HPP
#define TESTER_HPP

#include "Svc/TlmPacketizer/TlmPacketizer.hpp"
#include "TlmPacketizerGTestBase.hpp"

namespace Svc {

class TlmPacketizerTester : public TlmPacketizerGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object TlmPacketizerTester
    //!
    TlmPacketizerTester(void);

    //! Destroy object TlmPacketizerTester
    //!
    ~TlmPacketizerTester(void);

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Initialization test
    //!
    void initTest(void);

    //! push telemetry test
    //!
    void pushTlmTest(void);

    //! send packets test
    //!
    void sendPacketsTest(void);

    //! send packets with levels test
    //!
    void sendPacketLevelsTest(void);

    //! update packets test
    //!
    void updatePacketsTest(void);

    //! non-packetized channel test
    //!
    void nonPacketizedChannelTest(void);

    //! ping test
    //!
    void pingTest(void);

    //! ignore test
    //!
    void ignoreTest(void);

    //! manually send packet test
    //!
    void sendManualPacketTest(void);

    //! set packet level test
    //!
    void setPacketLevelTest(void);

    //! get channel value test
    //!
    void getChannelValueTest(void);

    //! Configured tlm groups test
    //!
    void configuredTelemetryGroupsTests(void);

    //! Configure telemetry enable logic
    //!
    void advancedControlGroupTests(void);

    //! Parameter test: SECTIONS_ENABLED
    //!
    void sectionEnabledParameterTest(void);

    //! Parameter test: SECTIONS_CONFIG
    //!
    void sectionConfigParameterTest(void);

    //! Commanding test: verify SET_LEVEL invalid-level returns VALIDATION_ERROR
    void setLevelInvalidTest(void);

    //! Duplicate channel ID across packets with identical size is accepted
    void duplicateChannelIdMatchingSizeTest(void);

    //! Duplicate channel ID across packets with conflicting size asserts
    void duplicateChannelIdConflictingSizeTest(void);

    //! Oversized channel value is rejected with a warning event
    void oversizedChannelTest(void);

    //! Packet specification with no channels is accepted and can be sent header-only
    void emptyPacketTest(void);

    //! Non-empty packet with a nullptr channel list is still rejected by the configuration assert
    void nullChannelListTest(void);
    
    //! Per-packet override test: an ENABLE_PACKET override disables one packet/section while
    //! the group-enabled remainder still sends (per-packet control)
    void perPacketOverrideTest(void);

    //! Per-packet command test: ENABLE_PACKET / FORCE_PACKET / CONFIGURE_PACKET_RATES update
    //! the override table + mirror one entry out configOut; bad args/id -> VALIDATION_ERROR
    void perPacketCommandsTest(void);

    //! configIn reload test: a batch pushed over configIn is applied to the volatile table
    //! (unknown ids warned + skipped) and is not echoed back out configOut
    void configInReloadTest(void);

    //! GET_PACKET_CONFIG test: effective config reported for a known id; unknown id warns
    void getPacketConfigTest(void);

    //! Helper to set the component into a stock-configuration regardless of default config
    //!
    void stockConfiguration();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_PktSend
    //!
    void from_PktSend_handler(const FwIndexType portNum, /*!< The port number*/
                              Fw::ComBuffer& data,       /*!< Buffer containing packet data*/
                              U32 context                /*!< Call context value; meaning chosen by user*/
                              ) override;

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
                              ) override;

    //! Handler for from_configOut: captures the per-packet override mirror sent to external component
    void from_configOut_handler(FwIndexType portNum,                //!< The port number
                                FwSizeType count,                   //!< Number of valid entries
                                const Svc::PacketConfigBatch& batch  //!< The mirrored overrides
                                ) override;

    virtual void textLogIn(const FwEventIdType id,         /*!< The event ID*/
                           const Fw::Time& timeTag,        /*!< The time*/
                           const Fw::LogSeverity severity, /*!< The severity*/
                           const Fw::TextLogString& text   /*!< The event string*/
                           ) override;

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void pushAllChannels(Fw::Time& ts, Fw::TlmBuffer& buff);

    //! Connect ports
    //!
    void connectPorts(void);

    //! Initialize components
    //!
    void initComponents(void);

    //! Reset Counter
    //!
    void resetCounter(void);

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    TlmPacketizer component;

    Fw::Time m_testTime;  //!< store test time for packets

    // bool m_primaryTestLock{true};  //! Lock limited to entries from port 0 PktSend
    FwSizeType m_portOutInvokes[Svc::TELEMETRY_SEND_PORTS]{};

    //! configOut mirror capture (per-packet override sent to an external component)
    U32 m_configOutInvokes{0};              //!< Number of configOut invocations
    FwSizeType m_lastConfigCount{0};        //!< Count arg of the most recent configOut invocation
    Svc::PacketConfigBatch m_lastConfigBatch{};  //!< Batch of the most recent configOut invocation
};

}  // end namespace Svc

#endif
