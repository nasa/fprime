module Svc {
    @ Framer implementation for the F Prime protocol
    passive component FprimeFramer {

        import Framer

        # ----------------------------------------------------------------------
        # Allocation of buffers
        # ----------------------------------------------------------------------

        @ Port for allocating buffers to hold framed data
        output port bufferAllocate: Fw.BufferGet

        @ Port for deallocating buffers allocated for framed data
        output port bufferDeallocate: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ No buffer was available to hold the frame, which was dropped
        event NoBufferAvailable \
            severity warning high \
            format "Failed to allocate a frame buffer: frame dropped" \
            throttle 5

        # ----------------------------------------------------------------------
        # Standard AC Ports
        # ----------------------------------------------------------------------
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

    }
}
