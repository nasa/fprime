#####
# File announce ports:
#
# A port for announcing the availability of new files, e.g., after a successful file uplink.
#####

module Svc{
    
    @ Port for announcing new files
    port FileAnnounce(
             ref file_name: string @< The successfully uplinked file
    )
}
