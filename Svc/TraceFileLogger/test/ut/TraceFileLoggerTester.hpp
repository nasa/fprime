// ======================================================================
// \title  TraceFileLoggerTester.hpp
// \author sreddy
// \brief  hpp file for TraceFileLogger component test harness implementation class
// ======================================================================

#ifndef Svc_TraceFileLoggerTester_HPP
#define Svc_TraceFileLoggerTester_HPP

#include "Svc/TraceFileLogger/TraceFileLogger.hpp"
#include "Svc/TraceFileLogger/TraceFileLoggerGTestBase.hpp"
#include <cstdio>
namespace Svc {

class TraceFileLoggerTester : public TraceFileLoggerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwQueueSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    //File size to store Trace data
    static const U32 TEST_TRACE_FILE_SIZE_MAX = 27200;

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
    //Read trace file into a buffer
    void read_file();
    //Test trace filtering commands
    void test_filter_trace_id();
    void test_filter_trace_type();
    void test_trace_enable();
    void test_code_coverage();

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
    std::vector<U8> storage_buffer;
    U16 file_size;
};

}  // namespace Svc

#endif
