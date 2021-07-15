// ======================================================================
// \title  GenericHub/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for GenericHub test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP
#include <Fw/Com/ComBuffer.hpp>
#include "GTestBase.hpp"
#include "Svc/GenericHub/GenericHubComponentImpl.hpp"

// Larger than com buffer size
#define DATA_SIZE (FW_COM_BUFFER_MAX_SIZE*10 + sizeof(U32)  + sizeof(U32) + sizeof(FwBuffSizeType))

namespace Svc {

class Tester : public GenericHubGTestBase {
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

    //! Test of basic in/out of a set of serialized ports
    //!
    void test_in_out();

    //! Test of buffer in/out of a set of buffer ports
    //!
    void test_buffer_io();

    //! Test of random in/out of a set of file and normal ports
    //!
    void test_random_io();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_buffersOut
    //!
    void from_buffersOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                 Fw::Buffer& fwBuffer);

    //! Handler for from_bufferDeallocate
    //!
    void from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

    //! Handler for from_dataOutAllocate
    //!
    Fw::Buffer from_dataOutAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                            U32 size);

    //! Handler for from_dataOut
    //!
    void from_dataOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                              Fw::Buffer& fwBuffer);

    //! Handler for from_dataDeallocate
    //!
    void from_dataInDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                     Fw::Buffer& fwBuffer);

  private:
    // ----------------------------------------------------------------------
    // Handlers for serial from ports
    // ----------------------------------------------------------------------

    //! Handler for from_portOut
    //!
    void from_portOut_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                              Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
    );

  private:
    void send_random_comm(U32 port);

    void send_random_buffer(U32 port);

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
    GenericHubComponentImpl componentIn;
    GenericHubComponentImpl componentOut;
    Fw::ComBuffer m_comm;
    Fw::Buffer m_buffer;
    Fw::Buffer m_allocate;
    U32 m_comm_in;
    U32 m_buffer_in;
    U32 m_comm_out;
    U32 m_buffer_out;
    U32 m_current_port;
    U8 m_data_store[DATA_SIZE];
    U8 m_data_for_allocation[DATA_SIZE];
};

}  // end namespace Svc

#endif
