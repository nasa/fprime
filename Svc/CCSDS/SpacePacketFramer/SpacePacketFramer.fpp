module Svc {
module CCSDS {
    @ Deframer for the CCSDS Space Packet protocol
    passive component SpacePacketFramer {

        include "../../Interfaces/FramerInterface.fppi"

        output port bufferAllocate: Fw.BufferGet
        output port bufferDeallocate: Fw.BufferSend

        output port getApidSeqCount: CCSDS.ApidSequenceCount

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

        @ Port to set the value of a parameter
        param set port prmSetOut

    }

} # end CCSDS
} # end Svc
