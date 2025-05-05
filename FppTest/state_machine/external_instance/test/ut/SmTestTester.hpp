// ======================================================================
// \title  SmTest/test/ut/Tester.hpp
// \author watney
// \brief  hpp file for SmTest test harness implementation class
// ======================================================================

#ifndef FppTest_SmTest_Tester_HPP
#define FppTest_SmTest_Tester_HPP

#include "SmTestGTestBase.hpp"
#include "FppTest/state_machine/external_instance/SmTest.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

class SmTestTester : public SmTestGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static constexpr FwSizeType MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static constexpr FwSizeType TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static constexpr FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;
    // The component id base
    static constexpr FwDpIdType ID_BASE = 100;
    // The max string length for string data
    static constexpr FwSizeType MAX_STRING_LENGTH = 100;

    //! Construct object Tester
    //!
    SmTestTester();

    //! Destroy object Tester
    //!
    ~SmTestTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! schedIn OK
    void schedIn_OK();


  private:
    // ----------------------------------------------------------------------
    // Handlers for data product ports
    // ----------------------------------------------------------------------

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

    void dispatchAll();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------


    //! The component under test
    SmTest component;
};

}  // end namespace FppTest

#endif
