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
    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! Mock framing protocol
    class MockFramer : public FramingProtocol {
      public:
        MockFramer(Tester& parent);
        void frame(
            const Fw::Buffer& data,
            const Fw::Buffer& context
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

    //! Test incoming Fw::Buffer data to the framer alongside a context
    void test_buffer_and_context(U32 iterations = 1);

    //! Tests statuses pass-through
    void test_status_pass_through();

    //! Tests statuses on no-send
    void test_no_send_status();

    //! Check that buffer is equal to the last buffer allocated
    void check_last_buffer(Fw::Buffer buffer);

    //! Check that context buffer is equal to the last context allocated
    void check_last_context(Fw::Buffer context);

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

    //! Handler for from_contextDeallocate
    //!
    void from_contextDeallocate_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &fwBuffer
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

    //! Buffer for sending context data
    Fw::Buffer m_context;

    //! Mock framing protocol
    MockFramer m_mock;

    //! Whether framing succeeded
    bool m_framed;

    //! Whether sending succeeded
    bool m_sent;

    //! Whether the frame buffer was deallocated
    bool m_returned;

    //! Whether the context buffer was deallocated
    bool m_contextReturned;

    //! Whether the context should be valid
    bool m_contextValid;

    //! Send status for error injection
    Drv::SendStatus m_sendStatus;
};

}  // end namespace Svc

#endif
