module Svc {
    
    @ Tracks versions for project, framework and user defined versions etc
    
    enum VersionEnabled {
        DISABLED = 0 @< verbosity disabled
        ENABLED = 1 @< verbosity enabled
    }
    
   
    @ An enumeration for Version Type
    enum VersionType {
        PROJECT = 0 @< project version
        FRAMEWORK = 1 @< framework version
        LIBRARY = 2 @< library version
        CUSTOM = 3 @< custom version
        ALL = 4 @< all above versions
    }

    @Data Structure for custom version Tlm
    struct CustomVersionDb {
        version_enum: VersionCfg.VersionEnum @<enumeration/name of the custom version
        version_value: string size 80  @< string containing custom version
        version_status : Svc.VersionStatus @< status of the custom version
    }

    passive component Version {

        ##############################################################################
        #### Customizing version component ####
        ##############################################################################
        
        @ Mutexed Port to get values
        guarded input port getVersion: Svc.Version

        @ Mutexed Port to set values
        guarded input port setVersion: Svc.Version
        
        @ A command to enable or disable Event verbosity and Telemetry
        guarded command ENABLE(
                                enable: VersionEnabled @< whether or not Version telemetry is enabled
                              ) \
        opcode 0 

        @ Report version as Event
        guarded command VERSION(
                                version_type: VersionType @<which version type Event is requested
                               ) \
        opcode 1

        @ Version of the git repository.
        event FrameworkVersion(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 0 \
        format "Framework Version: [{}]"

        @ Version of the git repository.
        event ProjectVersion(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 1 \
        format "Project Version: [{}]"

        @ Version of the git repository.
        event LibraryVersions(
                   version: string size 40 @< version string
                 ) \
        severity activity low \
        id 2 \
        format "Libary Versions: [{}]"
        
        @ Version of the git repository.
        event CustomVersions(
                   version_enum : VersionCfg.VersionEnum @< The enum to access
                   version_value : string size 40 @< version 
                 ) \
        severity activity low \
        id 3 \
        format "Custom Versions: [{}] [{}]"

        @ Software framework version
        telemetry FrameworkVersion: string size 40 id 0

        @ Software project version
        telemetry ProjectVersion: string size 40 id 1

        @ Custom Versions
        telemetry CustomVersion01: CustomVersionDb id 2 
        telemetry CustomVersion02: CustomVersionDb id 3 
        telemetry CustomVersion03: CustomVersionDb id 4 
        telemetry CustomVersion04: CustomVersionDb id 5 
        telemetry CustomVersion05: CustomVersionDb id 6 
        telemetry CustomVersion06: CustomVersionDb id 7 
        telemetry CustomVersion07: CustomVersionDb id 8 
        telemetry CustomVersion08: CustomVersionDb id 9 
        telemetry CustomVersion09: CustomVersionDb id 10
        telemetry CustomVersion10: CustomVersionDb id 11

        @ Library Versions
        telemetry LibraryVersion01: string size 40 id 12 
        telemetry LibraryVersion02: string size 40 id 13 
        telemetry LibraryVersion03: string size 40 id 14 
        telemetry LibraryVersion04: string size 40 id 15 
        telemetry LibraryVersion05: string size 40 id 16 
        telemetry LibraryVersion06: string size 40 id 17 
        telemetry LibraryVersion07: string size 40 id 18 
        telemetry LibraryVersion08: string size 40 id 19 
        telemetry LibraryVersion09: string size 40 id 20 
        telemetry LibraryVersion10: string size 40 id 21 

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