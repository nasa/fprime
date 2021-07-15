// ======================================================================
// \title  Framer/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for Framer test harness implementation class
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
#include "Svc/Framer/FramerComponentImpl.hpp"

namespace Svc {


class Tester : public FramerGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------
    class MockFramer : public FramingProtocol {
      public:
        MockFramer(Tester& parent);
        void frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type);
        Tester& m_parent;
    };

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

    //! Test incoming data to the framer
    //!
    void test_incoming(U32 iterations = 1);

    //! Test incoming data to the framer
    //!
    void test_buffer(U32 iterations = 1);

    void check_last_buffer(Fw::Buffer buffer);

    void check_not_freed();
  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_bufferDeallocate
    //!
    void from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                   Fw::Buffer& fwBuffer);

    //! Handler for from_framedAllocate
    //!
    Fw::Buffer from_framedAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                     U32 size);

    //! Handler for from_framedOut
    //!
    Drv::SendStatus from_framedOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                           Fw::Buffer& sendBuffer);

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
    FramerComponentImpl component;

    Fw::Buffer m_buffer;
    MockFramer m_mock;
    bool m_framed;
    bool m_returned;
};

}  // end namespace Svc

#endif
