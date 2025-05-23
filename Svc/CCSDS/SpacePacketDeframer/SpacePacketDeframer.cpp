// ======================================================================
// \title  SpacePacketDeframer.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component implementation class
// ======================================================================

#include "Svc/CCSDS/SpacePacketDeframer/SpacePacketDeframer.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpacePacketDeframer ::SpacePacketDeframer(const char* const compName) : SpacePacketDeframerComponentBase(compName) {
    // Initialize the APID sequence table with APID values that need to be counted (order does not matter)
    this->m_apidSequences[0].apid = ComCfg::APID::FW_PACKET_COMMAND;
    this->m_apidSequences[2].apid = ComCfg::APID::FW_PACKET_FILE;
    this->m_apidSequences[3].apid = ComCfg::APID::FW_PACKET_PACKETIZED_TLM;
    this->m_apidSequences[4].apid = ComCfg::APID::FW_PACKET_UNKNOWN;
}

SpacePacketDeframer ::~SpacePacketDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void SpacePacketDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // ################################
    // CCSDS SpacePacket Format:
    // 6 octets - Primary Header
    // 0-65536 octets - Data Field (with optional secondary header)

    // CCSDS SpacePacket Primary Header:
    //  3b - 000 - (PVN) Packet Version Number
    //  1b - 0/1 - (PT) Packet Type
    //  1b - 0/1 - (SHF) Secondary Header Flag
    // 11b - n/a - (APID) Application Process ID
    //  2b - 00  - Sequence Flag
    // 14b - n/a - Sequence Count
    // 16b - n/a - Packet Data Length
    // ################################

    FW_ASSERT(data.getSize() >= SpacePacketHeader::SERIALIZED_SIZE, static_cast<FwAssertArgType>(data.getSize()));

    SpacePacketHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 apidValue = header.getpacketIdentification() & SpacePacketMasks::ApidMask;
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(apidValue);
    ComCfg::FrameContext contextCopy = context;
    contextCopy.setapid(apid);

    U16 pkt_length = header.getpacketDataLength();
    if (pkt_length > data.getSize()) {
        this->log_WARNING_HI_InvalidLength(pkt_length, data.getSize());
        this->dataReturnOut_out(0, data, context);  // Drop the packet
        return;
    }
    U16 receivedSequenceCount = header.getpacketSequenceControl() & SpacePacketMasks::SeqCountMask;
    U16 expectedSequenceCount = this->getAndIncrementSeqCount(apid);
    if (expectedSequenceCount == SEQUENCE_COUNT_ERROR) {
        // This APID is not being tracked
        this->log_ACTIVITY_LO_UntrackedApid(apidValue);
    } else if (receivedSequenceCount != expectedSequenceCount) {
        // Likely a packet was dropped or out of order
        this->log_WARNING_HI_UnexpectedSequenceCount(receivedSequenceCount, expectedSequenceCount);
        // Synchronize onboard count with received number so that count can keep going
        this->setNextSeqCount(apid, receivedSequenceCount + 1);
    }
    contextCopy.setsequenceCount(receivedSequenceCount);

    // Set data buffer to be of the encapsulated data: HEADER (6 bytes) | PACKET DATA
    data.setData(data.getData() + SpacePacketHeader::SERIALIZED_SIZE);
    data.setSize(pkt_length);

    this->dataOut_out(0, data, contextCopy);
}

void SpacePacketDeframer ::dataReturnIn_handler(FwIndexType portNum,
                                                Fw::Buffer& data,
                                                const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, data, context);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

U16 SpacePacketDeframer::getAndIncrementSeqCount(ComCfg::APID::T apid) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; ++i) {
        if (m_apidSequences[i].apid == apid) {
            U16 seq = m_apidSequences[i].sequenceCount;
            m_apidSequences[i].sequenceCount = (seq + 1) % (1 << 14);  // Wrap around at 14 bits
            printf("APID: %d, SeqCount: %d\n", apid, seq);
            return seq;
        }
    }
    return SEQUENCE_COUNT_ERROR; // error value - this value is never returned with wraparound
}

void SpacePacketDeframer::setNextSeqCount(ComCfg::APID::T apid, U16 seqCount) {
    for (U8 i = 0; i < MAX_TRACKED_APIDS; i++) {
        if (m_apidSequences[i].apid == apid) {
            m_apidSequences[i].sequenceCount = seqCount;
            return;
        }
    }
}

}  // namespace CCSDS
}  // namespace Svc
