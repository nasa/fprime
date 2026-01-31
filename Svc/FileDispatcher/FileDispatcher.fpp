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

        # @ File type dispatch enabled/disabled event
        event FileDispatchState(file_type: Svc.FileDispatcherCfg.FileDispatchPort, enabled: Fw.Enabled) severity activity high format "File dispatch {} state changed: to {}"

        # @ File dispatched event
        event FileDispatched(file_name: string size FileNameStringSize, file_type: Svc.FileDispatcherCfg.FileDispatchPort) severity activity high format "File {} dispatched to {}"

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

        ###############################################################################
        # Input ports                                                                 #
        ###############################################################################

        @ Port for receiving files to dispatch
        async input port fileAnnounceRecv: Svc.FileAnnounce

        @ Ping in
        async input port pingIn: Svc.Ping

        ###############################################################################
        # Output ports                                                                 #
        ###############################################################################

        @ Port for sending files to dispatch
        output port fileDispatch: [Svc.FileDispatcherCfg.FileDispatchPort.MAX_FILE_DISPATCH_PORTS] Svc.FileDispatch

        @ Ping out
        output port pingOut: Svc.Ping

    }
}