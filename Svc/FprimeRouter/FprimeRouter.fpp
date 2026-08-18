module Svc {
    @ Routes packets deframed by the Deframer to the rest of the system
    passive component FprimeRouter {

        # ----------------------------------------------------------------------
        # Router interface
        # ----------------------------------------------------------------------
        import Router

        @ Port for forwarding non-recognized packet types
        @ Ownership of the buffer is passed to the receiver. The receiver must return
        @ the buffer via fileBufferReturnIn when done.
        output port unknownDataOut: Svc.ComDataWithContext

        @ An error occurred while serializing a com buffer
        event SerializationError(
                status: U32 @< The status of the operation
            ) \
            severity warning high \
            format "Serializing com buffer failed with status {}"

        @ An error occurred while deserializing a packet
        event DeserializationError(
                status: U32 @< The status of the operation
            ) \
            severity warning high \
            format "Deserializing packet type failed with status {}"

        @ The buffer-to-context table was full when a buffer was handed off on
        @ fileOut. The buffer is still forwarded, but its context cannot be
        @ restored on return and will be returned empty.
        event FileOutContextTableFull() \
            severity warning high \
            format "Buffer-to-context table full on fileOut; context will be lost for this buffer"

        @ The buffer-to-context table was full when a buffer was handed off on
        @ unknownDataOut. The buffer is still forwarded, but its context cannot be
        @ restored on return and will be returned empty.
        event UnknownDataOutContextTableFull() \
            severity warning high \
            format "Buffer-to-context table full on unknownDataOut; context will be lost for this buffer"

        @ A buffer returned on fileBufferReturnIn was not found in the
        @ buffer-to-context table. The buffer is still returned, but with an
        @ empty context.
        event BufferContextNotFound() \
            severity warning high \
            format "Returned buffer not found in context table; returning with empty context"

        ###############################################################################
        # Standard AC Ports for Events 
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

    }
}
