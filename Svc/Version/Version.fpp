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

    @ An enumeration for Version Type
    enum VersionType {
        PROJECT = 0 @< project version
        FRAMEWORK = 1
        LIBRARY = 2
        CUSTOM = 3
        ALL = 4
    }

   
    @ Port for setting and getting Versions
    port Vers(
             version_id: VersionCfg.VersionEnum @< The entry to access
             ref version_string: string @< The value to be passed
             ref status: VersionStatus @< The command response argument
             )
    
    @Data Structure for custom version Tlm
    struct CusVerDb {
        ver_enum: VersionCfg.VersionEnum
        ver_val: string size 80  
        ver_status : VersionStatus
    }

    passive component Version {

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################
        
        @ Run port
        #guarded input port run: [1] Svc.Sched
       
        @ Mutexed Port to get values
        guarded input port getVersion: Svc.Vers

        @ Mutexed Port to set values
        guarded input port setVersion: Svc.Vers
        
        @ A command to enable or disable EVR verbosity and Telemetry
        guarded command ENABLE(
                                enable: VersionEnabled @< whether or not Version telemetry is enabled
                              ) \
        opcode 0 

        @ Report version as EVR
        guarded command VERSION(
                                version_type: VersionType @<which version type EVR is requested
                               ) \
        opcode 1

        @ Version of the git repository.
        #event STARTUP_EVR(
        #           proj_version: string size 40 @< project version
        #           frm_version: string size 40 @< framework version
        #         ) \
        #severity activity low \
        #id 2 \
        #format "Project Version: [{}] Framework Version: [{}]"

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

        @ Version of the git repository.
        event LIBRARY_VERSIONS(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 2 \
        format "Libary Versions: [{}]"
        
        @ Version of the git repository.
        event CUSTOM_VERSIONS(
                   version_enum : VersionCfg.VersionEnum @< The enum to access
                   version_val : string size 40 @< version 
                 ) \
        severity activity low \
        id 3 \
        format "Custom Versions: [{}] [{}]"

        @ Software framework version
        telemetry FRAMEWORK_VERSION: string size 40 id 0

        @ Software project version
        telemetry PROJECT_VERSION: string size 40 id 1

        @ Custom Versions
        telemetry CUSTOM_VERSION_01: CusVerDb id 2 
        telemetry CUSTOM_VERSION_02: CusVerDb id 3 
        telemetry CUSTOM_VERSION_03: CusVerDb id 4 
        telemetry CUSTOM_VERSION_04: CusVerDb id 5 
        telemetry CUSTOM_VERSION_05: CusVerDb id 6 
        telemetry CUSTOM_VERSION_06: CusVerDb id 7 
        telemetry CUSTOM_VERSION_07: CusVerDb id 8 
        telemetry CUSTOM_VERSION_08: CusVerDb id 9 
        telemetry CUSTOM_VERSION_09: CusVerDb id 10
        telemetry CUSTOM_VERSION_10: CusVerDb id 11

        @ Library Versions
        telemetry LIBRARY_VERSION_01: string size 40 id 12 
        telemetry LIBRARY_VERSION_02: string size 40 id 13 
        telemetry LIBRARY_VERSION_03: string size 40 id 14 
        telemetry LIBRARY_VERSION_04: string size 40 id 15 
        telemetry LIBRARY_VERSION_05: string size 40 id 16 
        telemetry LIBRARY_VERSION_06: string size 40 id 17 
        telemetry LIBRARY_VERSION_07: string size 40 id 18 
        telemetry LIBRARY_VERSION_08: string size 40 id 19 
        telemetry LIBRARY_VERSION_09: string size 40 id 20 
        telemetry LIBRARY_VERSION_10: string size 40 id 21 


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