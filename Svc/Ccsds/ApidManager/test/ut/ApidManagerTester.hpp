// ======================================================================
// \title  ApidManagerTester.hpp
// \author thomas-bc
// \brief  hpp file for ApidManager component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_ApidManagerTester_HPP
#define Svc_Ccsds_ApidManagerTester_HPP

#include "Svc/Ccsds/ApidManager/ApidManager.hpp"
#include "Svc/Ccsds/ApidManager/ApidManagerGTestBase.hpp"
#include "Svc/Ccsds/ApidManager/test/ut/TestState/TestState.hpp"
#include "TestUtils/RuleBasedTesting.hpp"

namespace Svc {

namespace Ccsds {

class ApidManagerTester : public ApidManagerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ApidManagerTester
    ApidManagerTester();

    //! Destroy object ApidManagerTester
    ~ApidManagerTester();

  private:
    // ----------------------------------------------------------------------
    // Helper functions (auto-generated via UT_AUTO_HELPERS)
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ApidManager component;

  public:
    // ----------------------------------------------------------------------
    // Shadow state and rule method declarations
    // ----------------------------------------------------------------------

    //! Shadow model used by precondition/action rule methods
    TestState shadow;

    //! Rules for the getApidSeqCountIn port
    FW_TEST_STATE_DEF_RULE(GetSeqCount, Existing)
    FW_TEST_STATE_DEF_RULE(GetSeqCount, NewOk)
    FW_TEST_STATE_DEF_RULE(GetSeqCount, NewTableFull)

    //! Rules for the validateApidSeqCountIn port
    FW_TEST_STATE_DEF_RULE(ValidateSeqCount, Ok)
    FW_TEST_STATE_DEF_RULE(ValidateSeqCount, Failure)
};

}  // namespace Ccsds

}  // namespace Svc

#endif
