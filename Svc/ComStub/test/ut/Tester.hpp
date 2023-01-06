// ======================================================================
// \title  ComStub/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for ComStub test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/ComStub/ComStub.hpp"

namespace Svc {

class Tester : public ComStubGTestBase {
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
    //! Buffer to fill with data
    //!
    void fill(Fw::Buffer& buffer_to_fill);
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial READY setup
    //!
    void test_initial();

    //! Tests the basic input and output of the component
    //!
    void test_basic();

    //! Tests the basic failure case for the component
    //!
    void test_fail();

    //! Tests the basic failure retry component
    //!
    void test_retry();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_comDataOut
    //!
    void from_comDataOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                 Fw::Buffer& recvBuffer,
                                 const Drv::RecvStatus& recvStatus);

    //! Handler for from_comStatus
    //!
    void from_comStatus_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                Fw::Success& condition         /*!<
                                    Status of communication state
                                    */
    );

    //! Handler for from_drvDataOut
    //!
    Drv::SendStatus from_drvDataOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
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
    ComStub m_component;
    Drv::SendStatus m_send_mode;  //! Send mode
    U32 m_retries; //! Number of retries to test
};

}  // end namespace Svc

#endif
