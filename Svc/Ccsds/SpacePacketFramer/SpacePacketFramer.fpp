module Svc {
module Ccsds {
    @ Deframer for the CCSDS Space Packet protocol
    passive component SpacePacketFramer {

        import Framer

        @ Port to allocate a buffer for a space packet
        output port bufferAllocate: Fw.BufferGet

        @ Port to deallocate a buffer once space packet is sent
        output port bufferDeallocate: Fw.BufferSend

        @ Port to retrieve the current sequence count for a given APID
        output port getApidSeqCount: Ccsds.ApidSequenceCount

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

} # end Ccsds
} # end Svc
