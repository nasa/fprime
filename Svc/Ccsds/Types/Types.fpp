# ------------------------------------------------
# CCSDS protocols header and trailer types
# ------------------------------------------------ 

module Svc {
module Ccsds {

    # ------------------------------------------------
    # SpacePacket
    # ------------------------------------------------ 
    @ Describes the frame header format for the SpacePacket communications protocol
    struct SpacePacketHeader {
        packetIdentification: U16,   @< 3 bits PVN | 1 bit Pkt type | 1 bit Sec Hdr | 11 bit APID
        packetSequenceControl: U16,  @< 2 bits Sequence flags | 14 bits packet seq count (or name)
        packetDataLength: U16        @< 16 bits length
    }
    @ Masks and Offsets for deserializing individual sub-fields in SpacePacket headers
    module SpacePacketSubfields {
        # packetIdentification sub-fields     |--- 16 bits ---|
        constant PvnMask        = 0xE000  @< 0b1110000000000000
        constant PktTypeMask    = 0x1000  @< 0b0001000000000000
        constant SecHdrMask     = 0x0800  @< 0b0000100000000000
        constant ApidMask       = 0x07FF  @< 0b0000011111111111
        constant PvnOffset      = 13 
        constant PktTypeOffset  = 12 
        constant SecHdrOffset   = 11 
        # packetSequenceControl sub-fields
        constant SeqFlagsMask   = 0xC000  @< 0b1100000000000000
        constant SeqCountMask   = 0x3FFF  @< 0b0011111111111111
        constant SeqFlagsOffset = 14 
        # Widths
        constant ApidWidth      = 11
        constant SeqCountWidth  = 14
    }

    # ------------------------------------------------
    # TC
    # ------------------------------------------------ 
    @ Describes the frame header format for a Telecommand (TC) Transfer Frame header
    struct TCHeader {
        flagsAndScId: U16,    @< 2 bits Frame V. | 1 bit bypass | 1 bit ctrl | 2 bit reserved | 10 bits spacecraft ID
        vcIdAndLength: U16,   @< 6 bits Virtual Channel ID | 10 bits Frame Length
        frameSequenceNum: U8  @< 8 bits Frame Sequence Number
    }
    @ Describes the frame trailer format for a Telecommand (TC) Transfer Frame
    struct TCTrailer {
        fecf: U16             @< 16 bit Frame Error Control Field (CRC16)
    }
    @ Masks and Offsets for deserializing individual sub-fields in TC headers
    module TCSubfields {
        # flagsAndScId sub-fields
        constant FrameVersionMask = 0xC000  @< 0b1100000000000000
        constant BypassFlagMask   = 0x2000  @< 0b0010000000000000
        constant ControlFlagMask  = 0x1000  @< 0b0001000000000000
        constant ReservedMask     = 0x0C00  @< 0b0000110000000000
        constant SpacecraftIdMask = 0x03FF  @< 0b0000001111111111
        constant BypassFlagOffset = 13
        # vcIdAndLength sub-fields
        constant VcIdMask         = 0xFC00  @< 0b1111110000000000
        constant FrameLengthMask  = 0x03FF  @< 0b0000001111111111
        constant VcIdOffset       = 10
    }

    # ------------------------------------------------
    # TM
    # ------------------------------------------------ 
    @ Describes the frame header format for a Telemetry (TM) Transfer Frame header
    struct TMHeader {
        globalVcId: U16,         @< 2 bit Frame Version | 10 bit spacecraft ID | 3 bit virtual channel ID | 1 bit OCF flag
        masterFrameCount: U8,    @< 8 bit Master Channel Frame Count
        virtualFrameCount: U8,   @< 8 bit Virtual Channel Frame Count
        dataFieldStatus: U16     @< 1 bit 2nd Header | 1 bit sync | 1 bit pkt order | 2 bit seg len | 11 bit header ptr
    }
    @ Describes the frame trailer format for a Telemetry (TM) Transfer Frame
    struct TMTrailer {
        fecf: U16             @< 16 bit Frame Error Control Field (CRC16)
    }
    @ Offsets for serializing individual sub-fields in TM headers
    module TMSubfields {
        constant frameVersionOffset = 14
        constant spacecraftIdOffset = 4
        constant virtualChannelIdOffset = 1
        constant segLengthOffset = 11
    }

}
}
