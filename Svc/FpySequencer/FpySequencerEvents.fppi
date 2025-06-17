event InvalidCommand($state: I32) \
    severity warning high \
    format "Cannot run command in state {}"

event InvalidSeqRunCall($state: I32) \
    severity warning high \
    format "Cannot run sequence from a port in state {}"

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
    severity warning high \
    format "Expected schema version {}, found {}"

event WrongCRC(
    expected: U32
    actual: U32
) \
    severity warning high \
    format "Expected CRC {}, actual was {}"

event ExtraBytesInSequence(
    remaining: U32
) \
    severity warning high \
    format "File had {} extra bytes at the end"

event InsufficientBufferSpace(
    bufferSize: U64
    filePath: string
) \
    severity warning high \
    format "Buffer capacity of {} was not big enough for sequence {}"

# runtime

event StatementFailed(
    stmtType: Fpy.StatementType
    stmtOpcode: FwOpcodeType
    stmtIdx: U32
    filePath: string
    response: Fw.CmdResponse
) \
    severity warning high \
    format "Failed to execute statement type {} opcode {} index {} in sequence file {}: response was {}"

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
    $opcode: FwOpcodeType
    stmtIdx: U32
    filePath: string
) \
    severity warning high \
    format "Unknown sequencer directive id {} at index {} in file {}"

event CommandSerializeError(
    cmdOpcode: FwOpcodeType
    bufCapacity: U64
    curPos: U64
    writeSize: U64
    errorCode: U8
    stmtIdx: U32
) \
    severity warning high \
    format "Failure while serializing cmd opcode {}: buf capacity {}, cur write pos {}, write size {}, error code {}, current statement index {}"

event CmdResponseWhileNotRunningSequence(
    $state: I32
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
) \
    severity warning low \
    format "Received a command response while not running a sequence (was in state {} opcode was {} response code {})"

event CmdResponseFromOldSequence(
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
    oldSequenceIdx: U16
    currentSequenceIdx: U16
) \
    severity warning low \
    format "Received a response from cmd opcode {} (response {}), but it was from a previous sequence, not the current one (old idx: {}, current idx: {})"

event CmdResponseWhileNotAwaiting(
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
) \
    severity warning high \
    format "Received a response from cmd opcode {} (response {}) from this sequence, but was not awaiting a response"

event WrongCmdResponseOpcode(
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
    expectedOpcode: FwOpcodeType
) \
    severity warning high \
    format "Received a response from cmd opcode {} (response {}) from this sequence, but was expecting a response from command opcode {}"

event WrongCmdResponseIndex(
    $opcode: FwOpcodeType
    response: Fw.CmdResponse
    actualCmdIdx: U16
    expectedCmdIdx: U16
) \
    severity warning high \
    format "Received a response from the correct cmd (opcode {} response {}), but it was for a different instance of that opcode in the same sequence (actual idx {} expected {})"

# TODO make sure to call this with appropriate stmt idx when directive is dynamically constructed
event DirectiveDeserializeError(
    $opcode: FwOpcodeType
    stmtIdx: U32
    errorCode: I32
    buffLeft: U64
    buffLength: U64
) \
    severity warning high \
    format "Deserialize error encountered while reading directive opcode {} at index {}: {} ({} bytes left out of {})"

event MismatchedTimeBase(
    internalTimeBase: I32
    otherTimeBase: I32
) \
    severity warning high \
    format "getTime() time base was {}, but tried to operate on it with time base {}"

event MismatchedTimeContext(
    internalTimeContext: I32
    otherTimeContext: I32
) \
    severity warning high \
    format "getTime() time context was {}, but tried to operate on it with time context {}"

event StatementTimedOut(
    stmtType: Fpy.StatementType
    stmtOpcode: FwOpcodeType
    stmtIdx: U32
    filePath: string
) \
    severity warning high \
    format "A statement type {} opcode {} at index {} timed out in sequence {}, causing the sequence to fail"

event TooManySequenceArgs(
    count: U8
    max: U8
) \
    severity warning high \
    format "A sequence specified it had {} args but the max was {}"

event TooManySequenceStatements(
    count: U16
    max: U16
) \
    severity warning high \
    format "A sequence specified it had {} statements but the max was {}"

event DebugBroken(
    stmtIdx: U32
    breakOnce: bool
) \
    severity activity high \
    format "Debug breakpoint reached before dispatching statement index {}. Will break once: {}"

event DebugBreakpointSet(
    breakpointIdx: U32
    breakOnce: bool
) \
    severity activity high \
    format "Debug breakpoint set before statement index {}. Will break once: {}"

event DebugBreakpointCleared() \
    severity activity high \
    format "Debug breakpoint cleared"
    