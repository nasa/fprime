struct SequenceExecutionArgs {
    filePath: string size FileNameStringSize
    buffer: Svc.SeqArgs
}

struct ModuleLoad {
    fileName: string size FileNameStringSize
    moduleName: string size 16
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

enum AllocError : U8 {
    @ Not enough pages could be allocated to accommodate this allocation
    OutOfMemory,

    @ Page was too small to fit this allocation
    PageTooSmall,

    @ A generic allocation failure
    AllocationFailed,
}

type ModuleIdx = U8

state machine SequencerStateMachine {
    initial enter RESET

    ###############
    # Commands    #
    ###############

    @ Signal sent from a command to execute a Wasm module
    @ Only valid while in the IDLE state
    signal cmd_RUN: SequenceExecutionArgs

    @ Signal sent from command to load a Wasm module but not execute it
    signal cmd_LOAD: ModuleLoad

    @ called on CONTINUE cmd. only raised in RUNNING.PAUSED state
    signal cmd_CONTINUE

    @ called on PAUSE cmd. only raised in RUNNING state
    signal cmd_PAUSE

    signal cmd_INVOKE: ModuleIdx

    @ Called on CANCEL cmd. Raised in LOADING, READY and RUNNING modes
    signal cmd_CANCEL

    ###############
    # Guards      #
    ###############

    @ return true if the module is pending to execute after loading
    guard pendingRun

    @ return true if an invoke succeeded and the interpreter can now run
    guard invokeFailed

    @ return true if execution should pause before spinning the next statement
    guard pendingPause

    @ a host function is waiting to be processed
    guard pendingHostFunction

    @ return true if the last module load succeeded
    guard moduleLoadSucceeded

    guard alwaysTrue

    ###############
    # Actions     #
    ###############

    @ Raises the "entered" signal
    action signalEntered

    @ Create a new store with N_MODULES (parameter) modules
    action resetStore

    @ Invoke the main function of last module loaded into the store
    action invokeMainOfLastModule

    @ Invoke the main function on a module given its index
    action invokeMain: ModuleIdx

    @ Set the Wasm module to load and execute
    @ Sets pendingInvoke
    action pendRun: SequenceExecutionArgs

    @ Set the Wasm module to load but not execute
    action pendLoad: ModuleLoad

    action reportLoadFailure

    action reportInvokeFailure

    @ Load a pending module request
    action load

    @ responds to any command waiting on load with OK
    action sendLoadCmdResponse_OK
    @ responds to any command waiting on load with EXECUTION_ERROR
    action sendLoadCmdResponse_EXECUTION_ERROR

    @ Invoke the start function on a loaded module
    action invokeStartOfLastModule
    signal startFinished
    signal startInvoked
    signal startError

    ###############
    # Signals     #
    ###############

    @ Generic entry of a state
    signal entered

    choice RESET {
        if alwaysTrue do { resetStore } enter IDLE \
        else enter IDLE
    }

    @ Interpreter does not have any modules loaded or a store initialized
    state IDLE {
        on cmd_CANCEL enter RESET
        on cmd_RUN do { resetStore, pendRun } enter LOADING
        on cmd_LOAD do { pendLoad } enter LOADING
        on cmd_INVOKE do { invokeMain } enter INVOKE_CHECK
    }

    @ Interpreter is loading a Wasm module into the store
    state LOADING {
        entry do { signalEntered }
        on entered do { load } enter LOAD_CHECK

        choice LOAD_CHECK {
            if moduleLoadSucceeded enter STARTING \
            else do {
                reportLoadFailure,
                sendLoadCmdResponse_EXECUTION_ERROR,
                sendRunCmdResponse_EXECUTION_ERROR
                # A failing module load must reset because there could be lingering allocations
                # and we explicitely do not allow fragmentation.
            } enter RESET
        }
    }

    @ A module finished loading and the start function needs to be invoked/run
    state STARTING {
        entry do { signalEntered }
        on entered do { invokeStartOfLastModule }

        # Start finished might be either start=host function or start=None
        on startFinished enter RUN_CHECK
        on startInvoked enter RUNNING
        on startError do {
            reportInvokeFailure,
            sendLoadCmdResponse_EXECUTION_ERROR,
            sendRunCmdResponse_EXECUTION_ERROR
        } enter RESET
    }

    @ A module finished loading; decide whether to invoke a pending function or wait
    choice RUN_CHECK {
        if pendingRun do { invokeMainOfLastModule } enter INVOKE_CHECK \
        else do {
            # We are not pending an invoke/run meaning we are running a LOAD/LOAD_NAME
            # Reply to that command
            sendLoadCmdResponse_OK
        } enter IDLE
    }

    choice INVOKE_CHECK {
        if invokeFailed do {
            reportInvokeFailure,
            sendLoadCmdResponse_EXECUTION_ERROR
        } enter IDLE \
        else enter RUNNING
    }

    ###############
    # runtime     #
    ###############

    @ responds to all commands waiting for finish with OK
    action sendRunCmdResponse_OK
    @ responds to all commands waiting for finish with EXECUTION_ERROR
    action sendRunCmdResponse_EXECUTION_ERROR

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
    @ reports that execution was paused at a breakpoint
    action report_seqPaused
    @ stores the requested wake time for the SLEEPING state
    action set_sleepTime
    @ checks if the current statement has timed out
    action checkStatementTimeout
    @ checks if the sequencer should wake from sleep
    action checkShouldWake
    @ sets the pause flag to true
    action pendPause
    @ sets the pause flag to false
    action clearPause
    @ dispatch a host function port call
    action dispatchPendingHostFunction

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

    @ The program has completed
    signal interpreterFinished

    @ The program has been aborted
    signal interpreterTrap: TrapReason

    @ No more fuel (ran to instruction bound)
    signal interpreterOutOfFuel

    @ An asynchronous host function has been called and has requested the interpreter pause
    signal interpreterPause

    @ A signal to put the interpreter to sleep until a given time
    signal interpreterSleep: Fw.Time

    @ sequencer is executing the loaded/invoked Wasm program
    state RUNNING {
        initial enter PAUSE_CHECK

        @ check whether to pause execution before spinning the next statement
        choice PAUSE_CHECK {
            if pendingPause enter PAUSED else enter SPINNING
        }

        @ sequencer is spinning the interpreter loop
        state SPINNING {
            entry do { signalEntered }
            on entered do { spin }
            # -->
            on interpreterFinished enter FINISHED
            on interpreterTrap do { report_seqTrap, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            on interpreterOutOfFuel enter PAUSE_CHECK
            on interpreterPause enter HOST_FUNCTION
            on interpreterSleep do { set_sleepTime } enter SLEEPING

            # Check if there is a pending 
            choice HOST_FUNCTION {
                if pendingPause enter PAUSED \
                else do { dispatchPendingHostFunction } enter AWAITING_RESPONSE
            }

            choice FINISHED {
                # Check if we are running the start function
                if pendingRun enter RUN_CHECK \
                else do {
                    # We are not executing the start function, report success to the command
                    # that requested the interpreter to spin
                    report_seqSucceeded,
                    clearPause,
                    sendRunCmdResponse_OK
                } enter IDLE
            }

        }

        @ a host function paused the interpreter to dispatch an F´ command;
        @ waiting for the command response to come back in
        state AWAITING_RESPONSE {
            on checkTimersIn do { checkStatementTimeout }
            # -->
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            # result_checkStatementTimeout_noTimeout not handled, no need

            on stmtResponse_success enter PAUSE_CHECK
            on stmtResponse_failure do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            # stmtResponse_keepWaiting not handled, we're already in the awaiting state
        }

        @ sequencer is not taking any action, waiting for a time in the future to continue
        state SLEEPING {
            on checkTimersIn do { checkShouldWake, checkStatementTimeout }
            # -->
            on result_checkShouldWake_wakeup enter PAUSE_CHECK
            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            on result_checkStatementTimeout_statementTimeout do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE
            # if result_checkShouldWake_keepSleeping is raised, stay asleep
            # if result_checkStatementTimeout_noTimeout is raised, stay asleep
        }

        state PAUSED {
            entry do { clearPause, report_seqPaused }
            # -->
            on cmd_CONTINUE enter RESUME

            choice RESUME {
                if pendingHostFunction do { dispatchPendingHostFunction } enter AWAITING_RESPONSE \
                else enter SPINNING
            }
        }

        @ fail the sequence if we got an unexpected statement response while running it.
        @ the definition of unexpected is complicated, see WasmSequencer::cmdResponseIn_handler.
        @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
        @ sometimes, we can produce responses in strange situations nominally by cancelling and
        @ quickly running sequences. this signal does not cover such cases.
        on stmtResponse_unexpected do { report_seqFailed, sendRunCmdResponse_EXECUTION_ERROR } enter IDLE

        ####################
        # commands         #
        ####################
        on cmd_CANCEL do { report_seqCancelled, sendRunCmdResponse_EXECUTION_ERROR } enter RESET
        on cmd_PAUSE do { pendPause }
    }
}
