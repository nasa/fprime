#####
# File announce ports:
#
# A port setting/getting custom versions per project.
#####

module Svc{
    
    @ Port for announcing new files
    port FileAnnounce(
             ref file_name: string @< The successfully uplinked file
    )
}
