module Svc {

    port CompressChunk(
        ref buffer: Fw.Buffer,
        min_compression: FwSizeType,
        write_offset: FwSizeType,
    ) -> CompressionAlgorithm
}
