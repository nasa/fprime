// ======================================================================
// \title  SpacePacketDeframer.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component implementation class
// ======================================================================

#include "Svc/CCSDS/SpacePacketDeframer/SpacePacketDeframer.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"

namespace Svc {

namespace CCSDS {

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

    FW_ASSERT(data.getSize() >= Svc::CCSDS::Types::SpacePacketHeader::SERIALIZED_SIZE, static_cast<FwAssertArgType>(data.getSize()));

    CCSDS::Types::SpacePacketHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 apid = header.getpacketIdentification() & CCSDS::Types::SpacePacketMasks::ApidMask;
    ComCfg::FrameContext contextCopy = context;
    contextCopy.setapid(apid);

    // TODO: Add checks and events in case of failure
    U16 pkt_length = header.getpacketDataLength();

    // Set data buffer to be of the encapsulated data: HEADER (6 bytes) | PACKET DATA
    data.setData(data.getData() + Svc::CCSDS::Types::SpacePacketHeader::SERIALIZED_SIZE);
    data.setSize(pkt_length);

    this->dataOut_out(0, data, contextCopy);
}

void SpacePacketDeframer ::dataReturnIn_handler(FwIndexType portNum,
                                                Fw::Buffer& data,
                                                const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, data, context);
}


}  // namespace CCSDS
}  // namespace Svc
