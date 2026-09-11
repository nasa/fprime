module Svc {
module Ccsds {
    @ Deframer for the TC Space Data Link Protocol (CCSDS Standard)
    @ Optionally reassembles packets segmented across multiple TC frames (CCSDS 232.0-B-4 Section 4.1.3.3)
    passive component TcDeframer {

        import Deframer

        @ Port to notify of a deframing error
        output port errorNotify: Ccsds.ErrorNotify

        @ Buffer allocation and deallocation for packets that span across multiple TC frames
        import Svc.BufferAllocation

        @ Invalid packet received that will be dropped
        event InvalidPacket() \
            severity warning low \
            format "Invalid packet received refusing to deframe"

        @ Deframing received an invalid SCID
        event InvalidSpacecraftId(transmitted: U16, configured: U16) \
            severity warning low \ 
            format "Invalid Spacecraft ID Received. Received: {} | Deframer configured with: {}"

        @ Deframing received an invalid frame length
        event InvalidFrameLength(transmitted: U16, actual: FwSizeType) \
            severity warning high \
            format "Not enough data received. Header length specified: {} | Received data length: {}"

        @ Deframing received an invalid VCID
        event InvalidVcId(transmitted: U16, configured: U16) \
            severity activity low \
            format "Invalid Virtual Channel ID Received. Header token specified: {} | Deframer configured with: {}"

        @ Deframing received an invalid checksum
        event InvalidCrc(transmitted: U16, computed: U16) \
            severity warning high \
            format "Invalid checksum received. Trailer specified: {} | Computed on board: {}"

        @ Segment header MAP ID does not match the configured MAP ID; segment dropped
        event InvalidMapId(transmitted: U8, configured: U8) \
            severity warning low \
            format "Invalid MAP ID received. Received: {} | Deframer configured with: {}"

        @ A continuing or last segment was received with no spanning packet in progress; segment dropped
        event UnexpectedSegment(sequenceFlags: TCSegmentSequenceFlags) \
            severity warning high \
            format "Unexpected TC segment ({}) received with no spanning packet in progress"

        @ A spanning packet was abandoned before its last segment was received
        event SpanningPacketAbandoned(bytesReceived: FwSizeType) \
            severity warning high \
            format "Spanning TC packet abandoned after receiving {} bytes"

        @ Spanning packet buffer allocation failed; packet dropped
        event SpanningPacketAllocFailed(requestedSize: FwSizeType) \
            severity warning high \
            format "Spanning TC packet allocation of {} bytes failed; packet dropped"

        @ A spanning packet exceeded the configured maximum size; packet dropped
        event SpanningPacketOverflow(bytesReceived: FwSizeType, segmentSize: FwSizeType, maxSize: FwSizeType) \
            severity warning high \
            format "Spanning TC packet of {} bytes plus {} byte segment exceeds maximum of {} bytes; packet dropped"

        @ A reassembled spanning packet was dropped because too many are outstanding downstream
        event SpanningPacketInFlightLimit(maxInFlight: FwSizeType) \
            severity warning high \
            format "Spanning TC packet dropped: {} reassembled packets already outstanding downstream"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}
}