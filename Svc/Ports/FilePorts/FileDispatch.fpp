#####
# File dispatch ports:
#
# A port setting/getting custom versions per project.
#####

module Svc{
    
    @ Port for dispatching files
    port FileDispatch(
             ref file_name: string @< The file to dispatch
    )
}
