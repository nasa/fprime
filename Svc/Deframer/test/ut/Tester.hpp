// ======================================================================
// \title  Deframer/test/ut/Tester.hpp
// \author janamian
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
#include "Svc/Deframer/DeframerComponentImpl.hpp"

namespace Svc {

class Tester : public DeframerGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------
    class MockDeframer : public DeframingProtocol {
      public:
        MockDeframer(Tester& parent);
        DeframingStatus deframe(Types::CircularBuffer& ring_buffer, U32& needed);
        void test_interface(Fw::ComPacket::ComPacketType  com_type);
        DeframingStatus m_status;
    };

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

    void test_incoming_frame(DeframingProtocol::DeframingStatus status);
    void test_com_interface();
    void test_buffer_interface();
    void test_unknown_interface();

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
    DeframerComponentImpl component;

    Fw::Buffer m_buffer;
    MockDeframer m_mock;
};

}  // end namespace Svc

#endif
