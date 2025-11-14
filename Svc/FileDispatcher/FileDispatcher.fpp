module Svc {
    @ Component to dispatch uplinked files to various services
    active component FileDispatcher {

        # @ disable dispatch of a file type
        async command ENABLE_DISPATCH(
                file_type: Svc.FileDispatcherCfg.FileDispatchPort @< the file type dispatch to enable/disable
                enable: Fw.Enabled
            ) \
            opcode 0

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################


        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        # @ File type dispatch enabled/disabled event
        event FileDispatchStateEvent(file_type: Svc.FileDispatcherCfg.FileDispatchPort, enabled: Fw.Enabled) severity activity low format "File dispatch {} state changed: to {}"
        event FileDispatched(file_name: string size 80, file_type: Svc.FileDispatcherCfg.FileDispatchPort) severity activity high format "File {} dispatched to {}"

        # @ Example port: receiving calls from the rate group
        # sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        ###############################################################################
        # Input ports                                                                 #
        ###############################################################################

        @ Port for receiving files to dispatch
        async input port fileRecv: Svc.FileAnnounce

        ###############################################################################
        # Output ports                                                                 #
        ###############################################################################

        @ Port for sending files to dispatch
        output port fileSend: [Svc.FileDispatcherCfg.FileDispatchPort.MAX_FILE_DISPATCH_PORTS] Svc.FileDispatch


    }
}