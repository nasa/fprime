// ======================================================================
// \title  ApidManagerTester.hpp
// \author thomas-bc
// \brief  hpp file for ApidManager component test harness implementation class
// ======================================================================

#ifndef Svc_Ccsds_ApidManagerTester_HPP
#define Svc_Ccsds_ApidManagerTester_HPP

#include "STest/Random/Random.hpp"
#include "STest/Rule/Rule.hpp"
#include "Svc/Ccsds/ApidManager/ApidManager.hpp"
#include "Svc/Ccsds/ApidManager/ApidManagerGTestBase.hpp"

namespace Svc {

namespace Ccsds {

class ApidManagerTester : public ApidManagerGTestBase {
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

    //! Construct object ApidManagerTester
    ApidManagerTester();

    //! Destroy object ApidManagerTester
    ~ApidManagerTester();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
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

    // Shadow test state
    std::map<ComCfg::APID::T, U16> shadow_seqCounts;  //!< Map to hold expected sequence counts for APIDs
    bool shadow_isTableFull = false;

    // ----------------------------------------------------------------------
    // Helpers for tracking the shadow test state
    // ----------------------------------------------------------------------

    U16 shadow_getAndIncrementSeqCount(ComCfg::APID::T apid);

    void shadow_validateApidSeqCount(ComCfg::APID::T apid, U16 expectedSeqCount);

    ComCfg::APID::T shadow_getRandomTrackedApid();

    ComCfg::APID::T shadow_getRandomUntrackedApid();

    // ----------------------------------------------------------------------
    // Tests: Rule Based Testing
    // ----------------------------------------------------------------------

  public:
    struct GetExistingSeqCount : public STest::Rule<ApidManagerTester> {
        GetExistingSeqCount() : STest::Rule<ApidManagerTester>("GetExistingSeqCount") {};
        bool precondition(const ApidManagerTester& state);
        void action(ApidManagerTester& state);
    };

    struct GetNewSeqCountOk : public STest::Rule<ApidManagerTester> {
        GetNewSeqCountOk() : STest::Rule<ApidManagerTester>("GetNewSeqCountOk") {};
        bool precondition(const ApidManagerTester& state);
        void action(ApidManagerTester& state);
    };

    struct GetNewSeqCountTableFull : public STest::Rule<ApidManagerTester> {
        GetNewSeqCountTableFull() : STest::Rule<ApidManagerTester>("GetNewSeqCountTableFull") {};
        bool precondition(const ApidManagerTester& state);
        void action(ApidManagerTester& state);
    };

    struct ValidateSeqCountOk : public STest::Rule<ApidManagerTester> {
        ValidateSeqCountOk() : STest::Rule<ApidManagerTester>("ValidateSeqCountOk") {};
        bool precondition(const ApidManagerTester& state);
        void action(ApidManagerTester& state);
    };

    struct ValidateSeqCountFailure : public STest::Rule<ApidManagerTester> {
        ValidateSeqCountFailure() : STest::Rule<ApidManagerTester>("ValidateSeqCountFailure") {};
        bool precondition(const ApidManagerTester& state);
        void action(ApidManagerTester& state);
    };
};

}  // namespace Ccsds

}  // namespace Svc

#endif
