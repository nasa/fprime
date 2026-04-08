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
    U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (receivedSeqCount != expectedSequenceCount && receivedSeqCount != SEQUENCE_COUNT_ERROR) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_LO_UnexpectedSequenceCount(receivedSeqCount, expectedSequenceCount);
        // Synchronize onboard count with received number so that count can keep going
        this->setNextSeqCount(apid, this->calculateNextSeqCount(receivedSeqCount));
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
    // Find sequence count if exists, otherwise use 0
    (void)m_apidSequences.find(apid, seqCount);
    // Increment sequence count for next call
    U16 updatedSeqCount = this->calculateNextSeqCount(seqCount);

    Fw::Success insertStatus = m_apidSequences.insert(apid, updatedSeqCount);
    if (insertStatus == Fw::Success::SUCCESS) {
        return seqCount;  // Return the current sequence count
    }

    this->log_WARNING_HI_ApidTableFull(apid);
    return SEQUENCE_COUNT_ERROR;
}

void ApidManager::setNextSeqCount(ComCfg::Apid::T apid, U16 seqCount) {
    Fw::Success insertStatus = m_apidSequences.insert(apid, seqCount);
    FW_ASSERT(insertStatus == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(apid));
}

U16 ApidManager::calculateNextSeqCount(const U16 seqCount) const {
    return static_cast<U16>((seqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
}

}  // namespace Ccsds
}  // namespace Svc
