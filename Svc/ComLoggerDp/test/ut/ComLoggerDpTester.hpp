// ======================================================================
// \title  ComLoggerDpTester.hpp
// \author tcanham
// \brief  hpp file for ComLoggerDp component test harness implementation class
// ======================================================================

#ifndef Svc_ComLoggerDpTester_HPP
#define Svc_ComLoggerDpTester_HPP

#include "Svc/ComLoggerDp/ComLoggerDp.hpp"
#include "Svc/ComLoggerDp/ComLoggerDpGTestBase.hpp"

namespace Svc {

class ComLoggerDpTester final : public ComLoggerDpGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ComLoggerDpTester
    ComLoggerDpTester();

    //! Destroy object ComLoggerDpTester
    ~ComLoggerDpTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test logging Com buffers when enabled
    void testComLogging();

    //! Test StartComDp command
    void testStartComDp();

    //! Test StopComDp command
    void testStopComDp();

    //! Test UpdatePriority command
    void testUpdatePriority();

    //! Test ping port
    void testPing();

    //! Test container overflow behavior
    void testContainerFill();

    //! Test allocation failure
    void testAllocationFailure();

    //! Test port validation failure
    void testPortValidationFailure();

    //! Test telemetry
    void testTelemetry();

    //! Test priority is preserved when starting logging
    void testPriorityPreserved();

    //! Test starting recording via port
    void testStartRecordingPort();

    //! Test stopping recording via port
    void testStopRecordingPort();

    //! Test clearing counters
    void testClearCounters();

    //! Test buffer overflow - buffer too large for container
    void testBufferOverflow();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Override productGet_handler to control allocation behavior
    Fw::Success::T productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ComLoggerDp component;

    //! Buffer for testing
    U8 m_buffer[10000];

    //! Flag to simulate allocation failure
    bool m_allocationFailure{false};
};

}  // namespace Svc

#endif
