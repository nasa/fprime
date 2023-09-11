// ======================================================================
// \title  Deframer/test/ut/Tester.hpp
// \author janamian, bocchino
// \brief  hpp file for Deframer test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/Deframer/Deframer.hpp"

namespace Svc {

class Tester : public DeframerGTestBase {
  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    struct ConnectStatus {
      //! Whether a port is connected
      typedef enum {
          CONNECTED,
          UNCONNECTED
      } t;
    };

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------
    class MockDeframer : public DeframingProtocol {
      public:
        MockDeframer(Tester& parent);
        DeframingStatus deframe(Types::CircularBuffer& ring_buffer, U32& needed);
        //! Test the implementation of DeframingProtocolInterface provided
        //! by the Deframer component
        void test_interface(Fw::ComPacket::ComPacketType  com_type);
        DeframingStatus m_status;
    };

  public:
    //! Construct object Tester
    //!
    Tester(ConnectStatus::t bufferOutStatus = ConnectStatus::CONNECTED);

    //! Destroy object Tester
    //!
    ~Tester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Send a frame to framedIn
    void test_incoming_frame(
        DeframingProtocol::DeframingStatus status //!< The status that the mock deframer will return
    );

    //! Route a com packet
    void test_com_interface();

    //! Route a file packet
    void test_file_interface();

    //! Route a packet of unknown type
    void test_unknown_interface();

    //! Invoke the command response input port
    void testCommandResponse();

    //! Attempt to route a command packet that is too large
    void testCommandPacketTooLarge();

    //! Attempt to route a packet buffer that is too small
    void testPacketBufferTooSmall();

    //! Route a file packet with bufferOutUnconnected
    void testBufferOutUnconnected();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_bufferOut
    //!
    void from_deframedOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                  Fw::Buffer& fwBuffer,
                                  Fw::Buffer& context);

    //! Handler for from_bufferAllocate
    //!
    Fw::Buffer from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                           U32 size);

    //! Handler for from_framedDeallocate
    //!
    void from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

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
    Deframer component;

    Fw::Buffer m_buffer;
    MockDeframer m_mock;
    // Whether the bufferOut port is connected
    ConnectStatus::t bufferOutStatus;
};

}  // end namespace Svc

#endif
