// ======================================================================
// \title  Framer/test/ut/Tester.hpp
// \author mstarch, bocchino
// \brief  hpp file for Framer test harness implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/Framer/Framer.hpp"

namespace Svc {


class Tester : public FramerGTestBase {
  public:

    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! Mock framing protocol
    class MockFramer : public FramingProtocol {
      public:
        MockFramer(Tester& parent);
        void frame(
            const U8* const data,
            const U32 size,
            Fw::ComPacket::ComPacketType packet_type
        );
        Tester& m_parent;
        bool m_do_not_send;
    };

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:

    //! Construct object Tester
    Tester();

    //! Destroy object Tester
    ~Tester();

  public:

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test incoming Fw::Com data to the framer
    void test_com(U32 iterations = 1);

    //! Test incoming Fw::Buffer data to the framer
    void test_buffer(U32 iterations = 1);

    //! Tests statuses pass-through
    void test_status_pass_through();

    //! Tests statuses on no-send
    void test_no_send_status();

    //! Check that buffer is equal to the last buffer allocated
    void check_last_buffer(Fw::Buffer buffer);

  private:

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_bufferDeallocate
    void from_bufferDeallocate_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        Fw::Buffer& fwBuffer //!< The buffer
    );

    //! Handler for from_framedAllocate
    Fw::Buffer from_framedAllocate_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        U32 size //!< The size
    );

    //! Handler for from_framedOut
    Drv::SendStatus from_framedOut_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        Fw::Buffer& sendBuffer //!< The buffer containing framed data
    );

    //! Handler for from_comStatusOut
    //!
    void from_comStatusOut_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Success &condition /*!< Condition success/failure */
    );

  public:

    // ----------------------------------------------------------------------
    // Public instance methods
    // ----------------------------------------------------------------------

    //! Set the send status
    void setSendStatus(Drv::SendStatus sendStatus);

  private:

    // ----------------------------------------------------------------------
    // Private instance methods
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:

    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    Framer component;

    //! Buffer for sending unframed data
    Fw::Buffer m_buffer;

    //! Mock framing protocol
    MockFramer m_mock;

    //! Whether framing succeeded
    bool m_framed;

    //! Whether sending succeeded
    bool m_sent;

    //! Whether the frame buffer was deallocated
    bool m_returned;

    //! Send status for error injection
    Drv::SendStatus m_sendStatus;
};

}  // end namespace Svc

#endif
