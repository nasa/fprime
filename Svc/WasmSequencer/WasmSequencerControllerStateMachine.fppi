type ModuleIdx = U8

struct ModuleLoad {
  fileName: string size FileNameStringSize
  moduleName: string size 16
}

@ diagram-layout engine=elk direction=TB cycleBreaking=MODEL_ORDER considerModelOrder=NODES_AND_EDGES nodePlacement=BRANDES_KOEPF nodeSpacing=60 rankSpacing=60
state machine ControllerStateMachine {
  initial enter IDLE

  ###############
  # Commands    #
  ###############

  @ Signal sent from a command to execute a Wasm module
  @ Valid while in the IDLE or READY state
  signal cmd_RUN: ModuleLoad

  @ Signal sent from command to load a Wasm module but not execute it
  signal cmd_LOAD: ModuleLoad

  @ Signal sent from a command to invoke main on an already-loaded module
  @ Only valid while in the READY state
  signal cmd_INVOKE: ModuleIdx

  @ Called on CANCEL cmd. Raised in LOADING, READY and RUNNING modes
  signal cmd_CANCEL

  @ responds to any command waiting on load with OK
  action loadCmd_OK

  @ responds to any command waiting on load with EXECUTION_ERROR
  action loadCmd_ERROR

  @ responds to any command waiting on load with OK
  action runCmd_OK

  @ responds to any command waiting on load with EXECUTION_ERROR
  action runCmd_ERROR

  @ report that a RUN has begun on seqStartOut, and mark a run active so its
  @ terminal outcome (runCmd_OK / runCmd_ERROR) reports on seqDoneOut
  action reportSeqStarted: ModuleLoad

  ###############
  # Loading     #
  ###############

  @ Load a pending module request
  action load: ModuleLoad

  @ Signal emited inside of the load action when a module load succeeds
  signal loadSucceeded: ModuleIdx

  @ Signal emited inside the laod action when module load fails
  signal loadFailed: Status

  @ Emit an event to denote failed module load
  action reportModuleLoadFailed: Status

  ###############
  # Start       #
  ###############

  @ return true if this module has a start function
  guard moduleHasStart: ModuleIdx

  @ Invoke the start function on a loaded module
  action invokeStart: ModuleIdx

  @ return true if this module has a valid main function ([] -> i32)
  guard moduleHasValidMain: ModuleIdx

  @ return true if invokeStatus == SPACEWASM_OK. This flag is set as a result of invokeStart/invokeMain
  guard invokeSucceeded

  @ Invoke the main function on a module given its index
  action invokeMain: ModuleIdx

  @ Invoke the main function on the module whose start function just ran. Used on
  @ the RUN-with-start path where the engineFinished signal carries no module
  @ index, so the index recorded by invokeStart is reused.
  action invokeMainPending

  @ Emit an event noting that a given module has no [valid] main function
  action reportModuleInvalidMain: ModuleIdx

  @ Emit an event noting why the invocation of a module main failed
  action reportModuleMainInvokeFailed

  @ Emit an event noting why the invocation of a module start failed
  action reportModuleStartInvokeFailed

  ###############
  # Engine      #
  ###############

  @ Create a new store with N_MODULES (parameter) modules
  action resetStore

  @ Send a signal to the engine state machine to begin running
  action runEngine

  @ Signal
  signal engineFinished

  guard interpreterSucceeded

  @ Emit an event noting a module succeeded during execution. Increment telemetry counters
  action reportModuleSucceeded

  @ Emit an event noting a module failed during execution. Increment telemetry counters
  action reportModuleFailed

  @ Interpreter does not have any modules loaded or a store initialized
  state IDLE {
    entry do { resetStore }

    on cmd_CANCEL enter IDLE
    on cmd_RUN do { reportSeqStarted, load } enter LOADING_TO_RUN
    on cmd_LOAD do { load } enter LOADING_TO_READY
  }

  @ Interpreter has modules loaded into the store and is ready to execute
  state READY {
    on cmd_CANCEL enter IDLE
    on cmd_RUN do { reportSeqStarted, resetStore, load } enter LOADING_TO_RUN
    on cmd_LOAD do { load } enter LOADING_TO_READY
    on cmd_INVOKE enter MAIN_CHECK
  }

  @ Make sure the module we want to run has a main function
  choice MAIN_CHECK {
    if moduleHasValidMain do { invokeMain } enter MAIN_INVOKE_CHECK else do {
      reportModuleInvalidMain
      loadCmd_ERROR
      runCmd_ERROR
      # enter ready since we don't want an `INVOKE` to clear the store
      # Invoking a module that has no main is harmless
    } enter READY
  }

  choice MAIN_INVOKE_CHECK {
    if invokeSucceeded do {
      loadCmd_OK
    } enter RUNNING_MAIN else do {
      reportModuleMainInvokeFailed
      loadCmd_ERROR
      runCmd_ERROR
    } enter IDLE
  }

  state LOADING_TO_READY {
    on loadSucceeded enter START_CHECK
    on loadFailed do { reportModuleLoadFailed, loadCmd_ERROR } enter IDLE
  }

  choice START_CHECK {
    if moduleHasStart do {
      invokeStart
    } enter START_INVOKE_CHECK else do { loadCmd_OK } enter READY
  }

  choice START_INVOKE_CHECK {
    if invokeSucceeded enter RUNNING_START else do {
      reportModuleStartInvokeFailed
      loadCmd_ERROR
    } enter IDLE
  }

  state RUNNING_START {
    entry do { runEngine }

    on engineFinished enter INTERPRETER_STATUS

    choice INTERPRETER_STATUS {
      if interpreterSucceeded do {
        loadCmd_OK
      } enter READY else do { reportModuleFailed, loadCmd_ERROR } enter IDLE
    }
  }

  state LOADING_TO_RUN {
    on loadSucceeded enter MAIN_CHECK_PENDING_START_AND_MAIN
    on loadFailed do {
      reportModuleLoadFailed
      loadCmd_ERROR
      runCmd_ERROR
    } enter IDLE
  }

  @ Make sure the module we want to run has a main function
  choice MAIN_CHECK_PENDING_START_AND_MAIN {
    if moduleHasValidMain enter START_CHECK_PENDING_MAIN else do {
      reportModuleInvalidMain
      runCmd_ERROR
      loadCmd_ERROR
      # The module loaded fine; it just has no runnable main. Stay in READY
      # (the store is valid) rather than tearing everything down
    } enter READY
  }

  @ Run the start function (if there is one) before running the main function
  choice START_CHECK_PENDING_MAIN {
    if moduleHasStart do {
      invokeStart
    } enter START_INVOKE_CHECK_PENDING_CHAIN else do {
      loadCmd_OK
      invokeMain
    } enter MAIN_INVOKE_CHECK
  }

  choice START_INVOKE_CHECK_PENDING_CHAIN {
    if invokeSucceeded enter RUNNING_START_PENDING_MAIN else do {
      reportModuleStartInvokeFailed
      runCmd_ERROR
      loadCmd_ERROR
    } enter IDLE
  }

  @ Engine is running a module's start function before running the main function
  state RUNNING_START_PENDING_MAIN {
    entry do { runEngine }

    on engineFinished enter INTERPRETER_STATUS

    @ Once the start function finishes successfully we still owe the caller a
    @ main invocation (this is the RUN-with-start path). Chain into invokeMain
    @ rather than settling in READY.
    choice INTERPRETER_STATUS {
      if interpreterSucceeded do {
        invokeMainPending
      } enter MAIN_INVOKE_CHECK else do {
        reportModuleFailed
        loadCmd_ERROR
        runCmd_ERROR
      } enter IDLE
    }
  }

  @ The engine is running a module's main function
  state RUNNING_MAIN {
    entry do { runEngine }

    on engineFinished enter INTERPRETER_STATUS

    choice INTERPRETER_STATUS {
      if interpreterSucceeded do {
        reportModuleSucceeded
        loadCmd_OK
        runCmd_OK
      } enter READY else do {
        reportModuleFailed
        loadCmd_ERROR
        runCmd_ERROR
      } enter IDLE
    }
  }
}
