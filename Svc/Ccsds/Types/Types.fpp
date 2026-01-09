module Svc {
module Ccsds {

    @ Enum representing an error during framing/deframing in the CCSDS protocols
    enum FrameError: U8 {
        SP_INVALID_LENGTH = 0
        TC_INVALID_SCID = 1
        TC_INVALID_LENGTH = 2
        TC_INVALID_VCID = 3
        TC_INVALID_CRC = 4
    }

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
    @ Describes the frame header format for a Telecommand (TC) Transfer Frame
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
    @ Describes the frame header format for a Telemetry (TM) Transfer Frame
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

    # ------------------------------------------------
    # AOS
    # ------------------------------------------------
    @ Describes the frame header format for a Advanced Orbiting Systems (AOS) Space Data Link (SDL) Transfer Frame
    struct AOSHeader {
        globalVcId: U16,         @< 2 bit Frame Version | 8 least significant bits of spacecraft ID | 6 bit virtual channel ID
        frameCountAndSignaling: U32,  @< 24 bit virtual channel frame count
            @< 1 bit replay flag | 1 bit VC frame count cycle flag | 2 most significant bits of spacecraft ID | 4 bits VC frame count cycle
    }

    @ Offsets for serializing individual sub-fields in AOS headers
    module AOSHeaderSubfields {
        # globalVcId offsets
        constant frameVersionOffset = 14
        constant spacecraftIdLsbOffset = 6
        constant virtualChannelIdOffset = 0

        # signaling field offsets
        constant vcFrameCountOffset = 8
        constant replayFlagOffset = 7
        constant cycleCountFlagOffset = 6
        constant spacecraftIdMsbOffset = 4
    }

    @ Describes the header format for a Advanced Orbiting Systems (AOS) Space Data Link (SDL) multiplex protocol data unit (M_PDU)
    struct M_PDUHeader {
        firstHeaderPointer: U16     @< bytes to the header of the first new CCSDS Packet
    } default {
        firstHeaderPointer = 0xFFFF # Set first header pointer to all ones to mean no packet starts here (4.1.4.2.2.4)
    }

    @ Describes the frame trailer format for a Advanced Orbiting Systems (AOS) Space Data Link (SDL) Transfer Frame
    struct AOSTrailer {
        fecf: U16             @< 16 bit Frame Error Control Field (CRC16)
    }

    module PvnBitfield {
        constant SPP_MASK = 0x1 @< 1 << 0x0
        constant EPP_MASK = 0x8 @< 1 << 0x3
        constant VALID_MASK = SPP_MASK + EPP_MASK
    }

    @ Transfer Frame Version Numbers are 4 bits long
    @ CCSDS References add 1 to the binary value when counting versions in english (e.g. TM & TC are "version 1" but 0b00)
    @ Until the release of USLP, TFVN were 2 bits long
    @ With the addition of USLP two bits were added to the trailing end that serve as the 2 most significant bits
    @ (i.e. on the wire USLP sends 0b1100)
    @ See section 3.2.2.2 of USLP Overview (CCSDS 700.1-G-1),
    @ section 4.1.2.2.2 of AOS (CCSDS 732.0-B-5),
    @ section 4.3.2 of Space Data Link Protocol Summary (CCSDS 130.2-G-3),
    @ & table 3-1 in Overview of Space Comms Protocols (CCSDS 130.0-G-4)
    dictionary enum Tfvn : U8 {
        TM_TC         = 0x0   @< Telemetry and Telecommand SDLs
        AOS           = 0x1   @< Advanced Orbiting Systems SDL
        PROX_ONE      = 0x2   @< Proximity-1 SDL
        USLP          = 0x3   @< Unified Space Data Link Protocol
        INVALID_UNINITIALIZED         = 0x4  @< Anything equal or higher value is invalid and should not be used
    } default INVALID_UNINITIALIZED

}
}
