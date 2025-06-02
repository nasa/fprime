// ======================================================================
// \title  ApidManager.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component implementation class
// ======================================================================

#include "Svc/CCSDS/ApidManager/ApidManager.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ApidManager ::ApidManager(const char* const compName) : ApidManagerComponentBase(compName) {
    // Initialize APID sequence table with common APID values
    // This step is not strictly necessary, but it ensures that the most common APIDs are tracked
    this->m_apidSequences[0].apid = ComCfg::APID::FW_PACKET_COMMAND;
    this->m_apidSequences[1].apid = ComCfg::APID::FW_PACKET_TELEM;
    this->m_apidSequences[2].apid = ComCfg::APID::FW_PACKET_LOG;
    this->m_apidSequences[3].apid = ComCfg::APID::FW_PACKET_FILE;
    // this->m_apidSequences[4].apid = ComCfg::APID::FW_PACKET_PACKETIZED_TLM;
}

ApidManager ::~ApidManager() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

U16 ApidManager ::validateApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::APID& apid, U16 receivedSeqCount) {
    U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (receivedSeqCount != expectedSequenceCount) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_HI_UnexpectedSequenceCount(receivedSeqCount, expectedSequenceCount);
        // Synchronize onboard count with received number so that count can keep going
        this->setNextSeqCount(apid, receivedSeqCount + 1);
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
    U16 seqCount = SEQUENCE_COUNT_ERROR; // Default to error value
    // Search the APID in the sequence table
    for (U8 i = 0; i < MAX_TRACKED_APIDS; ++i) {
        if (this->m_apidSequences[i].apid == apid) {
            seqCount = this->m_apidSequences[i].sequenceCount;
            this->m_apidSequences[i].sequenceCount = static_cast<U16>((seqCount + 1) % (1 << 14)); // Increment for next call
            return seqCount; // Return the current sequence count
        }
    }
    // If not found, search for an uninitialized entry to track this APID
    for (U8 i = 0; i < MAX_TRACKED_APIDS; ++i) {
        if (this->m_apidSequences[i].apid == ComCfg::APID::INVALID_UNINITIALIZED) {
            this->m_apidSequences[i].apid = apid; // Initialize this entry with the new APID
            seqCount = this->m_apidSequences[i].sequenceCount; // Entries default to 0 unless otherwise specified
            this->m_apidSequences[i].sequenceCount = static_cast<U16>((seqCount + 1) % (1 << 14)); // Increment for next call
            return seqCount; // Return the initialized sequence count
        }
    }
    this->log_WARNING_HI_ApidTableFull(apid);
    return SEQUENCE_COUNT_ERROR;
}

void ApidManager::setNextSeqCount(ComCfg::APID::T apid, U16 seqCount) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (this->m_apidSequences[i].apid == apid) {
            this->m_apidSequences[i].sequenceCount = seqCount;
            return;
        }
    }
}

}  // namespace CCSDS
}  // namespace Svc
