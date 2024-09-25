// ======================================================================
// \title  SpaceDataLinkDeframer.cpp
// \author chammard
// \brief  cpp file for SpaceDataLinkDeframer component implementation class
// ======================================================================

#include "SpaceDataLinkDeframer.hpp"
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpaceDataLinkDeframer ::SpaceDataLinkDeframer(const char* const compName)
    : SpaceDataLinkDeframerComponentBase(compName) {}

SpaceDataLinkDeframer ::~SpaceDataLinkDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void SpaceDataLinkDeframer ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {

    // CCSDS TC Format:
    // 5 octets - TC Primary Header
    // Up to 1019 octets - Data Field (including optional 2 octets frame error control field)

    // CCSDS TC Primary Header:
    // 2b - 00  - TF Version Number
    // 1b - 0/1 - Bypass Flag            (0 = FARM checks enabled, 1 = FARM checks bypassed)
    // 1b - 0/1 - Control Command Flag   (0 = Type-D data, 1 = Type-C data)
    // 2b - 00  - Reserved Spare         (set to 00)
    // 10b- XX  - Spacecraft ID
    // 6b - XX  - Virtual Channel ID
    // 10b- XX  - Frame Length
    // 8b - XX  - Frame Sequence Number

    FW_ASSERT(data.getSize() >= SPACE_PACKET_HEADER_SIZE + SPACE_PACKET_TRAILER_SIZE, data.getSize());

    U8 byte = data.getData()[0];

    U8 version_number = (byte & 0xC0) >> 6;
    U8 bypass = (byte & 0x20) >> 5;
    U8 ctrl_cmd = (byte & 0x10) >> 4;
    U16 sc_id = ((byte & 0x03) << 8);

    byte = data.getData()[1];
    sc_id |= byte;

    byte = data.getData()[2];
    U8 vch_id = (byte & 0xFC) >> 2;


    U16 frame_length = (byte & 0x03) << 8;
    byte = data.getData()[3];
    frame_length |= byte;


    byte = data.getData()[4];
    U8 frame_seq_nbr = byte;

    // Set data buffer to be of the encapsulated data: HEADER (5 bytes) | DATA | CHECKSUM (2 bytes)
    data.setData(data.getData() + SPACE_PACKET_HEADER_SIZE);
    data.setSize(frame_length); // 5 bytes for Header, 2 bytes for Frame Error Control Field

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
