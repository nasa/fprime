module Fprime {

    @ TODO
    struct FrameHeader {
        start_word: U32,
        length: U32,
    } default {
        start_word = 0xdeadbeef
    }

    struct FrameFooter {
        crc: U32
    }

}