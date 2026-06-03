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

// ----------------------------------------------------------------------
// ValidateSeqCount.NewTableFull
//
// Exercises validateApidSeqCountIn with an APID that is not currently
// tracked while the APID table is full. The component must log
// ApidTableFull, must NOT log UnexpectedSequenceCount, and must return
// the received sequence count unchanged (the port is a passthrough).
// ----------------------------------------------------------------------

bool ApidManagerTester::ValidateSeqCount__NewTableFull__precondition() const {
    return this->shadow.shadow_isTableFull;
}

void ApidManagerTester::ValidateSeqCount__NewTableFull__action() {
    this->clearHistory();

    ComCfg::Apid::T apid = this->shadow.shadow_getRandomUntrackedApid();
    // Use any plausible 14-bit sequence count
    constexpr U16 receivedSeqCount = 42;
    U16 returned = this->invoke_to_validateApidSeqCountIn(0, apid, receivedSeqCount);

    ASSERT_EQ(returned, receivedSeqCount) << "validateApidSeqCountIn must return the received count unchanged";
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ApidTableFull_SIZE(1);
    ASSERT_EVENTS_ApidTableFull(0, static_cast<U16>(apid));
    ASSERT_EVENTS_UnexpectedSequenceCount_SIZE(0);
}

}  // namespace Ccsds

}  // namespace Svc
