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

@ The interpreter failed to read an instruction from memory
event SequenceReadError() \
    severity warning high \
    format "Wasm interpreter reader error"

@ Execution was paused at a breakpoint
event SequenceBroken() \
    severity activity high \
    format "Wasm program paused at breakpoint"
