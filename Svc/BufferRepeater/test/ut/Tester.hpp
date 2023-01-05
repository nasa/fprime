// ======================================================================
// \title  BufferRepeater/test/ut/Tester.hpp
// \author lestarch
// \brief  hpp file for GenericRepeater test harness implementation class
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
#include "Svc/BufferRepeater/BufferRepeater.hpp"

namespace Svc {

class Tester : public BufferRepeaterGTestBase {
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

    //! Test the repeating capability of the buffer
    //!
    void testRepeater();

    //! Test the repeating capability of the buffer
    //!
    void testFailure(BufferRepeater::BufferRepeaterFailureOption failure_option);

  private:
    // ----------------------------------------------------------------------
    // Handlers for serial from ports
    // ----------------------------------------------------------------------

    //! Handler for from_allocate
    //!
    Fw::Buffer from_allocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                     U32 size);

    //! Handler for from_deallocate
    //!
    void from_deallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                 Fw::Buffer& fwBuffer);

    //! Handler for from_portOut
    //!
    void from_portOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
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
    BufferRepeater component;
    History<NATIVE_INT_TYPE> m_port_index_history;
    Fw::Buffer m_initial_buffer;
    bool m_failure;
};

}  // end namespace Svc

#endif
