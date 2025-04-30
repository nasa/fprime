module Svc {
module CCSDS {
module Types {

    # ------------------------------------------------
    # Frame header types
    # ------------------------------------------------ 
    # Note: FPP does not currently support bit fields, so these structs may contain fields
    # that are compoed of multiple bitfields. One should mask the individual FPP types 
    # with the approriate mask to 

    module SpacePacketMasks {
        constant PvnMask = 0xE000      @< 0b1110000000000000
        constant PktTypeMask = 0x1000  @< 0b0001000000000000
        constant SecHdrMask = 0x0800   @< 0b0000100000000000
        constant ApidMask = 0x07FF     @< 0b0000011111111111
    }

    @ Describes the frame header format for the SpacePacket communications protocol
    struct SpacePacketHeader {
        packetIdentification: U16,   @< 3 bits PVN | 1 bit Pkt type | 1 bit Sec Hdr | 11 bit APID
        packetSequenceControl: U16,  @< 2 bits Sequence flags | 14 bits packet seq count (or name)
        packetDataLength: U16        @< 16 bits length
    }

    @ Describes the frame header format for a Telecommand (TC) Transfer Frame header
    struct TCFrameHeader {
        flagsAndScId: U16,    @< 2 bits Frame V. | 1 bit bypass | 1 bit ctrl | 2 bit rsvd | 10 bits spacecraft ID
        vcIdAndLength: U16,   @< 6 bits Virtual Channel ID | 10 bits Frame Length
        frameSequenceNbr: U8  @< 8 bits Frame Sequence Number
    }



}
}
}