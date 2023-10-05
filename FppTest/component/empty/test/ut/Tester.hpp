// ======================================================================
// \title  Empty/test/ut/Tester.hpp
// \author tiffany
// \brief  hpp file for Empty test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "EmptyGTestBase.hpp"
#include "FppTest/component/empty/Empty.hpp"

class Tester : public EmptyGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

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

    void test();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    Empty component;
};

#endif
