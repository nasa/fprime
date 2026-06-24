// ======================================================================
// \title  LedTester.hpp
// \author ortega
// \brief  hpp file for Led component test harness implementation class
// ======================================================================

#ifndef Components_LedTester_HPP
#define Components_LedTester_HPP

#include "LedBlinker/Components/Led/Led.hpp"
#include "LedBlinker/Components/Led/LedGTestBase.hpp"

namespace LedBlinker {

class LedTester : public LedGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const U32 MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object LedTester
    LedTester();

    //! Destroy object LedTester
    ~LedTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testBlinking();
    void testBlinkInterval();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_gpioSet
    //!
    Drv::GpioStatus from_gpioSet_handler(const FwIndexType portNum, /*!< The port number*/
                                         const Fw::Logic& state);

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
    Led component;
};

}  // namespace LedBlinker

#endif
