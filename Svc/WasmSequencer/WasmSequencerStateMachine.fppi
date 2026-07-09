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
    @ return true if the goal state is RUNNING
    # see explanation in FpySequencer::m_goalState
    guard goalStateIs_RUNNING

    guard pendingInvoke: FunctionInvoke

    ####################
    # signals          #
    ####################
    @ called on VALIDATE cmd with the path of the sequence file to validate. only raised in IDLE state
    signal cmd_VALIDATE: SequenceExecutionArgs
    @ called on RUN cmd with the path of the sequence file to run. only raised in IDLE state
    signal cmd_RUN: SequenceExecutionArgs
    @ called on RUN_VALIDATED cmd. only raised in AWAITING_CMD_RUN_VALIDATED state
    signal cmd_RUN_VALIDATED: SequenceExecutionArgs
    @ called on CANCEL cmd. raised in all states except IDLE
    signal cmd_CANCEL

    @ generic entry of a state
    signal entered
    signal invoke
    signal continue
    signal load: ModuleLoad
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

    signal loadFailure: LoadFailure
    signal cancel

    ####################
    # actions          #
    ####################

    action invoke: FunctionInvoke
    action setLoad: ModuleLoad
    action load
    action clearStore

    initial enter IDLE

    @ Interpreter is ready to load or run a Wasm function
    state IDLE {
        entry do { signalEntered }

        on entered if pendingInvoke do { invoke }
        on clearStore do { clearStore }
        on load do { setLoad } enter LOADING
        on invoke do { invoke }
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

    @ Interpreter is ready interpret instructions but in a paused state.
    @ The program and stack are initialized and ready to spin the interpreter loop.
    state PAUSED {
        on continue enter RUNNING
    }

    @ sequencer is executing all statements in the sequence
    state RUNNING {
        initial enter BREAK_CHECK

        @ check whether to pause execution before dispatching the next statement
        choice BREAK_CHECK {
            if shouldBreak do { report_seqBroken } enter PAUSED else enter SPINNING
        }

        @ sequencer is stepping into a single statement and dispatching it
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

        @ a breakpoint was hit or the sequence was broken via command. 
        @ sequencer is not taking any action, or allowing timeouts.
        @ must be exited via command, either CANCEL or CONTINUE
        state PAUSED {
            entry do { signalEntered, clearBreakBeforeNextLine }
            # -->
            on entered if breakOnce do { clearBreakpoint }

            on cmd_CONTINUE enter DISPATCH_STATEMENT
            # a step is really just a continue followed immediately by a break
            on cmd_STEP do { setBreakBeforeNextLine } enter DISPATCH_STATEMENT
        }

        @ fail the sequence if we got an unexpected statement response while running it.
        @ the definition of unexpected is complicated, see FpySequencer::cmdResponseIn_handler.
        @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
        @ sometimes, we can produce responses in strange situations nominally by cancelling and
        @ quickly running sequences. this signal does not cover such cases.
        on stmtResponse_unexpected do { report_seqFailed, sendCmdResponse_EXECUTION_ERROR } enter IDLE

        ####################
        # commands         #
        ####################
        on cmd_CANCEL do { report_seqCancelled, sendCmdResponse_EXECUTION_ERROR } enter IDLE
        on cmd_BREAK do { setBreakBeforeNextLine }
        on cmd_SET_BREAKPOINT do { setBreakpoint }
        on cmd_CLEAR_BREAKPOINT do { clearBreakpoint }
    }
}
