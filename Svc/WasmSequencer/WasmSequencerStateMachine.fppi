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
    UNREACHABLE
    @ A host function has noted an unrecoverable failure
    HOST
    @ Integer or floating point division by zero
    DIVIDE_BY_ZERO
    @ An indirect call tried to map to a table function out of range
    INVALID_TABLE_INDEX
    @ The function type in an indirect call does not match the function pointer's type
    INVALID_TABLE_FUNCTION_TYPE
    @ An indirect call tried to map to a table function out of range
    UNINTIIALIZED_TABLE_ELEMENT
    @ An imported global could not be read
    GLOBAL_GET_FAILED
    @ An imported global could not be set
    GLOBAL_SET_FAILED
    @ A memory operation is out of bounds
    OUT_OF_MEMORY
    @ memory.grow failed because a host function has taken ownership of a memory
    MEMORY_REF_NOT_UNIQUE
    @ A memory operation is out of bounds
    MEMORY_OUT_OF_BOUNDS
    @ Ran out of stack space
    STACK_OVERFLOW
    @ Attempting to convert Inf to integer
    UNREPRESENTABLE_RESULT
    @ Signed division causes integer overflow
    INTEGER_OVERFLOW
    @ Attempting to convert NaN to integer
    BAD_CONVERSION_TO_INTEGER
}

enum AllocError: U8 {
    @ Not enough pages could be allocated to accommodate this allocation
    OUT_OF_MEMORY

    @ Page was too small to fit this allocation
    PAGE_TOO_SMALL

    @ A generic allocation failure
    ALLOCATION_FAILED
}

type ModuleIdx = U8

state machine SequencerStateMachine {
    initial enter IDLE

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

    @ return true if there is a pending sleep timer
    guard pendingTimer

    @ a host function is waiting to be processed
    guard pendingHostFunction

    @ return true if the last module load succeeded
    guard moduleLoadSucceeded

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

    @ Interpreter does not have any modules loaded or a store initialized
    state IDLE {
        entry do { resetStore }

        on cmd_CANCEL enter IDLE
        on cmd_RUN do { pendRun } enter LOADING
        on cmd_LOAD do { pendLoad } enter LOADING
    }

    @ Interpreter has modules loaded into the store and is ready to execute
    state READY {
        on cmd_CANCEL enter IDLE
        on cmd_RUN do { resetStore, pendRun } enter LOADING
        on cmd_LOAD do { pendLoad } enter LOADING
        on cmd_INVOKE do { invokeMain } enter INVOKE_CHECK
    }

    @ Interpreter is loading a Wasm module into the store
    state LOADING {
        entry do { signalEntered }
        on entered do { load } enter LOAD_CHECK

        choice LOAD_CHECK {
            if moduleLoadSucceeded enter STARTING else do {
                reportLoadFailure
                sendLoadCmdResponse_EXECUTION_ERROR
                sendRunCmdResponse_EXECUTION_ERROR
                # A failing module load must reset because there could be lingering allocations
                # and we explicitely do not allow fragmentation.
            } enter IDLE
        }
    }

    @ A module finished loading and the start function needs to be invoked/run
    state STARTING {
        entry do { signalEntered }
        on entered do { invokeStartOfLastModule }

        # Start finished might be either start=host function or start=None
        on startFinished do { sendLoadCmdResponse_OK } enter RUN_CHECK
        on startInvoked enter RUNNING
        on startError do {
            reportInvokeFailure
            sendLoadCmdResponse_EXECUTION_ERROR
            sendRunCmdResponse_EXECUTION_ERROR
        } enter IDLE
    }

    @ A module finished loading; decide whether to invoke a pending function or wait
    choice RUN_CHECK {
        if pendingRun do {
            invokeMainOfLastModule
        } enter INVOKE_CHECK else enter READY
    }

    choice INVOKE_CHECK {
        if invokeFailed do {
            reportInvokeFailure
            # NO_BLOCK RUN parks its response in the load-cmd slot; BLOCK RUN/INVOKE
            # park in the finish-cmd queue. Respond to both so no caller hangs.
            sendLoadCmdResponse_EXECUTION_ERROR
            sendRunCmdResponse_EXECUTION_ERROR
        } enter READY else enter RUNNING
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
    action report_seqTrap: TrapReason
    @ reports that execution was paused at a breakpoint
    action report_seqPaused
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
    @ clears the pending host function port call
    action clearPendingHostFunction

    @ raised whenever the checkTimers port is called
    signal checkTimersIn
    @ raised when an operation could not be performed on a Fw::Time object due to a
    @ mismatched time base or context
    signal result_timeOpFailed
    @ raised when the statement times out, according to the timeout parameter
    signal result_checkStatementTimeout_statementTimeout
    @ called when a paused host statement (F´ command) successfully executed. only raised in RUNNING.AWAITING_RESPONSE
    signal stmtResponse_success
    @ called when a paused host statement (F´ command) unsuccessfully executed. only raised in RUNNING.AWAITING_RESPONSE
    signal stmtResponse_failure
    @ called when an unexpected or incorrect statement response comes in. only raised in the RUNNING state
    signal stmtResponse_unexpected

    @ The program has completed
    signal interpreterFinished

    @ The program has been aborted
    signal interpreterTrap: TrapReason

    @ No more fuel (ran to instruction bound)
    signal interpreterOutOfFuel

    @ An asynchronous host function has been called and has requested the interpreter pause
    signal interpreterPause

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
            on interpreterOutOfFuel enter PAUSE_CHECK
            on interpreterPause enter HOST_FUNCTION

            on interpreterTrap do {
                report_seqTrap
                sendRunCmdResponse_EXECUTION_ERROR
            } enter IDLE

            # Check if there is a pending
            choice HOST_FUNCTION {
                if pendingPause enter PAUSED else do {
                    dispatchPendingHostFunction
                } enter AWAITING_RESPONSE
            }

            choice FINISHED {
                # Check if we are running the start function
                if pendingRun enter RUN_CHECK else do {
                    # We are not executing the start function, report success to the command
                    # that requested the interpreter to spin
                    report_seqSucceeded
                    clearPause
                    sendRunCmdResponse_OK
                } enter READY
            }
        }

        @ a host function paused the interpreter to dispatch an F´ command;
        @ waiting for the command response to come back in
        state AWAITING_RESPONSE {
            on checkTimersIn enter CHECK_TIMERS

            choice CHECK_TIMERS {
                if pendingTimer do {
                    checkShouldWake
                    checkStatementTimeout
                } enter AWAITING_RESPONSE else do {
                    checkStatementTimeout
                } enter AWAITING_RESPONSE
            }

            # -->
            on result_checkStatementTimeout_statementTimeout do {
                clearPendingHostFunction
                report_seqFailed
                sendRunCmdResponse_EXECUTION_ERROR
            } enter IDLE

            # this can occur if the time base/ctx changes, or was invalid from the start
            on result_timeOpFailed do {
                clearPendingHostFunction
                report_seqFailed
                sendRunCmdResponse_EXECUTION_ERROR
            } enter IDLE

            on stmtResponse_success do {
                clearPendingHostFunction
            } enter PAUSE_CHECK
            on stmtResponse_failure do {
                clearPendingHostFunction
                report_seqFailed
                sendRunCmdResponse_EXECUTION_ERROR
            } enter IDLE
        }

        state PAUSED {
            entry do { clearPause, report_seqPaused }
            # -->
            on cmd_CONTINUE enter RESUME

            choice RESUME {
                if pendingHostFunction do {
                    dispatchPendingHostFunction
                } enter AWAITING_RESPONSE else enter SPINNING
            }
        }

        @ fail the sequence if we got an unexpected statement response while running it.
        @ the definition of unexpected is complicated, see WasmSequencer::cmdResponseIn_handler.
        @ in general, it is "unexpected" if there isn't a nominal way of producing this response.
        @ sometimes, we can produce responses in strange situations nominally by cancelling and
        @ quickly running sequences. this signal does not cover such cases.
        on stmtResponse_unexpected do {
            report_seqFailed
            sendRunCmdResponse_EXECUTION_ERROR
        } enter IDLE

        ####################
        # commands         #
        ####################
        on cmd_CANCEL do {
            clearPause
            clearPendingHostFunction
            report_seqCancelled
            sendRunCmdResponse_EXECUTION_ERROR
        } enter IDLE

        on cmd_PAUSE do { pendPause }
    }
}
