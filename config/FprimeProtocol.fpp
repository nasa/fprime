module FprimeProtocol {

    @ Describes the frame header format for the F Prime communications protocol
    struct FrameHeader {
        start_word: U32,
        length: U32,
    } default {
        start_word = 0xdeadbeef
    }

    @ Describes the frame footer format for the F Prime communications protocol
    struct FrameFooter {
        crc: U32
    }

}
