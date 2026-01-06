module Svc {
module Ccsds {

    @ F' implementation of the CFDP file transfer prototcol
    active component CfdpManager {

        ##############################################################################
        # Includes
        ##############################################################################

        include "Commands.fppi"
        include "Events.fppi"
        include "Parameters.fppi"

        ##############################################################################
        # Custom ports
        ##############################################################################

        @ Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
        async input port run1Hz: Svc.Sched

        @ Port for outputting PDU data
        output port dataOut: [CfdpManagerNumChannels] Fw.BufferSend

        @ Buffer that was sent via the dataOut port and is now being retruned
        sync input port dataReturnIn: [CfdpManagerNumChannels] Svc.ComDataWithContext

        @ Port for input PDU data
        async input port dataIn: [CfdpManagerNumChannels] Fw.BufferSend

        @ Return buffer that was recieved on the dataIn port
        output port dataInReturn: [CfdpManagerNumChannels] Fw.BufferSend

        @ Port for allocating buffers to hold PDU data
        output port bufferAllocate: [CfdpManagerNumChannels] Fw.BufferGet

        @ Port for deallocating buffers allocated for PDU data
        output port bufferDeallocate: [CfdpManagerNumChannels] Fw.BufferSend
        

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
} # end Ccsds
} # end Svc