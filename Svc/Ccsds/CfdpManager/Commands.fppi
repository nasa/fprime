@ Command to start a CFDP file transaction
async command SendFile(
    cfdpClass: CfdpClass @< CFDP class for the file transfer
    keep: CfdpKeep @< Whether or not to keep or delete the file upon completion
    channelNum: U8 @< Channel number for the file transaction
    $priority: U8 @< Priority: 0=highest priority
    destId: CfdpEntityId @< Destination entity id
    sourceFileName: string size CfdpManagerMaxFileSize @< The name of the on-board file to send
    destFileName: string size CfdpManagerMaxFileSize @< The name of the destination file on the ground
)

@ Command to start a directory playback
async command PlaybackDirectory(
    cfdpClass: CfdpClass @< CFDP class for the file transfer(s)
    keep: CfdpKeep @< Whether or not to keep or delete the file(s) upon completion
    channelNum: U8 @< Channel number for the file transaction(s)
    $priority: U8 @< Priority: 0=highest priority
    destId: CfdpEntityId @< Destination entity id
    sourceDirectory: string size CfdpManagerMaxFileSize @< The name of the on-board directory to send
    destDirectory: string size CfdpManagerMaxFileSize @< The name of the destination directory on the ground
)

@ Command to set channel's flow status
async command SetChannelFlow(
    channelNum: U8 @< Channel number to set
    freeze: CfdpFlow @< Flow state to set
)