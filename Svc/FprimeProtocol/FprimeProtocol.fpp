module Svc {
module FprimeProtocol {

    type TokenType = U32

    @ Describes the frame header format for the F Prime communications protocol
    struct FrameHeader {
        startWord: TokenType,
        lengthField: TokenType,
        descriptor: FwPacketDescriptorType,
    } default {
        startWord = 0xdeadbeef
        lengthField = 0
        descriptor = ComCfg.Apid.FW_PACKET_UNKNOWN
    }  # TODO: update docs on the Fprime protocol

    @ Describes the frame trailer format for the F Prime communications protocol
    struct FrameTrailer {
        crcField: U32
    }

}
}
