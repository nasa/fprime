event BuffersExuasted severity warning low \
    format "Unable to alocate a PDU buffer"

event InvalidChannel(
    channelNum: U8 @< Requested channel number
    maxChannelNum: U8 @< Maximum channel number
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event SendFileInitiatied(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Succesfully initiated file send transfer for {}" 

event SendFileFailInitiate(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event PlaybackInvalidChannel(
    channelNum: U8 @< Requested channel number
    maxChannelNum: U8 @< Maximum channel number
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event PlaybackInitiatied(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Succesfully initiated file send transfer for {}" 

event PlaybackInitiate(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event SetFlowState(
    channelId: U8 @< Channel being set
    flowState: CfdpFlow @< Flow state set
) \
    severity activity low \
    format "Set channel {} to {}" 