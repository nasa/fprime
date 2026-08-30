type ModuleIdx = U8

enum SignalSource : U8 {
    COMMAND_RUN
    COMMAND_INVOKE
    COMMAND_LOAD
    PORT_RUN
    PORT_INVOKE
}

@ A struct holding the context needed to reply to a command request
struct CommandRequest {
    $opcode: FwOpcodeType  @< opcode to reply to
    cmdSeq: U32            @< command sequence number to reply with
}

struct LoadRequest {
    fileName: string size FileNameStringSize
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE
    args: Svc.SeqArgs
    context: RequestContext
}

struct InvokeRequest {
    moduleName: string size Wasm.MODULE_NAME_STRING_SIZE
    args: Svc.SeqArgs
    context: RequestContext
}

@ A struct holding information about where an asynchronous request originated
@ and information about how to reply to the request
struct RequestContext {
    source: SignalSource    @< The originating signal source
    cmdCtx: CommandRequest  @< If 'source' == COMMAND this holds the opcode/cmdSeq
    $block: BlockState      @< Should we reply after we initialize or block until we finish executing
    moduleIdx: ModuleIdx    @< Module index to invoke start/main function, filled in for LOAD/RUN
}

@ diagram-layout engine=elk direction=LR cycleBreaking=MODEL_ORDER considerModelOrder=NODES_AND_EDGES nodePlacement=BRANDES_KOEPF nodeSpacing=60 rankSpacing=60
state machine ControllerStateMachine {
    initial enter IDLE

    ###############
    # Commands    #
    ###############

    @ Signal sent from a command to execute a Wasm module
    @ Valid while in the IDLE or READY state
    signal run: LoadRequest

    @ Signal sent from command to load a Wasm module but not execute it
    signal load: LoadRequest

    @ Signal sent from a command to invoke main on an already-loaded module
    @ Only valid while in the READY state
    signal invoke: InvokeRequest

    @ Action to save the invoke arguments and emit an 'invoked' signal
    action processInvoke: InvokeRequest

    @ Signal emitted by processInvoke when the requested module was resolved successfully
    signal invoked: RequestContext

    @ Signal emitted by processInvoke when the requested module could not be
    @ resolved. Returns the controller to READY (the store is still valid).
    signal invokeFailed: RequestContext

    @ Called on CANCEL cmd. Raised in LOADING, READY and RUNNING modes
    signal cancel

    @ Latch a cancel that arrives while a sequence is loading/resolving
    action setCancelRequested

    @ Acknowledge/clear any pending cancel
    action clearCancelRequested

    @ True if a cancel was latched since the controller last came to rest.
    guard cancelRequested

    @ Abort a load/invoke that was cancelled before the engine ran
    action cancelPendingRequest: RequestContext

    @ Responds to $block == NO_BLOCK requests with OK
    action respond_noblock_OK: RequestContext

    @ Responds to request with EXECUTION_ERROR
    action respond_ERROR: RequestContext

    @ Increment the SequencesFailed telemetry counter
    action incrementSequenceFailure

    @ Responds to request with BUSY
    @ Emit an event to say why we are rejecting this request in the current state
    action respondInvoke_BUSY: InvokeRequest

    @ Responds to request with EXECUTION_ERROR
    @ Emit an event to say why we are rejecting this request in the current state
    action respondInvoke_ERROR: InvokeRequest

    @ Responds to request with BUSY
    @ Emit an event to say why we are rejecting this request in the current state
    action respondLoad_BUSY: LoadRequest

    @ Responds to $block == BLOCK requests with OK
    @ Responds to any pending WAIT requests with OK
    action respond_block_OK: RequestContext

    @ Responds to $block == BLOCK requests with ERROR.
    @ Responds to any pending WAIT requests with ERROR
    action respond_block_ERROR: RequestContext

    ###############
    # Loading     #
    ###############

    @ Load a module into the store.
    @ Emits `loadSucceded` or `loadFailed` depending on result
    action load: LoadRequest

    @ Signal emited inside of the load action when a module load succeeds
    signal loadSucceeded: RequestContext

    @ Signal emited inside the laod action when module load fails
    signal loadFailed: RequestContext

    ###############
    # Start       #
    ###############

    @ return true if this module has a start function
    guard moduleHasStart: RequestContext

    @ Invoke the start function on a loaded module
    action invokeStart: RequestContext

    @ return true if this module has a valid main function ([] -> i32)
    guard moduleHasValidMain: RequestContext

    @ return true if invokeStatus == SPACEWASM_OK. This flag is set as a result of invokeStart/invokeMain
    guard invokeSucceeded: RequestContext

    @ Invoke the main function for module in context
    action invokeMain: RequestContext

    @ Emit an event noting that a given module has no [valid] main function
    action reportModuleInvalidMain: RequestContext

    @ Emit an event noting why the invocation of a module main failed
    action reportModuleMainInvokeFailed: RequestContext

    @ Emit an event noting why the invocation of a module start failed
    action reportModuleStartInvokeFailed: RequestContext

    @ Reports that a module started running it's main function
    action reportModuleStarted: RequestContext

    @ Emit an event noting a module succeeded during execution. Increment telemetry counters.
    action reportModuleSucceeded: RequestContext

    @ Emit an event noting a module failed during execution of start. Increment telemetry counters.
    action reportModuleStartFailed: RequestContext

    @ Emit an event noting a module failed during execution. Increment telemetry counters.
    action reportModuleMainFailed: RequestContext

    ###############
    # Engine      #
    ###############

    @ Create a new store with N_MODULES (parameter) modules
    action resetStore

    @ Send a signal to the engine state machine to begin running
    @ The engine will asynchronously reply with the `engineFinished` signal
    action runEngine: RequestContext

    @ Signal to the controller that the interpreter finished executing
    @ Request context will be sourced from the last `setContext` call
    signal engineFinished: RequestContext

    @ Check if the last engine execution finished executing successfully
    guard interpreterSucceeded

    @ Interpreter does not have any modules loaded or a store initialized
    state IDLE {
        entry do { resetStore, clearCancelRequested }

        on cancel enter IDLE
        on run do { load } enter LOADING_TO_RUN
        on load do { load } enter LOADING_TO_READY
        on invoke do { respondInvoke_ERROR }
    }

    @ Interpreter has modules loaded into the store and is ready to execute
    state READY {
        entry do { clearCancelRequested }

        on cancel enter IDLE
        on run do { resetStore, load } enter LOADING_TO_RUN
        on load do { load } enter LOADING_TO_READY

        on invoke do { processInvoke } enter INVOKING
    }

    state INVOKING {
        on invoked enter INVOKE_CANCEL_CHECK
        on invokeFailed do {
            incrementSequenceFailure
            respond_ERROR
        } enter READY
        on cancel do { setCancelRequested }
        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }

    @ Honor a cancel latched while resolving the invoke, before running main
    choice INVOKE_CANCEL_CHECK {
        if cancelRequested do {
            cancelPendingRequest
        } enter IDLE else enter MAIN_CHECK
    }

    @ Make sure the module we want to run has a main function
    choice MAIN_CHECK {
        if moduleHasValidMain do {
            invokeMain
        } enter MAIN_INVOKE_CHECK else do {
            reportModuleInvalidMain
            incrementSequenceFailure
            respond_ERROR
        } enter READY
    }

    choice MAIN_INVOKE_CHECK {
        if invokeSucceeded do {
            reportModuleStarted
            respond_noblock_OK
            runEngine
        } enter RUNNING_MAIN else do {
            reportModuleMainInvokeFailed
            incrementSequenceFailure
            respond_ERROR
        } enter IDLE
    }

    state LOADING_TO_READY {
        on loadSucceeded enter LOAD_READY_CANCEL_CHECK
        on loadFailed do { incrementSequenceFailure, respond_ERROR } enter IDLE
        on cancel do { setCancelRequested }

        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }

    @ Honor a cancel latched while loading, before running the start function
    choice LOAD_READY_CANCEL_CHECK {
        if cancelRequested do {
            cancelPendingRequest
        } enter IDLE else enter START_CHECK
    }

    choice START_CHECK {
        if moduleHasStart do {
            invokeStart
        } enter START_INVOKE_CHECK else do { respond_noblock_OK } enter READY
    }

    choice START_INVOKE_CHECK {
        if invokeSucceeded do {
            runEngine
        } enter RUNNING_START else do {
            reportModuleStartInvokeFailed
            incrementSequenceFailure
            respond_ERROR
        } enter IDLE
    }

    state RUNNING_START {
        on engineFinished enter INTERPRETER_STATUS

        choice INTERPRETER_STATUS {
            if interpreterSucceeded do {
                respond_noblock_OK
            } enter READY else do {
                # reportModuleStartFailed increments SequencesFailed/SequencesCancelled
                # Do not incrementSequenceFailure
                reportModuleStartFailed
                respond_ERROR
            } enter IDLE
        }

        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }

    state LOADING_TO_RUN {
        on loadSucceeded enter LOAD_RUN_CANCEL_CHECK
        on loadFailed do { incrementSequenceFailure, respond_ERROR } enter IDLE
        on cancel do { setCancelRequested }

        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }

    @ Honor a cancel latched while loading, before starting/running the module
    choice LOAD_RUN_CANCEL_CHECK {
        if cancelRequested do {
            cancelPendingRequest
        } enter IDLE else enter MAIN_CHECK_PENDING_START_AND_MAIN
    }

    @ Make sure the module we want to run has a main function
    choice MAIN_CHECK_PENDING_START_AND_MAIN {
        if moduleHasValidMain enter START_CHECK_PENDING_MAIN else do {
            reportModuleInvalidMain
            incrementSequenceFailure
            respond_ERROR
            # The module loaded fine; it just has no runnable main. Stay in READY
            # (the store is valid) rather than tearing everything down
        } enter READY
    }

    @ Run the start function (if there is one) before running the main function
    choice START_CHECK_PENDING_MAIN {
        if moduleHasStart do {
            invokeStart
        } enter START_INVOKE_CHECK_PENDING_CHAIN else do {
            invokeMain
        } enter MAIN_INVOKE_CHECK
    }

    choice START_INVOKE_CHECK_PENDING_CHAIN {
        if invokeSucceeded do {
            runEngine
        } enter RUNNING_START_PENDING_MAIN else do {
            reportModuleStartInvokeFailed
            incrementSequenceFailure
            # The RUN has not been answered yet (respond_noblock_OK is only reached after
            # the start completes and main is invoked), so respond regardless of block.
            respond_ERROR
        } enter IDLE
    }

    @ Engine is running a module's start function before running the main function
    state RUNNING_START_PENDING_MAIN {
        on engineFinished enter INTERPRETER_STATUS
        on cancel do { setCancelRequested }

        @ Once the start function finishes successfully we still owe the caller a
        @ main invocation (this is the RUN-with-start path). Chain into invokeMain
        @ rather than settling in READY -- unless a cancel was latched in the brief
        @ interpreter-idle gap between the start finishing and main being invoked.
        choice INTERPRETER_STATUS {
            if interpreterSucceeded enter START_MAIN_CANCEL_CHECK else do {
                reportModuleStartFailed
                # The RUN has not been answered yet (respond_noblock_OK is only reached
                # after the start completes and main is invoked), so respond regardless
                # of block. reportModuleStartFailed increments SequencesFailed/SequencesCancelled
                # Do not incrementSequenceFailure
                respond_ERROR
            } enter IDLE
        }

        @ Honor a cancel latched during the start->main gap, before invoking main
        choice START_MAIN_CANCEL_CHECK {
            if cancelRequested do {
                cancelPendingRequest
            } enter IDLE else do { invokeMain } enter MAIN_INVOKE_CHECK
        }

        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }

    @ The engine is running a module's main function
    state RUNNING_MAIN {
        on engineFinished enter INTERPRETER_STATUS

        choice INTERPRETER_STATUS {
            if interpreterSucceeded do {
                reportModuleSucceeded
                respond_block_OK
            } enter READY else do {
                reportModuleMainFailed
                respond_block_ERROR
            } enter IDLE
        }

        on run do { respondLoad_BUSY }
        on load do { respondLoad_BUSY }
        on invoke do { respondInvoke_BUSY }
    }
}
