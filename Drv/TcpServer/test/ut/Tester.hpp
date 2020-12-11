// ======================================================================
// \title  TcpServer/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for ByteStreamDriverModel test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Drv/TcpServer/TcpServerComponentImpl.hpp"
#include "Drv/Ip/TcpClientSocket.hpp"

#define SEND_DATA_BUFFER_SIZE 1024

namespace Drv {

  class Tester :
    public ByteStreamDriverModelGTestBase
  {

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


      //! Test basic messaging
      //!
      void test_basic_messaging(void);

      //! Test basic reconnection behavior
      //!
      void test_multiple_messaging(void);

      //! Test receive via thread
      //!
      void test_receive_thread(void);

      //! Test advanced (duration) reconnect
      //!
      void test_advanced_reconnect(void);

      // Helpers
      void test_with_loop(U32 iterations, bool recv_thread=false);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_recv
      //!
      void from_recv_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &recvBuffer, 
          RecvStatus recvStatus 
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
      TcpServerComponentImpl component;
      Fw::Buffer m_data_buffer;
      U8 m_data_storage[SEND_DATA_BUFFER_SIZE];
      bool m_spinner;

  };

} // end namespace Drv

#endif
