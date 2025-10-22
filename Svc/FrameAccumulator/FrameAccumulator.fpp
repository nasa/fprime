module Svc {
    @ Accumulates data into frames
    passive component FrameAccumulator {

        # ----------------------------------------------------------------------
        # FrameAccumulator interface
        # ----------------------------------------------------------------------
        import Svc.FrameAccumulator

        @ Port for deallocating buffers holding extracted frames
        output port bufferDeallocate: Fw.BufferSend

        @ Port for allocating buffer to hold extracted frame
        output port bufferAllocate: Fw.BufferGet

        @ An error occurred while deserializing a packet
        event NoBufferAvailable \
            severity warning high \
            format "Could not allocate a valid buffer to fit the detected frame"

        @ A frame was detected whose size exceeds the internal accumulation buffer
        @ capacity. No choice but to drop the frame.
        event FrameDetectionSizeError(size_out: FwSizeType) \
            severity warning high \
            format "Reported size_out={} exceeds accumulation buffer capacity"

        ###############################################################################
        # Standard AC Ports for Events 
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Ports for logging events
        import Fw.Event

    }
}
