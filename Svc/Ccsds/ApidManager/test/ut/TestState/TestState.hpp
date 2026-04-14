// ======================================================================
// \title  TestState.hpp
// \author thomas-bc
// \brief  Shadow state model for ApidManager rule-based testing
//
// ApidManagerTestState contains only shadow-model data and state-only helpers.
// Rule preconditions/actions are implemented on ApidManagerTester.
//
// Shadow state
// ------------
// The shadow is a simple software model of the component's internal APID
// sequence-count table.  Precondition methods read from it to decide
// whether a rule may fire; action methods update it in lockstep with the
// component.  After every action the shadow and the component must agree.
// ======================================================================

#ifndef Svc_Ccsds_ApidManager_ApidManagerTestState_HPP
#define Svc_Ccsds_ApidManager_ApidManagerTestState_HPP

#include <map>

#include "Svc/Ccsds/ApidManager/ApidManager.hpp"

namespace Svc {

namespace Ccsds {

class ApidManagerTestState {
  public:
    // ----------------------------------------------------------------------
    // Component "shadow" test state
    //
    // These state variables mirror the component's internal state (APID map, etc).
    // ----------------------------------------------------------------------

    //! Expected next sequence count for every tracked APID.
    //! Mirrors the component's m_apidSequences map.
    std::map<ComCfg::Apid::T, U16> shadow_seqCounts;

    //! True once shadow_seqCounts has reached MAX_TRACKED_APIDS entries.
    bool shadow_isTableFull = false;

  public:
    // -----------------------------------------------------------------------------
    // Helpers maintaining the shadow state in line with component under test,
    // or simple test helpers querying the state
    // -----------------------------------------------------------------------------

    //! Return the current expected sequence count for apid and advance the
    //! shadow to the next value.
    U16 shadow_getAndIncrementSeqCount(ComCfg::Apid::T apid);

    //! Advance the shadow's expected count as if validateApidSeqCountIn was
    //! called with expectedSeqCount.  Mirrors the component's setNextSeqCount.
    void shadow_validateApidSeqCount(ComCfg::Apid::T apid, U16 expectedSeqCount);

    //! Return a uniformly random APID from the set currently tracked in the shadow.
    ComCfg::Apid::T shadow_getRandomTrackedApid() const;

    //! Return a random APID that is not currently tracked in the shadow.
    ComCfg::Apid::T shadow_getRandomUntrackedApid() const;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
