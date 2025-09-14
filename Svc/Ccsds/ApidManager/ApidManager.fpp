module Svc {
module Ccsds {
    @ Maps output of ComQueue to CCSDS APIDs
    passive component ApidManager {

        @ Port to validate a given sequence count for a given APID
        guarded input port validateApidSeqCountIn: Ccsds.ApidSequenceCount

        @ Port to request a sequence count for a given APID
        guarded input port getApidSeqCountIn: Ccsds.ApidSequenceCount

        @ Deframing received an unexpected sequence count
        event UnexpectedSequenceCount(transmitted: U16, expected: U16) \
            severity warning low \
            format "Unexpected sequence count received. Packets may have been dropped. Transmitted: {} | Expected on board: {}"

        @ Received an unregistered APID
        event ApidTableFull(invalidApidValue: U16) \
            severity warning high \
            format "APID Table is full, cannot generate or check sequence counts for APID: {}"

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
