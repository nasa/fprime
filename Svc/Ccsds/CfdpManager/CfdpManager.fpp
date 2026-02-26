module Svc {
module Ccsds {
module Cfdp {

    @ F' implementation of the CFDP file transfer protocol
    active component CfdpManager {

        ##############################################################################
        # Includes
        ##############################################################################

        include "Commands.fppi"
        include "Events.fppi"
        include "Parameters.fppi"
        include "Telemetry.fppi"

        ##############################################################################
        # Custom ports
        ##############################################################################

        # Admin ports
        @ Run port which must be invoked at 1 Hz in order to satisfy CFDP timer logic
        async input port run1Hz: Svc.Sched

        @ Ping in port
        async input port pingIn: Svc.Ping

        @ Ping out port
        output port pingOut: Svc.Ping

        # Downlink ports
        @ Port for outputting PDU data
        output port dataOut: [NumChannels] Fw.BufferSend

        @ Buffer that was sent via the dataOut port and is now being returned
        async input port dataReturnIn: [NumChannels] Svc.ComDataWithContext

        @ Port for allocating buffers to hold PDU data
        output port bufferAllocate: [NumChannels] Fw.BufferGet

        @ Port for deallocating buffers allocated for PDU data
        output port bufferDeallocate: [NumChannels] Fw.BufferSend

        # Uplink ports
        @ Port for input PDU data
        async input port dataIn: [NumChannels] Fw.BufferSend

        @ Return buffer that was received on the dataIn port
        output port dataInReturn: [NumChannels] Fw.BufferSend

        # DP ports
        @ File send request port
        guarded input port fileIn: Svc.SendFileRequest

        @ File send complete notification port
        output port fileDoneOut: Svc.SendFileComplete


        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Enables command handling
        import Fw.Command

        @ Enables event handling
        import Fw.Event

        @ Enables telemetry channels handling
        import Fw.Channel

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
} # end Cfdp
} # end Ccsds
} # end Svc