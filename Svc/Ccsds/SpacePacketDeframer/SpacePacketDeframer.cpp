// ======================================================================
// \title  SpacePacketDeframer.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component implementation class
// ======================================================================

#include "Svc/Ccsds/SpacePacketDeframer/SpacePacketDeframer.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpacePacketDeframer ::SpacePacketDeframer(const char* const compName) : SpacePacketDeframerComponentBase(compName) {}

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

    FW_ASSERT(data.getSize() > SpacePacketHeader::SERIALIZED_SIZE, static_cast<FwAssertArgType>(data.getSize()));

    SpacePacketHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Space Packet protocol defines the Data Length as number of bytes minus 1
    // so we need to add 1 to the length to get the actual data size
    U16 pkt_length = static_cast<U16>(header.get_packetDataLength() + 1);
    if (pkt_length > data.getSize() - SpacePacketHeader::SERIALIZED_SIZE) {
        FwSizeType maxDataAvailable = data.getSize() - SpacePacketHeader::SERIALIZED_SIZE;
        this->log_WARNING_HI_InvalidLength(pkt_length, maxDataAvailable);
        this->dataReturnOut_out(0, data, context);  // Drop the packet
        return;
    }

    U16 apidValue = header.get_packetIdentification() & SpacePacketSubfields::ApidMask;
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(apidValue);
    ComCfg::FrameContext contextCopy = context;
    contextCopy.set_apid(apid);

    // Validate with the ApidManager that the sequence count is correct
    U16 receivedSequenceCount = header.get_packetSequenceControl() & SpacePacketSubfields::SeqCountMask;
    (void)this->validateApidSeqCount_out(0, apid, receivedSequenceCount);
    contextCopy.set_sequenceCount(receivedSequenceCount);

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

}  // namespace Ccsds
}  // namespace Svc
