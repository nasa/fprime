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
  public:
    enum class TestMode { UNSPECIFIED, SYNC, ASYNC };

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
    ComStubTester(TestMode mode = TestMode::UNSPECIFIED);  //!< Constructor with test mode

    //! Destroy object ComStubTester
    //!
    ~ComStubTester() = default;

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
    void test_retry_async();
    void test_retry_sync();

    //! Tests the retry -> reset -> retry again
    //!
    void test_retry_reset_async();
    void test_retry_reset_sync();

    //! Tests buffer is returned
    //!
    void test_buffer_return();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports (test harness)
    // ----------------------------------------------------------------------

    //! Handler for from_drvSendOut
    Drv::ByteStreamStatus from_drvSendOut_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer);

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports based on test mode. Unspecified connects all ports.
    //!
    void connectPortsWithTestMode(TestMode mode);

    void connectPorts();    //!< Connects all ports for the component under test
    void initComponents();  //!< Initializes the component under test

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    ComStub component;
    Drv::ByteStreamStatus m_sync_send_status;  //! Next return value for a sync send operation
    TestMode m_test_mode;                      //! Test mode
    FwIndexType m_retries;                     // Number of retries to test
    bool m_retry_fail = false;                 // Whether to keep failing after max retries are hit
};

}  // end namespace Svc

#endif
