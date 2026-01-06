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