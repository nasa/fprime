// ======================================================================
// \title  ChronoTimeTester.hpp
// \author mstarch
// \brief  hpp file for ChronoTime component test harness implementation class
// ======================================================================

#ifndef Svc_ChronoTimeTester_HPP
#define Svc_ChronoTimeTester_HPP

#include "Svc/ChronoTime/ChronoTime.hpp"
#include "Svc/ChronoTime/ChronoTimeGTestBase.hpp"

namespace Svc {

class ChronoTimeTester : public ChronoTimeGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ChronoTimeTester
    ChronoTimeTester();

    //! Destroy object ChronoTimeTester
    ~ChronoTimeTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void test_basic_time();

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
    ChronoTime component;
};

}  // namespace Svc

#endif
