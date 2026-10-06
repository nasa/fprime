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
    //! Construct the tester, optionally leaving the product ports of the component unconnected
    TlmPacketizerTester(bool connectProductPorts = true);

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

    //! Data products: nothing is requested or sent unless recording was commanded
    void dpDisabledByDefaultTest(void);

    //! Data products: a recorded group fills and sends a container holding its packets; downlink unchanged
    void dpRecordGroupTest(void);

    //! Data products: stop and restart send the partially filled container
    void dpStopAndRestartTest(void);

    //! Data products: packets are recorded even when their downlink is disabled
    void dpRecordWhenDownlinkDisabledTest(void);

    //! Data products: invalid command arguments are rejected with VALIDATION_ERROR
    void dpCommandRejectTest(void);

    //! Data products: a failed container allocation drops the packet and is reported
    void dpAllocationFailureTest(void);

    //! START_DP_RECORDING is rejected while the product ports are not connected
    void dpPortsNotConnectedTest(void);

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

    //! Handler for product get: hand out m_dpBuffer unless m_dpAllocationFailure is set
    Fw::Success::T productGet_handler(FwDpIdType id,       /*!< The container ID*/
                                      FwSizeType dataSize, /*!< The requested size*/
                                      Fw::Buffer& buffer   /*!< The buffer*/
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
    void connectPorts(bool connectProductPorts);

    //! Initialize components
    //!
    void initComponents(void);

    //! Reset Counter
    //!
    void resetCounter(void);

    //! Serialize a U32 channel value into the component
    void pushU32Channel(FwChanIdType id, U32 value);

    //! Check a sent data product container: header, group record, then one packet record per expected packet
    //!
    //! When packets is nullptr, only the record count is checked
    void checkDpContainer(const Fw::Buffer& buffer,          /*!< The sent container buffer*/
                          FwChanIdType tlmGroup,             /*!< Expected group*/
                          FwDpPriorityType priority,         /*!< Expected priority*/
                          FwSizeType packetCount,            /*!< Expected number of packet records*/
                          const Fw::ComBuffer* const packets /*!< Expected packets, or nullptr*/
    );

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

    static const FwSizeType DP_BUFFER_SIZE = 1024;  //!< Enough for the containers requested by the tests
    U8 m_dpBuffer[DP_BUFFER_SIZE]{};                //!< Memory handed out by productGet_handler
    bool m_dpAllocationFailure = false;             //!< When set, productGet_handler fails
};

}  // end namespace Svc

#endif
