event CfdpBuffersExuasted severity warning low \
    format "Unable to alocate a PDU buffer"

event CfdpInvalidChannel(
    channelNum: U8 @< Requested channel number
    maxChannelNum: U8 @< Maximum channel number
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event CfdpSendFileInitiatied(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Succesfully initiated file send transfer for {}" 

event CfdpSendFileFailInitiate(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event CfdpPlaybackInvalidChannel(
    channelNum: U8 @< Requested channel number
    maxChannelNum: U8 @< Maximum channel number
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event CfdpPlaybackInitiatied(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Succesfully initiated file send transfer for {}" 

event CfdpPlaybackInitiate(
    sourceFileName: string size CfdpManagerMaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"