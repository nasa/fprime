module Svc {
module FprimeProtocol {

    type TokenType = U32

    @ Describes the frame header format for the F Prime communications protocol
    @ Wire format: [startWord][lengthField][packetDescriptor][payload][crcField]
    @ lengthField counts the packetDescriptor and payload bytes (preserves the legacy on-wire length semantics)
    struct FrameHeader {
        startWord: TokenType,
        lengthField: TokenType,
        packetDescriptor: FwPacketDescriptorType,
    } default {
        startWord = 0xdeadbeef
        lengthField = 0
        packetDescriptor = ComCfg.Apid.FW_PACKET_UNKNOWN
    }  # TODO: update docs on the Fprime protocol

    @ Describes the frame trailer format for the F Prime communications protocol
    struct FrameTrailer {
        crcField: U32
    }

}
}
