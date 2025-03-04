event InvalidCommand($state: I32) \
    severity warning low \
    format "Cannot run command in state {}"

event FileOpenError(
    filePath: string
    errorCode: I32
) \
    severity warning high \
    format "File open error encountered while opening {}: {}"

event FileReadError(
    filePath: string
    errorCode: I32
) \
    severity warning high \
    format "File read error encountered while reading {}: {}"

event EndOfFileError(
    filePath: string
) \
    severity warning high \
    format "End of file encountered unexpectedly while reading {}"

event FileReadDeserializeError(
    filePath: string
    errorCode: I32
    buffLeft: U32
    buffLength: U32
) \
    severity warning high \
    format "Deserialize error encountered while reading {}: {} ({} bytes left out of {})"

event ReadHeaderSuccess(
    filePath: string
) \
    severity diagnostic \
    format "Successfully read header of {}"

event ReadBodySuccess(
    filePath: string
) \
    severity diagnostic \
    format "Successfully read body of {}"

event ReadFooterSuccess(
    filePath: string
) \
    severity diagnostic \
    format "Successfully read footer of {}"

event WrongSchemaVersion(
    expected: U8,
    actual: U8
) \
    severity warning low \
    format "Expected schema version {}, found {}"

event InvalidSequence(filePath: string) \
    severity warning high \
    format "Failed to validate sequence {}"

# runtime

event CancellingOnNextStatement() \
    severity diagnostic \
    format "Scheduling the sequence for cancellation on the next statement"

event SequenceFailed(
    filePath: string
) \
    severity warning low \
    format "Failed to execute sequence file {}"

event SequenceDone(
    filePath: string
) \
    severity activity high \
    format "Completed sequence file {}"

event SequenceCancelled(
    filePath: string
) \
    severity activity high \
    format "Cancelled sequence file {}"

event UnknownSequencerDirective(
    $opcode: U32
) \
    severity warning high \
    format "Unknown sequencer directive id {}"

event SerializeError(
    bufCapacity: U32, 
    curPos: U32, 
    writeSize: U32, 
    errorCode: U8
) \
    severity warning high \
    format "Serialization failure: buf capacity {}, cur write pos {}, write size {}, error code {}"

event UnexpectedStatementResponseForState(
    $state: I32,
    $opcode: U32,
    response: Fw.CmdResponse
) \
    severity warning low \
    format "Received a statement response while in state {} (opcode {} response code {})"

event UnexpectedStatementResponseOpcode(
    expectedOpcode: U32,
    $opcode: U32,
    response: Fw.CmdResponse
) \
    severity warning low \
    format "Was expecting a response from statement opcode {}, but got one for opcode {} (response code {})"

event DirectiveDeserializeError(
    $opcode: U32
    errorCode: I32
    buffLeft: U32
    buffLength: U32
) \
    severity warning high \
    format "Deserialize error encountered while reading directive opcode {}: {} ({} bytes left out of {})"