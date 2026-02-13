event BuffersExhausted severity warning low \
    format "Unable to allocate a PDU buffer"

event InvalidChannel(
    channelId: U8 @< Requested channel index
    maxChannelId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event SendFileInitiated(
    sourceFileName: string size MaxFileSize @< Source file being sent
) \
    severity activity low \
    format "Successfully initiated file send transfer for {}" 

event UnsupportedSendFileArguments(
    offset: U32 @< Offset of the send file request
    length: U32 @< Length of the send file request
) \
    severity warning low \
    format "Invalid send file port request with offset {}, length {}" 

event SendFileInitiateFail(
    sourceFileName: string size MaxFileSize @< Source file being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event PlaybackInvalidChannel(
    channelId: U8 @< Requested channel index
    maxChannelId: U8 @< Maximum channel index
) \
    severity warning low \
    format "Invalid channel ID {}, maximum channel ID is {}" 

event PlaybackInitiated(
    sourceDirectory: string size MaxFileSize @< Source directory being sent
) \
    severity activity low \
    format "Successfully initiated directory playback for {}" 

event PlaybackInitiateFail(
    sourceDirectory: string size MaxFileSize @< Source directory being sent
) \
    severity warning low \
    format "Failed to initiate file send transfer for {}"

event PollDirInitiated(
    sourceDirectory: string size MaxFileSize @< Source directory being sent
) \
    severity activity low \
    format "Successfully initiated directory poll for {}"

event PollDirStopped(
    channelId: U8 @< Channel index stopped
    pollId: U8 @< Channel poll index stopped
) \
    severity activity low \
    format "Successfully stopped directory poll for channel {}, index {}"

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
    srcFile: string size MaxFileSize @< Source file being moved
    moveDir: string size MaxFileSize @< Directory file was moved to
    status: I32 @< Status of the move operation
) \
    severity warning low \
    format "Failed to move {} to {} error {}" 

event FailPollFileMove(
    srcFile: string size MaxFileSize @< Source file being moved
    failDir: string size MaxFileSize @< Directory file was moved to
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

event RxAckLimitReached(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "RX class {} ACK limit reached for transaction {}:{}, no fin-ack sent"

event RxTempFileCreated(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    filename: string size MaxFileSize @< Temporary filename created
) \
    severity activity low \
    format "RX class {} transaction {}:{} creating temp file {} without metadata"

event RxFileCreateFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    filename: string size MaxFileSize @< File that failed to create
    status: I32 @< File operation status
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to create file {}, error={}"

event RxCrcMismatch(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    expected: U32 @< Expected CRC value
    actual: U32 @< Actual CRC value
) \
    severity warning high \
    format "RX class {} transaction {}:{} CRC mismatch: expected 0x{x} actual 0x{x}"

event RxNakLimitReached(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "RX class {} transaction {}:{} NAK limit reached"

event RxSeekFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    offset: U32 @< Offset that failed to seek
    status: I32 @< File operation status
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to seek to offset {}, error={}"

event RxWriteFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    expected: U32 @< Expected bytes to write
    actual: I32 @< Actual bytes written
) \
    severity warning high \
    format "RX class {} transaction {}:{} write failed: expected {} bytes, got {}"

event RxFileSizeMismatch(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    expected: U32 @< Expected file size from metadata
    actual: U32 @< Actual file size from EOF
) \
    severity warning high \
    format "RX class {} transaction {}:{} EOF file size mismatch: expected {} got {}"

event RxInvalidEofPdu(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning low \
    format "RX class {} transaction {}:{} received invalid EOF PDU"

event RxSeekCrcFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    offset: U32 @< Offset that failed during CRC calculation
    status: I32 @< File operation status
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to seek offset {} during CRC calculation, error={}"

event RxReadCrcFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    expected: U32 @< Expected bytes to read
    actual: I32 @< Actual bytes read
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to read during CRC calculation: expected {} bytes, got {}"

event RxEofMdSizeMismatch(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    mdSize: U32 @< File size from metadata
    eofSize: U32 @< File size from EOF
) \
    severity warning high \
    format "RX class {} transaction {}:{} EOF/metadata size mismatch: metadata={}, EOF={}"

event RxFileRenameFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    tempFile: string size MaxFileSize @< Temporary file path
    finalFile: string size MaxFileSize @< Final file path
    status: I32 @< File system operation status
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to rename {} to {}, error={}"

event RxFileReopenFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    filename: string size MaxFileSize @< File that failed to reopen
    status: I32 @< File operation status
) \
    severity warning high \
    format "RX class {} transaction {}:{} failed to reopen file {} after rename, error={}"

event RxInactivityTimeout(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "RX class {} transaction {}:{} inactivity timer expired"

event RxInvalidDirectiveCode(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    directiveCode: U8 @< Invalid directive code received
    substate: U8 @< Current transaction substate
) \
    severity warning low \
    format "RX class {} transaction {}:{} received invalid directive code {} for substate {}"

event TxAckLimitReached(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "TX class {} transaction {}:{} ACK limit reached, no eof-ack received"

event TxInactivityTimeout(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "TX class {} transaction {}:{} inactivity timer expired"

event TxFileOpenFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    filename: string size MaxFileSize @< File that failed to open
    status: I32 @< File operation status
) \
    severity warning high \
    format "TX class {} transaction {}:{} failed to open file {}, error={}"

event TxFileSeekFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    status: I32 @< File operation status
) \
    severity warning high \
    format "TX class {} transaction {}:{} failed to seek to beginning of file, error={}"

event TxSendMetadataFailed(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "TX class {} transaction {}:{} failed to send metadata PDU"

event TxEarlyFinReceived(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "TX class {} transaction {}:{} received early FIN, cancelling transfer"

event TxInvalidNakPdu(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning high \
    format "TX class {} transaction {}:{} received invalid NAK PDU"

event TxInvalidSegmentRequests(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    badCount: U32 @< Number of invalid segment requests
) \
    severity warning low \
    format "TX class {} transaction {}:{} received {} invalid NAK segment requests"

event TxNonFileDirectivePduReceived(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
) \
    severity warning low \
    format "TX class {} transaction {}:{} received non-file-directive PDU"

event TxInvalidDirectiveCode(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    seqNum: U32 @< Transaction sequence number
    directiveCode: U8 @< Invalid directive code received
    substate: U8 @< Current transaction substate
) \
    severity warning low \
    format "TX class {} transaction {}:{} received invalid directive code {} for substate {}"

event TxFileTransferStarted(
    cfdpClass: U8 @< CFDP class (1 or 2)
    srcEid: U32 @< Source entity ID
    srcFile: string size MaxFileSize @< Source filename
    destEid: U32 @< Destination entity ID
    destFile: string size MaxFileSize @< Destination filename
) \
    severity activity high \
    format "TX starting class {} transfer {}:{} -> {}:{}"

event MetadataReceived(
    srcFile: string size MaxFileSize @< Source filename from metadata
    destFile: string size MaxFileSize @< Destination filename from metadata
) \
    severity activity low \
    format "Metadata received for source: {}, dest: {}"

event FileDataSegmentMetadata severity warning high \
    format "File data PDU with unsupported segment metadata received"

event ChunklistUnavailable(
    transactionSeq: U32 @< Transaction sequence number
) \
    severity warning high \
    format "Cannot get chunklist, abandoning transaction {}"

event UnhandledPduInIdleState severity warning low \
    format "Unhandled PDU type received in idle state"

event RxTransactionLimitReached(
    srcEid: U32 @< Source entity ID
    transactionSeq: U32 @< Transaction sequence number
) \
    severity warning high \
    format "Dropping packet from {} transaction {} due to max RX transactions reached"

event InvalidDestinationEid(
    destEid: U32 @< Invalid destination entity ID
) \
    severity warning low \
    format "Dropping packet for invalid destination EID {}"

event MaxTxTransactionsReached severity warning high \
    format "Maximum number of commanded TX files reached"

event PlaybackDirOpenFailed(
    directory: string size MaxFileSize @< Directory that failed to open
    status: I32 @< Directory operation status
) \
    severity warning high \
    format "Failed to open playback directory {}, error={}"

event PlaybackDirSlotUnavailable severity warning high \
    format "No playback directory slot available"

event ResetFreedTransaction severity diagnostic \
    format "Attempt to reset a transaction that has already been freed"

event FileRemoveFailed(
    filename: string size MaxFileSize @< File that failed to delete
    status: I32 @< File system operation status
) \
    severity warning low \
    format "Failed to remove file {}, error={}"
