// ======================================================================
// \title  ApidManager.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component implementation class
// ======================================================================

#include "Svc/Ccsds/ApidManager/ApidManager.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ApidManager ::ApidManager(const char* const compName) : ApidManagerComponentBase(compName) {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

U16 ApidManager ::validateApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::Apid& apid, U16 receivedSeqCount) {
    const U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (receivedSeqCount != expectedSequenceCount) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_LO_UnexpectedSequenceCount(receivedSeqCount, expectedSequenceCount);
        // Sync onboard count to what was received so counting can keep going. Insert cannot fail:
        // getAndIncrementSeqCount above inserted the APID already, we would have returned early
        // due to SEQUENCE_COUNT_ERROR
        const Fw::Success insertStatus = m_apidSequences.insert(apid, this->calculateNextSeqCount(receivedSeqCount));
        FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(apid));
    }
    return receivedSeqCount;
}

U16 ApidManager ::getApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::Apid& apid, U16 unused) {
    return this->getAndIncrementSeqCount(apid);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

U16 ApidManager ::getAndIncrementSeqCount(ComCfg::Apid::T apid) {
    U16 seqCount = 0;
    // Find current sequence count if APID is tracked; otherwise default to 0 (first count for a new APID)
    (void)m_apidSequences.find(apid, seqCount);
    // Store the next sequence count for this APID. If the APID is not yet tracked, this
    // also registers it.
    // Insert should never fail because the apid should be a valid enum value and
    // the table should be large enough to hold all the enum values
    const Fw::Success insertStatus = m_apidSequences.insert(apid, this->calculateNextSeqCount(seqCount));
    FW_ASSERT(insertStatus == Fw::Success::SUCCESS);
    return seqCount;
}

U16 ApidManager::calculateNextSeqCount(const U16 seqCount) const {
    return static_cast<U16>((seqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
}

}  // namespace Ccsds
}  // namespace Svc
