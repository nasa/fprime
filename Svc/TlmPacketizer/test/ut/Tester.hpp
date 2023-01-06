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

#include "GTestBase.hpp"
#include "Svc/TlmPacketizer/TlmPacketizer.hpp"

namespace Svc {

class Tester : public TlmPacketizerGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object Tester
    //!
    Tester(void);

    //! Destroy object Tester
    //!
    ~Tester(void);

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

    //! Handler for from_Time
    //!
    void from_Time_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           Fw::Time& time                 /*!< The U32 cmd argument*/
    );

    //! Handler for from_PktSend
    //!
    void from_PktSend_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                              U32 context                    /*!< Call context value; meaning chosen by user*/
    );

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              U32 key                        /*!< Value to return to pinger*/
    );

    virtual void textLogIn(const FwEventIdType id,         /*!< The event ID*/
                           Fw::Time& timeTag,              /*!< The time*/
                           const Fw::LogSeverity severity, /*!< The severity*/
                           const Fw::TextLogString& text   /*!< The event string*/
    );

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
    bool m_timeSent;      //!< flag when time was sent
};

}  // end namespace Svc

#endif
