module Svc {
    @ The FprimePacketizer transform s known input data types into F Prime packets
    passive component FprimePacketizer {

        include "../Interfaces/PacketizerInterface.fppi"

        # ----------------------------------------------------------------------
        # Handling of of ready signals
        # ----------------------------------------------------------------------

        @ Port receiving the general status from the downstream component
        @ indicating it is ready or not-ready for more input
        sync input port comStatusIn: Fw.SuccessCondition

        @ Port receiving indicating the status of framer for receiving more data
        output port comStatusOut: Fw.SuccessCondition

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

    }
}
