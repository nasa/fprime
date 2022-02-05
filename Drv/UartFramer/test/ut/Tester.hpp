// ======================================================================
// \title  UartFramer/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for UartFramer test harness implementation class
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
#include "Drv/UartFramer/UartFramer.hpp"

#define NUM_INIT_BUFFERS 5

namespace Drv {

  class Tester :
    public UartFramerGTestBase
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

      //! Initialization
      //!
      void initialization();

      //! packet send
      //!
      void packetSend();

      //! packet receive
      //!
      void packetReceive();

      //! packet receive error
      //! 
      void packetReceiveError();

      //! error allocating UART receive buffer
      //! 
      void packetAllocationError();


    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_Deframer
      //!
      void from_Deframer_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &recvBuffer, 
          const Drv::RecvStatus &recvStatus 
      );

      //! Handler for from_DeframerAllocate
      //!
      Fw::Buffer from_DeframerAllocate_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size 
      );

      //! Handler for from_FramerDeallocate
      //!
      void from_FramerDeallocate_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler for from_readBufferSend
      //!
      void from_readBufferSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer 
      );

      //! Handler for from_serialSend
      //!
      void from_serialSend_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer 
      );

      void textLogIn(
          const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
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
      UartFramer component;

      Fw::Buffer m_buffPool[NUM_INIT_BUFFERS];
      NATIVE_UINT_TYPE m_currBuff;
      bool m_emptyAlloc;

  };

} // end namespace Drv

#endif
