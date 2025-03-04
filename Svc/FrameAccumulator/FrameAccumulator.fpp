module Svc {
    @ Accumulates data into frames
    passive component FrameAccumulator {

        @ Receives raw data from a ByteStreamDriver, ComStub, or other buffer producing component
        guarded input port dataIn: Drv.ByteStreamRecv

        @ Port for deallocating buffers received on dataIn.
        output port bufferDeallocate: Fw.BufferSend

        @ Port for allocating buffer to hold extracted frame
        output port bufferAllocate: Fw.BufferGet

        @ Port for sending an extracted frame out
        output port frameOut: Fw.DataWithContext

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
