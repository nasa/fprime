module Svc {
    @ Generic Data Product compression helper component
    passive component DpCompressProc {

        sync input port procRequest: Fw.BufferSend

        output port compressChunk: CompressChunk

        @ Chunk size to use when passing data to the compression backend
        param CHUNK_SIZE: FwSizeStoreType default 32*1024
        
        @ Number of bytes to use in a compression chunk
        param ENABLE: Fw.Enabled default Fw.Enabled.ENABLED

        event CompressionComplete(dp_id: FwDpIdType,
                                  initial_size: FwSizeType
                                  final_size: FwSizeType) \
            severity diagnostic id 0 \
            format "Compressed Data Product {} from {} to {} bytes"

        event DidNotCompress(dp_id: FwDpIdType,
                             data_size: FwSizeType) \
            severity activity low \
            format "Unable to reduce size of Data Product {}. Uncompressed size {}" \
            throttle 10

        @ Record ID used to mark compressed records in a data product
        product record CompressionRecord: U8 array

        @ Dummy container needed because a Record ID is defined
        product container Dummy default priority 0

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Enables event handling
        import Fw.Event

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

        @ Port for synchronously receiving Data Products
        @ These are not used by DpCompressProc but are needed
        @ because a product record is defined for DpCompressProc
        product get port dpGet
        product send port dpSend
    }
}
