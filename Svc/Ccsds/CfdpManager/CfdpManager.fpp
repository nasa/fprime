module Svc {
module Ccsds {
    @ F' implementation of the CFDP file transfer prototcol
    active component CfdpManager {

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ TODO
        async command TODO opcode 0

        event CfdpBuffersExuasted severity warning low \
            format "Unable to alocate a PDU buffer"

            
        ##############################################################################
        # Parameters
        ##############################################################################

        @ CFDP ID to denote the current node when sending PDUs
        param LocalEid: CfdpEntityId \
            default 42

        @ Maximum number of bytes to put into a file PDU
        @ TODO - Should this exist or should this always be CF_MAX_PDU_SIZE - header?
        param OutgoingFileChunkSize: U32 \
            default 480

        @ Parameter configuration for an array CFDP channels
        param ChannelConfig: CfdpChannelArrayParams \
            default [ \
                {
                    ack_limit = 4 \
                }, \
                {
                    ack_limit = 4 \
                } \
            ]


        ##############################################################################
        # Custom ports
        ##############################################################################

        @ Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
        async input port run1Hz: Svc.Sched

        @ Port for outputting PDU data
        output port dataOut: [CfdpManagerNumChannels] Fw.BufferSend

        @ Port for allocating buffers to hold PDU data
        output port bufferAllocate: [CfdpManagerNumChannels] Fw.BufferGet

        @ Port for deallocating buffers allocated for PDU data
        output port bufferDeallocate: [CfdpManagerNumChannels] Fw.BufferSend
        
        @ Buffer that was sent via the dataOut port and is now being retruned
        sync input port dataReturnIn: [CfdpManagerNumChannels] Svc.ComDataWithContext

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