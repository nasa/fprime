module Svc {
module FprimeProtocol {

    type TokenType = U32

    @ Describes the frame header format for the F Prime communications protocol
    @ Wire format: [startWord][lengthField][packetDescriptor][payload][crcField]
    @ lengthField counts payload bytes only (not including the packetDescriptor)
    struct FrameHeader {
        startWord: TokenType,
        lengthField: TokenType,
        packetDescriptor: FwPacketDescriptorType,
    } default {
        startWord = 0xdeadbeef
    }

    @ Describes the frame trailer format for the F Prime communications protocol
    struct FrameTrailer {
        crcField: U32
    }

}
}
