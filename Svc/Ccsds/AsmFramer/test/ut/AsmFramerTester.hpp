// ======================================================================
// \title  AsmFramerTester.hpp
// \author devin
// \brief  hpp file for AsmFramer component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_AsmFramerTester_HPP
#define Svc_Ccsds_AsmFramerTester_HPP

#include "Svc/Ccsds/AsmFramer/AsmFramer.hpp"
#include "Svc/Ccsds/AsmFramer/AsmFramerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class AsmFramerTester final : public AsmFramerGTestBase {
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

    //! Construct object AsmFramerTester
    AsmFramerTester();

    //! Destroy object AsmFramerTester
    ~AsmFramerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testComStatusPassthrough();
    void testNominalFraming();
    void testConfiguredAsm();
    void testConfigureInvalid();
    void testInputBufferTooLarge();
    void testDataReturn();
    void testBufferOwnershipState();

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
    AsmFramer component;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
