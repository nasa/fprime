// ======================================================================
// \title  SpacePacketDeframer.cpp
// \author chammard
// \brief  cpp file for SpacePacketDeframer component implementation class
// ======================================================================

#include "SpacePacketDeframer.hpp"
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpacePacketDeframer ::SpacePacketDeframer(const char* const compName)
    : SpacePacketDeframerComponentBase(compName) {}

SpacePacketDeframer ::~SpacePacketDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void SpacePacketDeframer ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {
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

    FW_ASSERT(data.getSize() >= 6, data.getSize());

    // Read first byte from ring
    U8 byte = data.getData()[0];
    // Unpack Packet Version Number (PVN)
    U8 pvn = (byte & 0xE0) >> 5;
    // Unpack Packet Type (PT)
    U8 pt = (byte & 0x10) >> 4;
    // Unpack Secondary Header Flag (SHF)
    U8 shf = (byte & 0x08) >> 3;
    
    // Unpack APID
    U16 apid = ((byte & 0x07) << 8);
    byte = data.getData()[1];
    apid |= byte;

    // Sequence flag
    byte = data.getData()[2];
    U8 seq_flag = (byte & 0xC0) >> 6;

    // Sequence count
    U16 seq_count = (byte & 0x3F) << 8;
    byte = data.getData()[3];
    seq_count |= byte;

    // Packet length
    byte = data.getData()[4];
    U16 pkt_length = byte << 8;
    byte = data.getData()[5];
    pkt_length |= byte;


    // Set data buffer to be of the encapsulated data: HEADER (6 bytes) | PACKET DATA
    data.setData(data.getData() + SPACE_PACKET_HEADER_SIZE);
    data.setSize(pkt_length); // TODO: look into whether a spacePacket can span multiple frames

    this->deframedOut_out(0, data, context);

}

}  // namespace Svc
