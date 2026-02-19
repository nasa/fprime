@ Command to start a CFDP file transaction
async command SendFile(
    channelId: U8 @< Channel ID for the file transaction
    destId: Cfdp.EntityId @< Destination entity id
    cfdpClass: Cfdp.Class @< CFDP class for the file transfer
    keep: Cfdp.Keep @< Whether or not to keep or delete the file upon completion
    $priority: U8 @< Priority: 0=highest priority
    sourceFileName: string size MaxFilePathSize @< The name of the on-board file to send
    destFileName: string size MaxFilePathSize @< The name of the destination file on the ground
)

@ Command to start a directory playback
async command PlaybackDirectory(
    channelId: U8 @< Channel ID for the file transaction(s)
    destId: Cfdp.EntityId @< Destination entity id
    cfdpClass: Cfdp.Class @< CFDP class for the file transfer(s)
    keep: Cfdp.Keep @< Whether or not to keep or delete the file(s) upon completion
    $priority: U8 @< Priority: 0=highest priority
    sourceDirectory: string size MaxFilePathSize @< The name of the on-board directory to send
    destDirectory: string size MaxFilePathSize @< The name of the destination directory on the ground
)

@ Command to start a directory poll
async command PollDirectory(
    channelId: U8 @< Channel ID for the file transaction(s)
    pollId: U8 @< Channel poll ID for the file transaction(s)
    destId: Cfdp.EntityId @< Destination entity id
    cfdpClass: Cfdp.Class @< CFDP class for the file transfer(s)
    $priority: U8 @< Priority: 0=highest priority
    interval: U32 @< Interval to poll the directory in seconds
    sourceDirectory: string size MaxFilePathSize @< The name of the on-board directory to send
    destDirectory: string size MaxFilePathSize @< The name of the destination directory on the ground
)

@ Command to stop a directory poll
async command StopPollDirectory(
    channelId: U8 @< Channel ID to stop
    pollId: U8 @< Channel poll ID to stop
)

@ Command to set channel's flow status
async command SetChannelFlow(
    channelId: U8 @< Channel ID to set
    freeze: Cfdp.Flow @< Flow state to set
)

@ Command to suspend or resume a transaction
async command SuspendResumeTransaction(
    channelId: U8 @< Channel ID for the transaction
    transactionSeq: Cfdp.TransactionSeq @< Transaction sequence number
    entityId: Cfdp.EntityId @< Entity ID of the transaction
    $action: Cfdp.SuspendResume @< Action to take: SUSPEND or RESUME
)

@ Command to cancel a transaction with graceful close-out
async command CancelTransaction(
    channelId: U8 @< Channel ID for the transaction
    transactionSeq: Cfdp.TransactionSeq @< Transaction sequence number
    entityId: Cfdp.EntityId @< Entity ID of the transaction
)

@ Command to abandon a transaction immediately
async command AbandonTransaction(
    channelId: U8 @< Channel ID for the transaction
    transactionSeq: Cfdp.TransactionSeq @< Transaction sequence number
    entityId: Cfdp.EntityId @< Entity ID of the transaction
)