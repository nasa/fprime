// ======================================================================
// \title  BlockDriverTester.hpp
// \author root
// \brief  hpp file for BlockDriver component test harness implementation class
// ======================================================================

#ifndef Ref_BlockDriverTester_HPP
#define Ref_BlockDriverTester_HPP

#include "Ref/BlockDriver/BlockDriver.hpp"
#include "Ref/BlockDriver/BlockDriverGTestBase.hpp"

namespace Ref {

class BlockDriverTester final : public BlockDriverGTestBase {
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

    //! Construct object BlockDriverTester
    BlockDriverTester();

    //! Destroy object BlockDriverTester
    ~BlockDriverTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test data loop back port
    void testDataLoopBack();

    //! Test Ping port
    void testPing();

    //! Test ISR cycle increment
    void testCycleIncrement();

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
    BlockDriver component;
};

}  // namespace Ref

#endif
