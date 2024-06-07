module Svc {
    @ Routes packets deframed by the Deframer to the rest of the system
    passive component Router {

        @ Receiving Fw::Buffer from Deframer
        guarded input port bufferIn: Fw.BufferSend

        @ Port for sending file packets as Fw::Buffer (ownership passed to receiver)
        output port fileOut: Fw.BufferSend

        @ Port for sending command packets as Fw::ComBuffers
        output port commandOut: Fw.Com

        @ Port for deallocating buffers
        output port bufferDeallocate: Fw.BufferSend

        @ Port for receiving command responses from a command dispatcher.
        @ Invoking this port does nothing. The port exists to allow the matching
        @ connection in the topology.
        sync input port cmdResponseIn: Fw.CmdResponse

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

    }
}
