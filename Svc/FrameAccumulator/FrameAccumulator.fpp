module Svc {
    @ Accumulates data into frames
    passive component FrameAccumulator {

        # ----------------------------------------------------------------------
        # FrameAccumulator interface
        # ----------------------------------------------------------------------
        import FrameAccumulator

        @ Port for deallocating buffers holding extracted frames
        output port bufferDeallocate: Fw.BufferSend

        @ Port for allocating buffer to hold extracted frame
        output port bufferAllocate: Fw.BufferGet

        @ An error occurred while deserializing a packet
        event NoBufferAvailable \
            severity warning high \
            format "Could not allocate a valid buffer to fit the detected frame"


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
