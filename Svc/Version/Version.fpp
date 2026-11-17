module Svc {
    
    @ Tracks versions for project, framework and user defined versions etc
    enum VersionEnabled : U8 {
        DISABLED = 0 @< verbosity disabled
        ENABLED = 1 @< verbosity enabled
    }
    
   
    @ An enumeration for Version Type
    enum VersionType : U8 {
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

        @ Scheduled port to periodically re-emit version telemetry
        guarded input port schedIn: Svc.Sched
        
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
        format "Library Versions: [{}]"
        
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

        # Custom Versions 11-32 (ids 2-11 hold CustomVersion01-10, ids 12-21 the library versions)
        telemetry CustomVersion11: CustomVersionDb id 22
        telemetry CustomVersion12: CustomVersionDb id 23
        telemetry CustomVersion13: CustomVersionDb id 24
        telemetry CustomVersion14: CustomVersionDb id 25
        telemetry CustomVersion15: CustomVersionDb id 26
        telemetry CustomVersion16: CustomVersionDb id 27
        telemetry CustomVersion17: CustomVersionDb id 28
        telemetry CustomVersion18: CustomVersionDb id 29
        telemetry CustomVersion19: CustomVersionDb id 30
        telemetry CustomVersion20: CustomVersionDb id 31
        telemetry CustomVersion21: CustomVersionDb id 32
        telemetry CustomVersion22: CustomVersionDb id 33
        telemetry CustomVersion23: CustomVersionDb id 34
        telemetry CustomVersion24: CustomVersionDb id 35
        telemetry CustomVersion25: CustomVersionDb id 36
        telemetry CustomVersion26: CustomVersionDb id 37
        telemetry CustomVersion27: CustomVersionDb id 38
        telemetry CustomVersion28: CustomVersionDb id 39
        telemetry CustomVersion29: CustomVersionDb id 40
        telemetry CustomVersion30: CustomVersionDb id 41
        telemetry CustomVersion31: CustomVersionDb id 42
        telemetry CustomVersion32: CustomVersionDb id 43

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
    }
}
