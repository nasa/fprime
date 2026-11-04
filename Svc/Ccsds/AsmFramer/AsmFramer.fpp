module Svc {
module Ccsds {
    @ Framer prepending the CCSDS Attached Sync Marker (ASM) to transfer frames,
    @ producing Sync-Marked Transfer Frames (CCSDS 131.0-B-5 Section 9)
    passive component AsmFramer {

        import Framer

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
