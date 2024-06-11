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

#include "TlmPacketizerGTestBase.hpp"
#include "Svc/TlmPacketizer/TlmPacketizer.hpp"

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

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_PktSend
    //!
    void from_PktSend_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                              U32 context                    /*!< Call context value; meaning chosen by user*/
    ) override;

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              U32 key                        /*!< Value to return to pinger*/
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

    //! Connect ports
    //!
    void connectPorts(void);

    //! Initialize components
    //!
    void initComponents(void);

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    TlmPacketizer component;

    Fw::Time m_testTime;  //!< store test time for packets
};

}  // end namespace Svc

#endif
