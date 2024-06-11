// ======================================================================
// \title  UdpSender/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for UdpSender test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "UdpSenderGTestBase.hpp"
#include "Svc/UdpSender/UdpSenderComponentImpl.hpp"

namespace Svc {

  class Tester :
    public UdpSenderGTestBase
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

      //! open socket test
      //!
      void openTest(const char* server, const char* port);

      //! open socket test
      //!
      void sendTest(const char* server, const char* port);

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
      UdpSenderComponentImpl component;

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::LogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            );

      NATIVE_INT_TYPE m_recvFd;
      void udpRecvStart(const char* port);
      NATIVE_INT_TYPE udpGet(U8* buff, NATIVE_UINT_TYPE size);

  };

} // end namespace Svc

#endif
