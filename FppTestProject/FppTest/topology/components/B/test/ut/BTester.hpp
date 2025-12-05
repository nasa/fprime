// ======================================================================
// \title  BTester.hpp
// \author bocchino
// \brief  hpp file for B component test harness implementation class
// ======================================================================

#ifndef FppTest_BTester_HPP
#define FppTest_BTester_HPP

#include "FppTest/topology/components/B/B.hpp"
#include "FppTest/topology/components/B/BGTestBase.hpp"

namespace FppTest {

class BTester final : public BGTestBase {
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

    //! Construct object BTester
    BTester();

    //! Destroy object BTester
    ~BTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Receive data
    void receiveData();

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
    B component;
};

}  // namespace FppTest

#endif
