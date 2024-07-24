#####
# Version Ports:
#
# A port setting/getting custom versions per project.
#####

module Svc{
    
    @ An enumeration for version status
    enum VersionStatus {
        OK = 0 @< Version was good
        FAILURE = 1 @< Failure to get version
    }

    @ Port for setting and getting Versions
    port Version(
             version_id: VersionCfg.VersionEnum @< The entry to access
             ref version_string: string @< The value to be passed
             ref status: VersionStatus @< The command response argument
    )
}
