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
    buffLeft: U64
    buffLength: U64
) \
    severity warning high \
    format "Deserialize error encountered while reading {}: {} ({} bytes left out of {})"

event WrongSchemaVersion(
    expected: U8
    actual: U8
) \
    severity warning low \
    format "Expected schema version {}, found {}"

event WrongCRC(
    expected: U32
    actual: U32
) \
    severity warning low \
    format "Expected CRC {}, actual was {}"

event ExtraBytesInSequence(
    remaining: U32
) \
    severity warning low \
    format "File had {} extra bytes at the end"

event OutOfBufferSpace(
    bufferSize: U32
    filePath: string
) \
    severity warning low \
    format "Buffer capacity of {} was not big enough for sequence {}"

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
    bufCapacity: U64
    curPos: U64
    writeSize: U64
    errorCode: U8
) \
    severity warning high \
    format "Serialization failure: buf capacity {}, cur write pos {}, write size {}, error code {}"

event UnexpectedStatementResponseForState(
    $state: I32
    $opcode: U32
    response: Fw.CmdResponse
) \
    severity warning low \
    format "Received a statement response while in state {} (opcode {} response code {})"

event WrongStatementResponseOpcode(
    expectedOpcode: U32
    $opcode: U32
    response: Fw.CmdResponse
) \
    severity warning low \
    format "Was expecting a response from statement opcode {}, but got one for opcode {} (response code {})"

event DirectiveDeserializeError(
    $opcode: U32
    errorCode: I32
    buffLeft: U64
    buffLength: U64
) \
    severity warning high \
    format "Deserialize error encountered while reading directive opcode {}: {} ({} bytes left out of {})"

event MismatchedTimeBase(
    internalTimeBase: I32
    otherTimeBase: I32
) \
    severity warning low \
    format "getTime() time base was {}, but tried to operate on it with time base {}"

event MismatchedTimeContext(
    internalTimeContext: I32
    otherTimeContext: I32
) \
    severity warning low \
    format "getTime() time context was {}, but tried to operate on it with time context {}"

event SequenceTimedOut(
    filePath: string
) \
    severity warning low \
    format "A statement timed out in sequence {}, causing the sequence to fail"

event TooManyArgs(
    count: U8
    max: U8
) \
    severity warning high \
    format "A sequence specified it had {} args but the max was {}"

event TooManyStatements(
    count: U16
    max: U16
) \
    severity warning high \
    format "A sequence specified it had {} statements but the max was {}"