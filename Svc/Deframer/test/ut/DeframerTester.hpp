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

#include "DeframerGTestBase.hpp"
#include "Svc/Deframer/Deframer.hpp"

namespace Svc {

class DeframerTester : public DeframerGTestBase {
  public:
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
        MockDeframer(DeframerTester& parent);
        DeframingStatus deframe(Types::CircularBuffer& ring_buffer, U32& needed);
        //! Test the implementation of DeframingProtocolInterface provided
        //! by the Deframer component
        void test_interface(Fw::ComPacket::ComPacketType  com_type);
        DeframingStatus m_status;
    };

  public:
    //! Construct object DeframerTester
    //!
    DeframerTester(ConnectStatus::t bufferOutStatus = ConnectStatus::CONNECTED);

    //! Destroy object DeframerTester
    //!
    ~DeframerTester();

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

    //! Handler for from_comOut
    //!
    void from_comOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                             U32 context                    /*!< Call context value; meaning chosen by user*/
    );

    //! Handler for from_bufferOut
    //!
    void from_bufferOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                Fw::Buffer& fwBuffer);

    //! Handler for from_bufferAllocate
    //!
    Fw::Buffer from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                           U32 size);

    //! Handler for from_bufferDeallocate
    //!
    void from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

    //! Handler for from_framedDeallocate
    //!
    void from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

    //! Handler for from_framedPoll
    //!
    Drv::PollStatus from_framedPoll_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                            Fw::Buffer& pollBuffer);

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
