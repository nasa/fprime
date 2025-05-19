module Svc {
module CCSDS {
    @ Maps output of ComQueue to CCSDS APIDs
    passive component ApidMapper {

        include "../../Interfaces/FramerInterface.fppi"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

    }
}
}