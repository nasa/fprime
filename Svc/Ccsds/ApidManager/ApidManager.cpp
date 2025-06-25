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

U16 ApidManager ::validateApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::APID& apid, U16 receivedSeqCount) {
    U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (receivedSeqCount != expectedSequenceCount && receivedSeqCount != SEQUENCE_COUNT_ERROR) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_LO_UnexpectedSequenceCount(receivedSeqCount, expectedSequenceCount);
        // Synchronize onboard count with received number so that count can keep going
        this->setNextSeqCount(apid, static_cast<U16>(receivedSeqCount + 1));
    }
    return receivedSeqCount;
}

U16 ApidManager ::getApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::APID& apid, U16 unused) {
    return this->getAndIncrementSeqCount(apid);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

U16 ApidManager ::getAndIncrementSeqCount(ComCfg::APID::T apid) {
    U16 seqCount = SEQUENCE_COUNT_ERROR;  // Default to error value
    // Search the APID in the sequence table
    for (U16 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (this->m_apidSequences[i].apid == apid) {
            seqCount = this->m_apidSequences[i].sequenceCount;
            // Increment entry for next call
            this->m_apidSequences[i].sequenceCount =
                static_cast<U16>((seqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
            return seqCount;  // Return the current sequence count
        }
    }
    // If not found, search for an uninitialized entry to track this APID
    for (U16 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (this->m_apidSequences[i].apid == ComCfg::APID::INVALID_UNINITIALIZED) {
            this->m_apidSequences[i].apid = apid;               // Initialize this entry with the new APID
            seqCount = this->m_apidSequences[i].sequenceCount;  // Entries default to 0 unless otherwise specified
            // Increment entry for next call
            this->m_apidSequences[i].sequenceCount =
                static_cast<U16>((seqCount + 1) % (1 << SpacePacketSubfields::SeqCountWidth));
            return seqCount;  // Return the initialized sequence count
        }
    }
    this->log_WARNING_HI_ApidTableFull(apid);
    return SEQUENCE_COUNT_ERROR;
}

void ApidManager::setNextSeqCount(ComCfg::APID::T apid, U16 seqCount) {
    for (U16 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (this->m_apidSequences[i].apid == apid) {
            this->m_apidSequences[i].sequenceCount = seqCount;
            return;
        }
    }
    // This code should not be reachable with the if statement in validateApidSeqCountIn_handler
    FW_ASSERT(false, static_cast<FwAssertArgType>(apid));
}

}  // namespace Ccsds
}  // namespace Svc
