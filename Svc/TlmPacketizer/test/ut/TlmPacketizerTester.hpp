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
    //! the override table + mirror one entry out packetOverrideOut; bad args/id -> VALIDATION_ERROR
    void perPacketCommandsTest(void);

    //! packetOverrideIn reload test: a batch pushed over packetOverrideIn is applied to the volatile table
    //! (unknown ids warned + skipped) and is not echoed back out packetOverrideOut
    void configInReloadTest(void);

    //! GET_PACKET_CONFIG test: effective config reported for a known id; unknown id warns
    void getPacketConfigTest(void);

    //! Seeding test: the first per-packet command seeds the override from the packet's current
    //! effective (group-derived) config, so untouched rate fields carry through
    void seedFromEffectiveConfigTest(void);

    //! CLEAR_PACKET_OVERRIDE test: an override is cleared back to group behavior and the cleared
    //! state is mirrored out packetOverrideOut; unknown id -> VALIDATION_ERROR + warning
    void clearPacketOverrideTest(void);

    //! Mirror allocation failure test: when bufferAllocate yields no buffer, the command answers
    //! EXECUTION_ERROR, ConfigMirrorAllocFailed is emitted, and the override is NOT applied locally
    void mirrorAllocFailTest(void);

    //! packetOverrideIn clear reload test: a GROUP_DEFAULT record reverts an overridden packet to group behavior
    void configInClearReloadTest(void);

    //! packetOverrideIn malformed reload test: a truncated/invalid record ends the reload with ConfigReloadFailed,
    //! records before it still apply, and an invalid buffer is still returned via bufferDeallocate
    void configInMalformedReloadTest(void);

    //! Forced-group test: an overridden-disabled packet stays silent in a Force-Enabled group
    void overrideDisabledInForcedGroupTest(void);

    //! Helper to set the component into a stock-configuration regardless of default config
    //!
    void stockConfiguration();

    //! Helper to populate the six channels backing packets id 4 and id 8 ahead of a Run
    void fillPacketChannels();

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

    //! Handler for from_packetOverrideOut: captures the per-packet override mirror sent to external component
    void from_packetOverrideOut_handler(FwIndexType portNum,  //!< The port number
                                        Fw::Buffer& fwBuffer  //!< The mirrored override (serialized)
                                        ) override;

    //! Handler for from_bufferAllocate: hand back tester-owned storage (mocked BufferManager)
    Fw::Buffer from_bufferAllocate_handler(FwIndexType portNum,  //!< The port number
                                           FwSizeType size       //!< The requested size
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

    //! packetOverrideOut mirror capture (per-packet override sent to an external component)
    U32 m_configOutInvokes{0};                   //!< Number of packetOverrideOut invocations
    Svc::PacketConfigEntry m_lastConfigEntry{};  //!< Entry deserialized from the most recent mirror buffer

    //! Backing storage for the mocked bufferAllocate port (stands in for a BufferManager pool)
    static const FwSizeType CONFIG_BUF_SLOT_SIZE = 256;
    U8 m_configBufSlot[CONFIG_BUF_SLOT_SIZE]{};
    Fw::Buffer m_configBuf;
    bool m_allocFail{false};  //!< When set, bufferAllocate returns an invalid buffer (pool exhausted)
};

}  // end namespace Svc

#endif
