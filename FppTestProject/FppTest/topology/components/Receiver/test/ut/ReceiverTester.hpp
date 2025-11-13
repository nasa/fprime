// ======================================================================
// \title  ReceiverTester.hpp
// \author bocchino
// \brief  hpp file for Receiver component test harness implementation class
// ======================================================================

#ifndef FppTest_ReceiverTester_HPP
#define FppTest_ReceiverTester_HPP

#include "FppTest/topology/components/Receiver/Receiver.hpp"
#include "FppTest/topology/components/Receiver/ReceiverGTestBase.hpp"

namespace FppTest {

class ReceiverTester final : public ReceiverGTestBase {
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
    ReceiverTester();

    //! Destroy object BTester
    ~ReceiverTester();

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
    Receiver component;
};

}  // namespace FppTest

#endif
