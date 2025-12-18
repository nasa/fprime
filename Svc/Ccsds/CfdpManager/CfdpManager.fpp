module Svc {
module Ccsds {
    @ F' implementation of the CFDP file transfer prototcol
    active component CfdpManager {

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ TODO
        async command TODO opcode 0

        ##############################################################################
        # Custom ports
        ##############################################################################

        @ Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
        async input port run1Hz: Svc.Sched

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