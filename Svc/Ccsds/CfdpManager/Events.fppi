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
    flowState: Cfdp.Flow @< Flow state set
) \
    severity activity low \
    format "Set channel {} to {}" 

event InvalidChannelPoll(
    pollId: U8 @< Requested channel index
    maxPollId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid poll ID {}, maximum poll ID is {}" 

event FailKeepFileMove(
    srcFile: string size CfdpManagerMaxFileSize @< Source file being moved
    moveDir: string size CfdpManagerMaxFileSize @< Directory file was moved to
    status: I32 @< Status of the move operation
) \
    severity warning low \
    format "Failed to move {} to {} error {}" 

event FailPollFileMove(
    srcFile: string size CfdpManagerMaxFileSize @< Source file being moved
    failDir: string size CfdpManagerMaxFileSize @< Directory file was moved to
    status: I32 @< Status of the move operation
) \
    severity warning low \
    format "Failed to move {} to {} error {}"

event FailPduHeaderDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize PDU header on channel {}, status {}"

event FailMetadataPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize Metadata PDU on channel {}, status {}"

event FailFileDataPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize File Data PDU on channel {}, status {}"

event FailEofPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize EOF PDU on channel {}, status {}"

event FailAckPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize ACK PDU on channel {}, status {}"

event FailFinPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize FIN PDU on channel {}, status {}"

event FailNakPduSerialization(
    channelId: U8 @< Channel sending the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to serialize NAK PDU on channel {}, status {}"

event FailMetadataPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize Metadata PDU on channel {}, status {}"

event FailFileDataPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize File Data PDU on channel {}, status {}"

event FailEofPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize EOF PDU on channel {}, status {}"

event FailAckPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize ACK PDU on channel {}, status {}"

event FailFinPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize FIN PDU on channel {}, status {}"

event FailNakPduDeserialization(
    channelId: U8 @< Channel that received the PDU
    status: I32 @< Serialization status code
) \
    severity warning low \
    format "Failed to deserialize NAK PDU on channel {}, status {}"