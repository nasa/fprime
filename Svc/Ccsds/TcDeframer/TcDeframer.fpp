module Svc {
module Ccsds {
    @ Deframer for the TC Space Data Link Protocol (CCSDS Standard)
    passive component TcDeframer {

        import Deframer

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