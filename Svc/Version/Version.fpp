module Svc {
    
    @ Tracks versions for project, framework and user defined versions etc
    
    enum VersionEnabled {
        DISABLED = 0
        ENABLED = 1
    }
    
    @ An enumeration for version status
    enum VersionStatus {
        OK = 0 @< Version was good
        FAILURE = 1 @< Failure to get version
    }

    @ Port for setting and getting Versions
    port Vers(
             version_id: VersionCfg.VersionEnum @< The entry to access
             ref version_string: string @< The value to be passed
             ref status: VersionStatus @< The command response argument
           )
    
    passive component Version {

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################
        
        @ Run port
        guarded input port run: [1] Svc.Sched
       
        @ Mutexed Port to get values
        guarded input port getVersion: Svc.Vers

        @ Mutexed Port to set values
        guarded input port setVersion: Svc.Vers
        
        @ A command to enable or disable Version telemetry
        guarded command ENABLE(
                                enable: VersionEnabled @< whether or not Version telemetry is enabled
                              ) \
        opcode 0 

        @ Report version as EVR
        guarded command VERSION \
            opcode 1

        @ Version of the git repository.
        event STARTUP_EVR(
                   proj_version: string size 40 @< project version
                   frm_version: string size 40 @< framework version
                 ) \
        severity activity low \
        id 2 \
        format "Project Version: [{}] Framework Version: [{}]"

        @ Version of the git repository.
        event FRAMEWORK_VERSION(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 0 \
        format "Framework Version: [{}]"

        @ Version of the git repository.
        event PROJECT_VERSION(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 1 \
        format "Project Version: [{}]"

        @ Software framework version
        telemetry FRAMEWORK_VERSION: string size 40 id 0

        @ Software project version
        telemetry PROJECT_VERSION: string size 40 id 1

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

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut

    }
}