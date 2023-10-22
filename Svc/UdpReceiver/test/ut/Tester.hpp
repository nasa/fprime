// ======================================================================
// \title  UdpReceiver/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for UdpReceiver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "UdpReceiverGTestBase.hpp"
#include "Svc/UdpReceiver/UdpReceiverComponentImpl.hpp"

namespace Svc {

  class Tester :
    public UdpReceiverGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Open the connection
      //!
      void openTest(const char* port);

      //! Receive a packet
      //!
      void recvTest(const char* port);

    private:

      // ----------------------------------------------------------------------
      // Handlers for serial from ports
      // ----------------------------------------------------------------------

      //! Handler for from_PortsOut
      //!
      void from_PortsOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      UdpReceiverComponentImpl component;

      // helper to send a packet
      void sendPacket(U32 val, const char* addr, const char* port, NATIVE_UINT_TYPE seq, NATIVE_UINT_TYPE portNum);

      // stored port call arguments
      U32 m_sentVal;
      NATIVE_INT_TYPE m_sentPort;

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::LogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            );

  };

} // end namespace Svc

#endif
