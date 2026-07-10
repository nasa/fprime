struct SequenceExecutionArgs {
    filePath: string size FileNameStringSize
    $block: Svc.BlockState
    buffer: Svc.SeqArgs
}

struct ModuleLoad {
    fileName: string size FileNameStringSize
    moduleName: string size 16
}

struct FunctionInvoke {
    moduleName: string size 16
    functionName: string size 64
}

enum TrapReason {
    @ Triggered by unreachable instruction
    Unreachable,
    @ A host function has noted an unrecoverable failure
    Host,
    @ Integer or floating point division by zero
    DivideByZero,
    @ An indirect call tried to map to a table function out of range
    InvalidTableIndex,
    @ The function type in an indirect call does not match the function pointer's type
    InvalidTableFunctionType,
    @ An indirect call tried to map to a table function out of range
    UninitializedTableElement,
    @ An imported global could not be read
    GlobalGetFailed,
    @ An imported global could not be set
    GlobalSetFailed,
    @ A memory operation is out of bounds
    OutOfMemory,
    @ memory.grow failed because a host function has taken ownership of a memory
    MemoryRefNotUnique,
    @ A memory operation is out of bounds
    MemoryOutOfBounds,
    @ Ran out of stack space
    StackOverflow,
    @ Attempting to convert Inf to integer
    UnrepresentableResult,
    @ Signed division causes integer overflow
    IntegerOverflow,
    @ Attempting to convert NaN to integer
    BadConversionToInteger,
}

enum Section {
    Custom,
    Type,
    Import,
    Function,
    Table,
    Memory,
    Global,
    Export,
    Start,
    Element,
    Code,
    Data,
    None,
}

struct LoadFailure {
    @ Wasm section where the validation error was detected
    section: Section
    @ Offset in-bytes where the decoding/validation failed
    offset: U32,
    @ The validation failure
    error: ValidationError,
    @ Optional additional context about the validation failure
    context: U8,
}

enum ValidationError : U8 {
    Eof,
    MalformedInteger,
    I33IsNegative,
    MalformedMagic,
    MalformedVersion,
    MalformedUtf8,
    DuplicateModuleName,
    DuplicateExportName,
    MalformedSectionId,
    MalformedValueType,
    MalformedFunction,
    MalformedLimit,
    MalformedElemType,
    MalformedSectionSize,
    ExpectedConstOrVar,
    MalformedImportExportDesc,
    InvalidSectionOrdering,
    DuplicateSection,
    InvalidMaxLimit,
    ExpectedTerminal,
    InvalidOpcode,
    MalformedCodeSize,
    InvalidCodeSectionFunctionCount,
    VecTooLong,
    IdxTooLarge,
    ModuleIdxTooLarge,
    MemoryTooLarge,
    MemoryImportTooLarge,
    MemAlignTooLarge,
    ControlFlowTooDeep,
    StackUnderflow,
    StackTooLarge,
    LabelStackJumpTooDeep,
    LabelJumpTooLarge,
    TypeMismatch,
    BlockResultTypeMismatch,
    FunctionResultTypeMismatch,
    IllegalMemoryGrow,
    InvalidElementOffset,
    InvalidElementOutOfBounds,
    InvalidTableIndex,
    TableNotDefined,
    InvalidElementCount,
    InvalidMemIndex,
    MemoryNotDefined,
    InvalidMemOffsetType,
    InvalidNegativeMemOffset,
    InvalidMemOffset,
    InvalidLabelIndex,
    InvalidElseBlock,
    InvalidEndBlock,
    MultipleMemories,
    MultipleTables,
    PossibleBackpatchCycle,
    PageFault,
    InstructionOutsideOfFunction,
    LocalIdxOutOfRange,
    FunctionIdxOutOfRange,
    TypeIdxOutOfRange,
    FunctionTextOutOfRange,
    GlobalIdxOutOfRange,
    FunctionImportNotFound,
    GlobalImportNotFound,
    MemoryImportNotFound,
    TableImportNotFound,
    FunctionImportOutOfRange,
    FunctionImportTypeMismatch,
    GlobalIsNotMutable,
    GlobalImportTypeMismatch,
    MemoryImportTypeMismatch,
    TableImportTypeMismatch,
    TableImportIncompatibleSize,
    FunctionParametersTooLarge,
    FunctionReturnsTooLarge,
    TooManyLocals,
    InvalidConstInstruction,
    BrTableHasTooManyCases,
    GlobalTypeMismatch,
    AlignmentLargerThanType,
    InvalidStartFunctionSignature,
    InvalidConstantExprInstruction,
    InvalidConstantExprAlreadyHasValue,
    InvalidConstantExprNoValue,
    InvalidConstantExprInvalidGlobal,
    # AllocError(AllocError),
    # MemoryError(MemoryError),
    ReaderError,
}

enum MemoryError : U8 {
    OutOfBounds,
    OutOfMemory,
    AllocationFailed,
    PageTooSmall,
}

enum AllocError : U8 {
    @ Not enough pages could be allocated to accommodate this allocation
    OutOfMemory,

    @ Page was too small to fit this allocation
    PageTooSmall,

    @ A generic allocation failure
    AllocationFailed,
}

enum IrReaderErrorKind : U8 {
    InvalidAddress,
    InvalidOpcode,
    InvalidType,
    InvalidCallType,
}

struct IrReaderError {
    kind: IrReaderErrorKind
    value: U8
}

state machine SequencerStateMachine {
    ####################
    # guards           #
    ####################
    @ return true if there is a function invocation pending to be set up on entry to IDLE.
    @ the pending FunctionInvoke is held in a component member var
    guard pendingInvoke

    @ return true if the freshly loaded module declared a Wasm start function
    guard hasStartFunction

    ####################
    # signals          #
    ####################
    @ called on CANCEL cmd. raised in all states except IDLE
    signal cmd_CANCEL

    @ generic entry of a state
    signal entered
    @ requests that a function invocation be set up
    signal invoke
    @ requests that a module be loaded into the store
    signal load: ModuleLoad
    @ requests that the store be cleared
    signal clearStore

    @ The program has completed
    signal interpreterFinished

    @ The program has been aborted
    signal interpreterTrap: TrapReason

    @ No more fuel (ran to instruction bound)
    signal interpreterOutOfFuel

    @ An instruction or host function has requested the interpreter to pause
    signal interpreterPause

    @ A signal to put the interpreter to sleep until a given time
    signal interpreterSleep: Fw.Time

    @ Failed to read an instruction from memory
    signal interpreterReaderError: IrReaderErrorKind

    @ raised when a module failed to decode/validate while loading
    signal loadFailure: LoadFailure
    @ raised when a module successfully decoded/validated while loading
    signal loadSuccess

    ####################
    # actions          #
    ####################
    @ simply raises the "entered" signal
    action signalEntered
    @ sets up a function invocation on the interpreter state from the pending FunctionInvoke member var
    action invoke
    @ stores the module load request for the LOADING state
    action setLoad: ModuleLoad
    @ streams, decodes and validates the module into the store
    action load
    @ clears the interpreter store of all modules, tables and memories
    action clearStore
    @ reports the module validation failure as an event
    action reportValidationFailure
    @ responds to the calling command with VALIDATION_ERROR
    action sendCmdResponse_VALIDATION_ERROR
    @ pushes the freshly loaded/validated module into the store
    action pushModuleToStore

    initial enter IDLE

    @ Interpreter is ready to load or run a Wasm function
    state IDLE {
        entry do { signalEntered }

        on entered if pendingInvoke do { invoke } enter RUNNING.READY
        on clearStore do { clearStore }
        on load do { setLoad } enter LOADING
        on invoke do { invoke } enter RUNNING.READY
    }

    @ Interpreter is loading a Wasm module into the store
    state LOADING {
        entry do { load }

        on loadFailure do { reportValidationFailure, sendCmdResponse_VALIDATION_ERROR } enter IDLE
        on loadSuccess do { pushModuleToStore } enter INITIALIZE

        choice INITIALIZE {
            if hasStartFunction enter RUNNING else enter IDLE
        }
    }

    ###############
    # runtime     #
    ###############

    @ spins the interpreter loop, executing up to a bounded number of instructions
    action spin
    @ reports that the interpreter ran to completion
    action report_seqSucceeded
    @ reports that the sequence was cancelled
    action report_seqCancelled
    @ reports that the sequence failed to execute successfully
    action report_seqFailed
    @ reports that the interpreter trapped, with the trap reason
    action report_seqTrap
    @ reports that the interpreter failed to read an instruction from memory
    action report_seqReadError
    @ reports that execution was paused at a breakpoint
    action report_seqBroken
    @ responds to the calling command with OK
    action sendCmdResponse_OK
    @ responds to the calling command with EXECUTION_ERROR
    action sendCmdResponse_EXECUTION_ERROR
    @ stores the requested wake time for the SLEEPING state
    action set_sleepTime
    @ checks if the current statement has timed out
    action checkStatementTimeout
    @ checks if the sequencer should wake from sleep
    action checkShouldWake
    @ sets the "break before next line" flag to true
    action setBreakBeforeNextLine
    @ sets the "break before next line" flag to false
    action clearBreakBeforeNextLine

    @ raised whenever the checkTimers port is called
    signal checkTimersIn
    @ raised when we are done sleeping
    signal result_checkShouldWake_wakeup
    @ raised when we should keep sleeping
    signal result_checkShouldWake_keepSleeping
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time base or context
    signal result_timeOpFailed
    @ raised when the statement times out, according to the timeout parameter
    signal result_checkStatementTimeout_statementTimeout
    @ raised when the statement has not timed out yet
    signal result_checkStatementTimeout_noTimeout
    @ called when a paused host statement (F´ command) successfully executed. only raised in RUNNING.AWAITING_RESPONSE
    signal stmtResponse_success
    @ called when a paused host statement (F´ command) unsuccessfully executed. only raised in RUNNING.AWAITING_RESPONSE
    signal stmtResponse_failure
    @ called when an unexpected or incorrect statement response comes in. only raised in the RUNNING state
    signal stmtResponse_unexpected
    @ called when the statement is telling the sequencer to await a later stmt response
    signal stmtResponse_keepWaiting

    @ called on CONTINUE cmd. only raised in RUNNING.PAUSED state
    signal cmd_CONTINUE
    @ called on BREAK cmd. only raised in RUNNING state
    signal cmd_BREAK

    @ return true if execution should break before spinning the next statement
    guard shouldBreak

    @ sequencer is executing the loaded/invoked Wasm program
    state RUNNING {
        initial enter BREAK_CHECK

        @ check whether to pause execution before spinning the next statement
        choice BREAK_CHECK {
            if shouldBreak do { report_seqBroken } enter PAUSED else enter SPINNING
        }

        @ a function has been invoked; the program and stack are initialized and ready to
        @ spin the interpreter loop. must be started via the CONTINUE command
        state READY {
            on cmd_CONTINUE enter SPINNING
        }

        @ sequencer is spinning the interpreter loop
        state SPINNING {
            entry do { spin }
            # -->
            on interpreterFinished do { report_seqSucceeded, sendCmdResponse_OK } enter IDLE
            on interpreterTrap do { report_seqTrap, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            on interpreterReaderError do { report_seqReadError, sendCmdResponse_EXECUTION_ERROR, clearStore } enter IDLE
            on interpreterOutOfFuel enter BREAK_CHECK
            on interpreterPause enter AWAITING_RESPONSE
            on interpreterSleep do { set_sleepTime } enter SLEEPING

        }

        @ a host function paused the interpreter to dispatch an F´ command;
        @ waiting for the command response to come back in
        state AWAITING_RESPONSE {
            on checkTimersIn do { checkStatementTimeout }
            # -->
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # result_checkStatementTimeout_noTimeout not handled, no need

            on stmtResponse_success enter BREAK_CHECK
            on stmtResponse_failure do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # stmtResponse_keepWaiting not handled, we're already in the awaiting state
        }

        @ sequencer is not taking any action, waiting for a time in the future to continue
        state SLEEPING {
            on checkTimersIn do { checkShouldWake, checkStatementTimeout }
            # -->
            on result_checkShouldWake_wakeup enter BREAK_CHECK
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE
            # if result_checkShouldWake_keepSleeping is raised, stay asleep
            # if result_checkStatementTimeout_noTimeout is raised, stay asleep
        }

        @ execution was broken via the BREAK command.
        @ sequencer is not taking any action, or allowing timeouts.
        @ must be exited via command, either CANCEL or CONTINUE
        state PAUSED {
            entry do { clearBreakBeforeNextLine }
            # -->
            on cmd_CONTINUE enter SPINNING
        }

        @ fail the sequence if we got an unexpected statement response while running it.
        @ the definition of unexpected is complicated, see WasmSequencer::cmdResponseIn_handler.
        @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
        @ sometimes, we can produce responses in strange situations nominally by cancelling and
        @ quickly running sequences. this signal does not cover such cases.
        on stmtResponse_unexpected do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE

        ####################
        # commands         #
        ####################
        on cmd_CANCEL do { report_seqCancelled, sendCmdResponse_EXECUTION_ERROR } enter IDLE
        on cmd_BREAK do { setBreakBeforeNextLine }
    }
}
