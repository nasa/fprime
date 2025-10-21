module Svc {
    @ Accumulates data into frames
    passive component FrameAccumulator {

        # ----------------------------------------------------------------------
        # FrameAccumulator interface
        # ----------------------------------------------------------------------
        import Svc.FrameAccumulator

        @ Enables buffer allocation and deallocation
        import Svc.BufferAllocation

        @ An error occurred while deserializing a packet
        event NoBufferAvailable \
            severity warning high \
            format "Could not allocate a valid buffer to fit the detected frame"

        event FrameDetectionSizeError(size_out: FwSizeType) \
            severity warning high \
            format "Reported size_out={} exceeds available data"

        ###############################################################################
        # Standard AC Ports for Events 
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Ports for logging events
        import Fw.Event

    }
}
