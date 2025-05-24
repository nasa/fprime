// ======================================================================
// \title  ApidManagerTester.hpp
// \author chammard
// \brief  hpp file for ApidManager component test harness implementation class
// ======================================================================

#ifndef Svc_CCSDS_ApidManagerTester_HPP
#define Svc_CCSDS_ApidManagerTester_HPP

#include "Svc/CCSDS/ApidManager/ApidManager.hpp"
#include "Svc/CCSDS/ApidManager/ApidManagerGTestBase.hpp"
#include "STest/Rule/Rule.hpp"

namespace Svc {

namespace CCSDS {

class ApidManagerTester final : public ApidManagerGTestBase {
    friend class ApidManagerTester;

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

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void toDo();

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
    ApidManager component;

    std::map<ComCfg::APID::T, U16> existingSeqCounts;  //!< Map to hold existing sequence counts for APIDs

    U16 shadow_getAndIncrementSeqCount(ComCfg::APID::T apid) {
        // This is a shadow function to simulate the getAndIncrementSeqCount behavior
        // without modifying the actual component state.
        auto found = this->existingSeqCounts.find(apid);
        if (found != this->existingSeqCounts.end()) {
            U16 seqCount = found->second;
            found->second = (seqCount + 1) % (1 << 14);  // Increment for next call
            return seqCount;  // Return the current sequence count
        }
        return ApidManager::SEQUENCE_COUNT_ERROR;  // Return error if APID not found
    }

    // ----------------------------------------------------------------------
    // Rules (Rule-based testing)
    // ----------------------------------------------------------------------
  public:
    struct GetExistingSeqCount : public STest::Rule<ApidManagerTester> {
        GetExistingSeqCount(): STest::Rule<ApidManagerTester>("GetExistingSeqCount") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };

};

}  // namespace CCSDS

}  // namespace Svc

#endif
