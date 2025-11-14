// ======================================================================
// \title  ATester.hpp
// \author bocchino
// \brief  hpp file for A component test harness implementation class
// ======================================================================

#ifndef FppTest_ATester_HPP
#define FppTest_ATester_HPP

#include "FppTest/topology/components/Sender/Sender.hpp"
#include "FppTest/topology/components/Sender/SenderGTestBase.hpp"

namespace FppTest {

class SenderTester final : public SenderGTestBase {
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

    //! Construct object ATester
    ATester();

    //! Destroy object ATester
    ~ATester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Send data
    void sendData();

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
    A component;
};

}  // namespace FppTest

#endif
