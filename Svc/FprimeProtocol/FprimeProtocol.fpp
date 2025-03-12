module FprimeProtocol {

    @ Describes the frame header format for the F Prime communications protocol
    struct FrameHeader {
        startWord: U32,
        lengthField: U32,
    } default {
        startWord = 0xdeadbeef
    }

    @ Describes the frame trailer format for the F Prime communications protocol
    struct FrameTrailer {
        crcField: U32
    }

}
