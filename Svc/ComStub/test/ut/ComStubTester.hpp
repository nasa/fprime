// ======================================================================
// \title  ComStub/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for ComStub test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "ComStubGTestBase.hpp"
#include "Svc/ComStub/ComStub.hpp"

namespace Svc {

class ComStubTester : public ComStubGTestBase {

  // Maximum size of histories storing events, telemetry, and port outputs
  static const FwSizeType MAX_HISTORY_SIZE = 30;

  // Instance ID supplied to the component instance under test
  static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object ComStubTester
    //!
    ComStubTester();

    //! Destroy object ComStubTester
    //!
    ~ComStubTester();

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

    //! Tests the retry -> reset -> retry again
    //!
    void test_retry_reset();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_comDataOut
    //!
    void from_comDataOut_handler(const FwIndexType portNum, //!< The port number
                                 Fw::Buffer& recvBuffer);

    //! Handler for from_comStatusOut
    //!
    void from_comStatusOut_handler(const FwIndexType portNum, //!< The port number
                                Fw::Success& condition         //!< Status of communication state
    );

    //! Handler for from_drvDataOut
    //!
    void from_drvDataOut_handler(const FwIndexType portNum, //!< The port number
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
    ComStub component;
    Drv::ByteStreamStatus m_send_mode;  //! Send mode
    U32 m_retries; //! Number of retries to test
};

}  // end namespace Svc

#endif
