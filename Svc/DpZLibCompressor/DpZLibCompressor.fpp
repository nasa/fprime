module Svc {
    @ Data Product Compressor using ZLib DEFLATE
    passive component DpZLibCompressor {

        sync input port compressChunk: CompressChunk

        output port bufferCompressionGet: Fw.BufferGet
        output port bufferCompressionReturn: Fw.BufferSend

        output port bufferZLibGet: Fw.BufferGet
        output port bufferZLibReturn: Fw.BufferSend

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        @ Unable to allocate a compression buffer for ZLib
        event ZLibCompressionBadBuffer($size: FwSizeType) \
            severity warning low \
            format "Unable to allocate a buffer of size {} for compression" \
            throttle 5

        @ Unable to allocate an alloc buffer for ZLib
        event ZLibAllocBadBuffer($size: FwSizeType) \
            severity warning low \
            format "Unable to allocate a buffer of size {} for ZLib alloc" \
            throttle 5

        @ ZLib error during deflateInit
        event ZLibInitError(err: I32,
                            msg: string) \
            severity warning low \
            format "ZLib Error during initialization {} {}" \
            throttle 5

        @ ZLib error during deflate
        event ZLibDeflateError(err: I32,
                               msg: string) \
            severity warning low \
            format "ZLib Error during compression {} {}" \
            throttle 5

        @ Buffers are too large to process with zlib due to the
        @ type differences between FwSizeType and uInt
        event BufferTooBigForZLib(in_buffer_size: FwSizeType,
                                  comp_buffer_size: FwSizeType,
                                  max_size: FwSizeType) \
            severity warning low \
            format "Compression buffer too large to compress with zlib. {} or {} > {}" \
            throttle 5

        @ ZLib compression failed
        event ZLibNoCompression(buffer_size: FwSizeType,
                                min_compression: FwSizeType) \
            severity diagnostic \
            format "Unable to compress buffer of size {} to at most {} bytes with zlib"

        @ ZLib compression succeeded
        event ZLibCompression(buffer_size: FwSizeType,
                              compressed_size: FwSizeType) \
            severity diagnostic \
            format "Compressed buffer of size {} to {} bytes"

        @ ZLib compression memory usage
        event ZLibMemoryUsage(memory_used: FwSizeType,
                              total_memory: FwSizeType) \
            severity diagnostic \
            format "Compression used {} of {} available bytes"


        # @ Example port: receiving calls from the rate group
        # sync input port run: Svc.Sched

        @ Compression level for ZLib from 0-9
        @ 0: No compression. Fast
        @ 9: Best compression. Slow
        @ ZLib documentation suggests 6 is a good compromise between speed and compression level
        param CompressionLevel: I8 default 6

        @ ZLib needs to allocate various memory regions to operate
        @ ZLibBufferSize is the size of the Fw::Buffer to allocate
        @ for the calls to zalloc and zfree
        @ The default value here is sufficient for chunk sizes of 32 KB
        @ however it should be changed with larger chunk sizes
        param ZLibBufferSize: FwSizeType default 269000

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Enables command handling
        import Fw.Command

        @ Enables event handling
        import Fw.Event

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}
