// ======================================================================
// \title  ApidManagerTestState.cpp
// \author thomas-bc
// \brief  Shadow helper implementations for ApidManager test state
// ======================================================================

#include "Svc/Ccsds/ApidManager/test/ut/TestState/TestState.hpp"

#include <iterator>
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/ApidManager/ApidManager.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Shadow helpers
// ----------------------------------------------------------------------

U16 ApidManagerTestState::shadow_getAndIncrementSeqCount(ComCfg::Apid::T apid) {
    auto it = this->shadow_seqCounts.find(apid);
    if (it != this->shadow_seqCounts.end()) {
        // APID already tracked: return current count and advance to the next
        U16 current = it->second;
        it->second = static_cast<U16>((current + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
        return current;
    }
    // APID not yet tracked: register it, starting at count 0
    if (this->shadow_seqCounts.size() < ApidManager::MAX_TRACKED_APIDS) {
        this->shadow_seqCounts[apid] = static_cast<U16>(1);  // next expected is 1
        return 0;                                            // first count returned is 0
    }
    return ApidManager::SEQUENCE_COUNT_ERROR;
}

void ApidManagerTestState::shadow_validateApidSeqCount(ComCfg::Apid::T apid, U16 expectedSeqCount) {
    auto it = this->shadow_seqCounts.find(apid);
    if (it != this->shadow_seqCounts.end()) {
        it->second = static_cast<U16>((expectedSeqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
    }
}

ComCfg::Apid::T ApidManagerTestState::shadow_getRandomTrackedApid() const {
    FW_ASSERT(!this->shadow_seqCounts.empty());
    U32 idx = STest::Random::lowerUpper(0, static_cast<U32>(this->shadow_seqCounts.size()) - 1);
    return std::next(this->shadow_seqCounts.begin(), idx)->first;
}

ComCfg::Apid::T ApidManagerTestState::shadow_getRandomUntrackedApid() const {
    ComCfg::Apid::T apid;
    do {
        apid = static_cast<ComCfg::Apid::T>(STest::Random::lowerUpper(10, ComCfg::Apid::SPP_IDLE_PACKET));
    } while (this->shadow_seqCounts.find(apid) != this->shadow_seqCounts.end());
    return apid;
}

}  // namespace Ccsds

}  // namespace Svc
