module Svc {

    enum CompressionAlgorithm : U8 {
        UNCOMPRESSED = 0
        ZLIB_DEFLATE = 1
    }

    dictionary struct CompressionMetadata {
        algorithm: CompressionAlgorithm
    }
}
