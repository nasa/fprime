// ======================================================================
// \title  ValidateSeqCount.cpp
// \author thomas-bc
// \brief  Rule implementations for the ValidateSeqCount rule group
//
// These rules exercise the validateApidSeqCountIn port.
// ======================================================================

#include "Svc/Ccsds/ApidManager/test/ut/ApidManagerTester.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// ValidateSeqCount.Ok
// ----------------------------------------------------------------------

bool ApidManagerTester::ValidateSeqCount__Ok__precondition() const {
    return !this->shadow.shadow_seqCounts.empty();
}

void ApidManagerTester::ValidateSeqCount__Ok__action() {
    this->clearHistory();

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomTrackedApid();
    U16 expected = this->shadow.shadow_seqCounts.at(apid);
    this->invoke_to_validateApidSeqCountIn(0, apid, expected);
    this->shadow.shadow_validateApidSeqCount(apid, expected);

    ASSERT_EVENTS_UnexpectedSequenceCount_SIZE(0);
}

// ----------------------------------------------------------------------
// ValidateSeqCount.Failure
// ----------------------------------------------------------------------

bool ApidManagerTester::ValidateSeqCount__Failure__precondition() const {
    return !this->shadow.shadow_seqCounts.empty();
}

void ApidManagerTester::ValidateSeqCount__Failure__action() {
    this->clearHistory();

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomTrackedApid();
    U16 correctCount = this->shadow.shadow_seqCounts.at(apid);

    // Increment by 1 (mod 14-bit counter width) to produce a provably wrong count
    U16 wrongCount = static_cast<U16>((correctCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));

    this->invoke_to_validateApidSeqCountIn(0, apid, wrongCount);
    this->shadow.shadow_validateApidSeqCount(apid, wrongCount);

    ASSERT_EVENTS_UnexpectedSequenceCount_SIZE(1);
    ASSERT_EVENTS_UnexpectedSequenceCount(0, wrongCount, correctCount);
}

}  // namespace Ccsds

}  // namespace Svc
