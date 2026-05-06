// ======================================================================
// \title  GetSeqCount.cpp
// \author thomas-bc
// \brief  Rule implementations for the GetSeqCount rule group
//
// These rules exercise the getApidSeqCountIn port.
// ======================================================================

#include "Svc/Ccsds/ApidManager/test/ut/ApidManagerTester.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// GetSeqCount.Existing
// ----------------------------------------------------------------------

bool ApidManagerTester::GetSeqCount__Existing__precondition() const {
    return !this->shadow.shadow_seqCounts.empty();
}

void ApidManagerTester::GetSeqCount__Existing__action() {
    this->clearHistory();

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomTrackedApid();
    U16 returned = this->invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 expected = this->shadow.shadow_getAndIncrementSeqCount(apid);

    ASSERT_EQ(returned, expected) << "Sequence count mismatch for APID " << static_cast<U16>(apid);
    ASSERT_EVENTS_SIZE(0);
}

// ----------------------------------------------------------------------
// GetSeqCount.NewOk
// ----------------------------------------------------------------------

bool ApidManagerTester::GetSeqCount__NewOk__precondition() const {
    return !this->shadow.shadow_isTableFull;
}

void ApidManagerTester::GetSeqCount__NewOk__action() {
    this->clearHistory();

    // Use constexpr local to avoid ODR-use of the static constexpr member
    constexpr U16 maxTrackedApids = ApidManager::MAX_TRACKED_APIDS;
    if (this->shadow.shadow_seqCounts.size() >= maxTrackedApids) {
        // Table is now full; update shadow flag so the precondition flips
        this->shadow.shadow_isTableFull = true;
        return;
    }

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomUntrackedApid();
    U16 returned = this->invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 expected = this->shadow.shadow_getAndIncrementSeqCount(apid);

    ASSERT_EQ(returned, expected) << "First sequence count for new APID " << static_cast<U16>(apid) << " must be 0";
    ASSERT_EVENTS_SIZE(0);
}

// ----------------------------------------------------------------------
// GetSeqCount.NewTableFull
// ----------------------------------------------------------------------

bool ApidManagerTester::GetSeqCount__NewTableFull__precondition() const {
    return this->shadow.shadow_isTableFull;
}

void ApidManagerTester::GetSeqCount__NewTableFull__action() {
    this->clearHistory();

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomUntrackedApid();
    U16 returned = this->invoke_to_getApidSeqCountIn(0, apid, 0);

    // Use constexpr local to avoid ODR-use of the static constexpr member
    constexpr U16 errorValue = ApidManager::SEQUENCE_COUNT_ERROR;
    ASSERT_EQ(returned, errorValue) << "Expected SEQUENCE_COUNT_ERROR for untracked APID " << static_cast<U16>(apid)
                                    << " when the table is full";
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ApidTableFull_SIZE(1);
    ASSERT_EVENTS_ApidTableFull(0, static_cast<U16>(apid));
}

}  // namespace Ccsds

}  // namespace Svc
