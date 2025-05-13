module Svc {
module CCSDS {
    @ Deframer for the TC Space Data Link Protocl (CCSDS Standard)
    passive component TCDeframer {

        include "../../Interfaces/DeframerInterface.fppi"

        @ Used for event reporting
        enum HeaderField {
            SpacecraftId,
            FrameLength,
            VcId
        }

        @ Invalid Data Field
        event InvalidHeaderField(field: HeaderField, expected: U32, received: U32) \
            severity warning high \
            format "Invalid Header Field Received. Field {} | Expected: {} | Received: {}"

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