event BuffersExuasted severity warning low \
    format "Unable to alocate a PDU buffer"

event InvalidChannel(
    channelId: U8 @< Requested channel index
    maxChannelId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event SendFileInitiatied(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Succesfully initiated file send transfer for {}" 

event SendFileInitiateFail(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event PlaybackInvalidChannel(
    channelId: U8 @< Requested channel index
    maxChannelId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event PlaybackInitiatied(
    sourceDirectory: string size CfdpManagerMaxFileSize @< Source directory being sent
) \
    severity activity low \
    format "Succesfully initiated directory playback for {}" 

event PlaybackInitiateFail(
    sourceDirectory: string size CfdpManagerMaxFileSize @< Source directory being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event PollDirInitiatied(
    sourceDirectory: string size CfdpManagerMaxFileSize @< Source directory being sent
) \
    severity activity low \
    format "Succesfully initiated directory poll for {}"

event PollDirStopped(
    channelId: U8 @< Channel index stopped
    pollId: U8 @< Channel poll index stopped
) \
    severity activity low \
    format "Succesfully stopped directory poll for channel {}, index {}"

event SetFlowState(
    channelId: U8 @< Channel being set
    flowState: CfdpFlow @< Flow state set
) \
    severity activity low \
    format "Set channel {} to {}" 

event InvalidChannelPoll(
    pollId: U8 @< Requested channel index
    maxPollId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid poll ID {}, maximum poll ID is {}" 