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
    // Initialize the APID sequence table with APID values that need to be counted (order does not matter)
    this->m_apidSequences[0].apid = ComCfg::APID::FW_PACKET_LOG;
    this->m_apidSequences[1].apid = ComCfg::APID::FW_PACKET_TELEM;
    this->m_apidSequences[2].apid = ComCfg::APID::FW_PACKET_FILE;
    this->m_apidSequences[3].apid = ComCfg::APID::FW_PACKET_PACKETIZED_TLM;
    this->m_apidSequences[4].apid = ComCfg::APID::FW_PACKET_UNKNOWN;
    this->m_apidSequences[5].apid = ComCfg::APID::FW_PACKET_COMMAND;
    this->m_apidSequences[6].apid = ComCfg::APID::SPP_FILE_DOWNLINK;
    this->m_apidSequences[7].apid = ComCfg::APID::FW_PACKET_DP;
    this->m_apidSequences[8].apid = ComCfg::APID::MY_USER_APID_EXAMPLE;
}

ApidManager ::~ApidManager() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ApidManager ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

Fw::Success ApidManager ::validateApidSeqCountIn_handler(FwIndexType portNum, const ComCfg::APID& apid, U16 receivedSeqCount) {
    // ComCfg::APID::T apidValue = static_cast<ComCfg::APID::T>(apid);
    Fw::Success status = Fw::Success::SUCCESS;
    U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (expectedSequenceCount == SEQUENCE_COUNT_ERROR) {
        // This APID is not being tracked
        this->log_ACTIVITY_LO_UntrackedApid(apid);
        status = Fw::Success::FAILURE;
    } else if (receivedSeqCount != expectedSequenceCount) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_HI_UnexpectedSequenceCount(receivedSeqCount, expectedSequenceCount);
        // Synchronize onboard count with received number so that count can keep going
        this->setNextSeqCount(apid, receivedSeqCount + 1);
        status = Fw::Success::FAILURE;
    }
    return status;
}

void ApidManager ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    ComCfg::FrameContext contextCopy = context;

    // Deserialize Packet Descriptor to map to an APID
    auto deserializer = data.getDeserializer();
    FwPacketDescriptorType descriptorValue = 0;
    Fw::SerializeStatus status = deserializer.deserialize(descriptorValue);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    ComCfg::APID::T apid;
    if (descriptorValue == ComCfg::APID::FW_PACKET_FILE) {
        // If descriptor is a file packet, set APID to the file downlink APID
        // This is because we don't want to use FW_PACKET_FILE as the APID so
        // the APID isn't shared between uplink and downlink
        apid = ComCfg::APID::SPP_FILE_DOWNLINK;
    } else {
        // Else, map one-to-one: Descriptor value is APID value
        apid = static_cast<ComCfg::APID::T>(descriptorValue);
    }

    contextCopy.setapid(static_cast<ComCfg::APID::T>(apid));
    contextCopy.setsequenceCount(this->getAndIncrementSeqCount(static_cast<ComCfg::APID::T>(apid)));
    // TODO: assert if sequence count is error since that means it is a coding error: APID was not registered ??

    // Forward the buffer and context to the output port
    this->dataOut_out(0, data, contextCopy);
}

void ApidManager ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(portNum, data, context);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

U16 ApidManager ::getAndIncrementSeqCount(ComCfg::APID::T apid) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; ++i) {
        if (m_apidSequences[i].apid == apid) {
            U16 seq = m_apidSequences[i].sequenceCount;
            m_apidSequences[i].sequenceCount = (seq + 1) % (1 << 14); // Wrap around at 14 bits
            return seq;
        }
    }
    // NOTE: log untracked APID on downlink as well? that's kind of a coding error?
    return SEQUENCE_COUNT_ERROR;
}

void ApidManager::setNextSeqCount(ComCfg::APID::T apid, U16 seqCount) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (m_apidSequences[i].apid == apid) {
            m_apidSequences[i].sequenceCount = seqCount;
            return;
        }
    }
}

}  // namespace CCSDS

}  // namespace Svc
