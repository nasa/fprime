// ======================================================================
// \title  OsTimeTester.hpp
// \author kubiak
// \brief  hpp file for OsTime component test harness implementation class
// ======================================================================

#ifndef Svc_OsTimeTester_HPP
#define Svc_OsTimeTester_HPP

#include "Svc/OsTime/OsTime.hpp"
#include "Svc/OsTime/OsTimeGTestBase.hpp"

namespace Svc {

class OsTimeTester : public OsTimeGTestBase {
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

    //! Construct object OsTimeTester
    OsTimeTester();

    //! Destroy object OsTimeTester
    ~OsTimeTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Zero Epoch
    void zeroEpochTest();

    //! Non-zero Epoch
    void offsetEpochTest();

    //! No Epoch
    void noEpochTest();

    //! Update Epoch
    void updateEpochTest();

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
    OsTime component;
};

}  // namespace Svc

#endif
