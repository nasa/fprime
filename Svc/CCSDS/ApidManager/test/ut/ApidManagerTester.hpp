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
#include "STest/Random/Random.hpp"

namespace Svc {

namespace CCSDS {

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

  public:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ApidManager component;

    std::map<ComCfg::APID::T, U16> shadow_seqCounts;  //!< Map to hold existing sequence counts for APIDs
    bool shadow_isTableFull = false;

    U16 shadow_getAndIncrementSeqCount(ComCfg::APID::T apid) {
        // This is a shadow function to simulate the getAndIncrementSeqCount behavior
        // without modifying the actual component state.
        auto found = this->shadow_seqCounts.find(apid);
        if (found != this->shadow_seqCounts.end()) {
            U16 seqCount = found->second;
            found->second = (seqCount + 1) % (1 << 14);  // Increment for next call
            return seqCount;  // Return the current sequence count
        }
        // If APID not found, initialize a new entry
        if (this->shadow_seqCounts.size() < this->component.MAX_TRACKED_APIDS) {
            U16 seqCount = 0;
            this->shadow_seqCounts[apid] = seqCount + 1; // increment for next call
            return seqCount;  // Return the initialized sequence count
        }
        return this->component.SEQUENCE_COUNT_ERROR;  // Return error if APID not found
    }

    void shadow_validateApidSeqCount(ComCfg::APID::T apid, U16 expectedSeqCount) {
      // This simply updates the shadow state to the next expected sequence count
      auto found = this->shadow_seqCounts.find(apid);
      if (found != this->shadow_seqCounts.end()) {
          found->second = (expectedSeqCount + 1) % (1 << 14);
      }
    }

    ComCfg::APID::T getRandomTrackedApid() {
      // Select a random APID from the sequence counts map
      U32 mapSize = static_cast<U32>(this->shadow_seqCounts.size());
      U32 randomIndex = STest::Random::lowerUpper(0, mapSize - 1);
      ComCfg::APID apid = std::next(this->shadow_seqCounts.begin(), randomIndex)->first;
      return apid;
    }

    ComCfg::APID::T getRandomUntrackedApid() {
      // Select a random APID that is not currently tracked
      ComCfg::APID::T apid;
      do {
        apid = static_cast<ComCfg::APID::T>(STest::Random::lowerUpper(10, ComCfg::APID::SPP_IDLE_PACKET));
      } while (this->shadow_seqCounts.find(apid) != this->shadow_seqCounts.end());
      return apid;
    }

  public:
    struct GetExistingSeqCount : public STest::Rule<ApidManagerTester> {
        GetExistingSeqCount(): STest::Rule<ApidManagerTester>("GetExistingSeqCount") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };
    
    struct GetNewSeqCountOk : public STest::Rule<ApidManagerTester> {
        GetNewSeqCountOk(): STest::Rule<ApidManagerTester>("GetNewSeqCountOk") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };
    
    struct GetNewSeqCountTableFull : public STest::Rule<ApidManagerTester> {
        GetNewSeqCountTableFull(): STest::Rule<ApidManagerTester>("GetNewSeqCountTableFull") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };

    struct ValidateSeqCountOk : public STest::Rule<ApidManagerTester> {
        ValidateSeqCountOk(): STest::Rule<ApidManagerTester>("ValidateSeqCountOk") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };

    struct ValidateSeqCountFailure : public STest::Rule<ApidManagerTester> {
        ValidateSeqCountFailure(): STest::Rule<ApidManagerTester>("ValidateSeqCountFailure") {};
        bool precondition(const ApidManagerTester &state);
        void action(ApidManagerTester &state);
    };
  };

}  // namespace CCSDS

}  // namespace Svc

#endif
