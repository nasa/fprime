#####
# File dispatch ports:
#
# A port for dispatching files (e.g., routing an uplinked file to a handler by type).
#####

module Svc{

    @ Port for dispatching files
    port FileDispatch(
             ref file_name: string size FileNameStringSize @< The file to dispatch
    )
}
