// ======================================================================
// \title  TraceFileLoggerTester.hpp
// \author sreddy
// \brief  hpp file for TraceFileLogger component test harness implementation class
// ======================================================================

#ifndef Svc_TraceFileLoggerTester_HPP
#define Svc_TraceFileLoggerTester_HPP

#include "Svc/TraceFileLogger/TraceFileLogger.hpp"
#include "Svc/TraceFileLogger/TraceFileLoggerGTestBase.hpp"

namespace Svc {

class TraceFileLoggerTester : public TraceFileLoggerGTestBase {
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

    //File size to store Trace data
    static const U32 TEST_TRACE_FILE_SIZE_MAX = 2720000;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object TraceFileLoggerTester
    TraceFileLoggerTester();

    //! Destroy object TraceFileLoggerTester
    ~TraceFileLoggerTester();

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
    TraceFileLogger component;
};

}  // namespace Svc

#endif
