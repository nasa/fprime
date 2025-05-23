module Svc {
module CCSDS {
    @ Maps output of ComQueue to CCSDS APIDs
    passive component ApidManager {

        include "../../Interfaces/FramerInterface.fppi"

        guarded input port validateApidSeqCountIn: CCSDS.ApidSequenceCount

        @ Deframing received an unexpected sequence count
        event UnexpectedSequenceCount(transmitted: U16, expected: U16) \
            severity warning high \
            format "Unexpected sequence count received. Packets may have been lost. Transmitted: {} | Expected on board: {}"

        @ Received an unregistered APID
        event UntrackedApid(invalidApidValue: U16) \
            severity activity low \
            format "Received APID not registered with the deframer. Not checking sequence count. APID={}"


        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut
    }
}
}
