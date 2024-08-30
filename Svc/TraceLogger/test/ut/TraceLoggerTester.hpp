// ======================================================================
// \title  TraceLoggerTester.hpp
// \author sreddy
// \brief  hpp file for TraceLogger component test harness implementation class
// ======================================================================

#ifndef Svc_TraceLoggerTester_HPP
#define Svc_TraceLoggerTester_HPP

#include "Svc/TraceLogger/TraceLogger.hpp"
#include "Svc/TraceLogger/TraceLoggerGTestBase.hpp"

namespace Svc {

class TraceLoggerTester : public TraceLoggerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwQueueSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object TraceLoggerTester
    TraceLoggerTester();

    //! Destroy object TraceLoggerTester
    ~TraceLoggerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void test_startup();
    void test_file();

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
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    TraceLogger component;
};

}  // namespace Svc

#endif
