event StoreAllocationFailed(moduleCount: U16, err: AllocError) \
    severity warning high \
    format "Failed to allocate store with {} modules: {}"

event StoreAllocationSucceeded(moduleCount: U16) \
    severity activity low \
    format "Successfully allocated store with {} modules"

@ A command was issued in a state where it is not valid
event InvalidCommand($state: SequencerStateMachine.State) \
    severity warning low \
    format "Command not valid in state {}"

@ Failed to open the requested Wasm module file
event FileOpenError(fileName: string size FileNameStringSize, status: I32) \
    severity warning high \
    format "Failed to open Wasm module file {}: status {}"

@ A module failed to decode/validate while loading
event ModuleLoadFailed(status: I32) \
    severity warning high \
    format "Module load failed: spacewasm status {}"

event TooManyBlockingCommands \
    severity warning high \
    format "Too many commands blocking on sequencer finish"

@ The interpreter ran a program to completion
event SequenceSucceeded() \
    severity activity high \
    format "Wasm program completed successfully"

event CommandTooLarge($size: U32, maxSize: U32) \
    severity warning high \
    format "Command payload size {} larger than host maximum size {}"

# TODO(tumbar) Expose spacewasm_status_t to FPP
event CommandInvalidPointer(code: I32) \
    severity warning high \
    format "Failed to reach command payload from guest: {}"

@ The sequence was cancelled
event SequenceCancelled() \
    severity activity high \
    format "Wasm program cancelled"

@ The sequence failed to execute successfully
event SequenceFailed() \
    severity warning high \
    format "Wasm program failed"

@ The interpreter trapped
event SequenceTrap(reason: TrapReason) \
    severity warning high \
    format "Wasm program trapped: {}"

@ Execution was paused at a breakpoint
event SequenceBroken() \
    severity activity high \
    format "Wasm program paused at breakpoint"

@ The guest program called fprime.exit with a non-zero code
event ProgramExited(code: I32) \
    severity warning high \
    format "Wasm program exited with code {}"

@ The guest program called fprime.panic, or the interpreter itself panicked
event PanicOccurred(code: I32) \
    severity warning high \
    format "Wasm program panicked: code {}"

@ A guest host function could not access guest linear memory
event GuestMemoryError(status: I32) \
    severity warning high \
    format "Guest memory access failed: spacewasm status {}"

# ----------------------------------------------------------------------
# Guest-emitted events (fprime.event host function). One event per F´
# severity; the host function switches on the requested severity id to
# emit the matching one so the event carries the correct FPP severity.
# ----------------------------------------------------------------------

@ A FATAL event emitted by the guest program
event GuestFatal(msg: string size 128) \
    severity fatal \
    format "Guest: {}"

@ A WARNING_HI event emitted by the guest program
event GuestWarningHi(msg: string size 128) \
    severity warning high \
    format "Guest: {}"

@ A WARNING_LO event emitted by the guest program
event GuestWarningLo(msg: string size 128) \
    severity warning low \
    format "Guest: {}"

@ A COMMAND event emitted by the guest program
event GuestCommand(msg: string size 128) \
    severity command \
    format "Guest: {}"

@ An ACTIVITY_HI event emitted by the guest program
event GuestActivityHi(msg: string size 128) \
    severity activity high \
    format "Guest: {}"

@ An ACTIVITY_LO event emitted by the guest program
event GuestActivityLo(msg: string size 128) \
    severity activity low \
    format "Guest: {}"

@ A DIAGNOSTIC event emitted by the guest program
event GuestDiagnostic(msg: string size 128) \
    severity diagnostic \
    format "Guest: {}"
