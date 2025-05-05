module Svc {
module FprimeProtocol {

    type TokenType = U32

    @ Describes the frame header format for the F Prime communications protocol
    struct FrameHeader {
        startWord: TokenType,
        lengthField: TokenType,
    } default {
        startWord = 0xdeadbeef
    }

    @ Describes the frame trailer format for the F Prime communications protocol
    struct FrameTrailer {
        crcField: U32
    }

}
}
