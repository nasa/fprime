@ The interpreter store was allocated with capacity for the given number of modules
event StoreAllocationSucceeded(moduleCount: U16) \
    severity diagnostic \
    format "Successfully allocated store with {} modules"

@ A command was issued in a state where it is not valid
event InvalidCommand($state: SequencerStateMachine.State) \
    severity warning low \
    format "Command not valid in state {}"

@ A command was issued in a state where it is not valid
event ModuleNotFound(module_name: string size 16) \
    severity warning low \
    format "Failed to invoke main on module with name {}"

@ Failed to open the requested Wasm module file
event FileOpenError(fileName: string size FileNameStringSize, status: I32) \
    severity warning high \
    format "Failed to open Wasm module file {}: status {}"

@ The SEQ_BASE_DIR base directory joined with the requested sequence file path
@ exceeds the file-path buffer; the load will fail to open the truncated path.
event SequenceFilePathTooLong(baseDir: string size FileNameStringSize, fileName: string size FileNameStringSize) \
    severity warning high \
    format "Sequence file path too long: base dir {} + file {}"

@ A module failed to decode/validate while loading
event ModuleLoadFailed(status: Status) \
    severity warning high \
    format "Module load failed: {}"

@ A module failed to invoke a function
event ModuleInvokeFailed(status: Status) \
    severity warning high \
    format "Module main invoke failed: {}"

@ A blocking command could not be queued because too many commands are already
@ waiting on the sequencer to finish
event TooManyBlockingCommands \
    severity warning high \
    format "Too many commands blocking on sequencer finish"

@ The interpreter ran a program to completion
event SequenceSucceeded() \
    severity activity high \
    format "Wasm program completed successfully"

@ A guest host function supplied a buffer larger than the host maximum for that call
event BufferTooLarge(host_function: HostFunction, $size: U32, maxSize: U32) \
    severity warning high \
    format "Host function {} buffer payload size {} larger than host maximum size {}"

@ A guest host function supplied a buffer too small to hold the host value being returned
event BufferTooSmall(host_function: HostFunction, $size: U32, valueSize: U32) \
    severity warning high \
    format "Host function {} buffer payload size {} smaller than host value size {}"

event HostFunctionInvalidPointer(host_function: HostFunction, code: Status) \
    severity warning high \
    format "Failed to read guest pointer in host function {}: {}"

@ A guest serial port host function requested a port index outside the configured range
event HostFunctionInvalidPort(host_function: HostFunction, index: I32, numPorts: U32) \
    severity warning high \
    format "Host function {} requested invalid serial port index {} (have {} ports)"

@ The guest requested an event at a severity that is not permitted for guest
@ programs (an out-of-range id, or FATAL/COMMAND which are reserved for the
@ host). The guest message is preserved and reported here at WARNING_HI.
event HostFunctionInvalidSeverity(raw: I32, msg: string size 128) \
    severity warning high \
    format "Guest event at invalid/forbidden severity '{}': {}"

@ The sequence was cancelled
event SequenceCancelled() severity activity high format "Wasm program cancelled"

@ The sequence failed to execute successfully
event SequenceFailed() severity warning high format "Wasm program failed"

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
event GuestMemoryError(status: Status) \
    severity warning high \
    format "Guest memory access failed: spacewasm status {}"

@ A command response arrived carrying a context (cmdUid) from a previous
@ sequence run. Reported (not failed) because a late reply after CANCEL is nominal.
event CmdResponseFromOldSequence($opcode: FwOpcodeType, response: Fw.CmdResponse, oldSequenceIdx: U16, currentSequenceIdx: U16) \
    severity warning low \
    format "Command response (opcode {}, response {}) was from a previous sequence (old idx {}, current idx {})"

@ A command response arrived for the correct opcode from this sequence, but for a
@ different dispatch instance than the one currently being awaited.
event WrongCmdResponseIndex($opcode: FwOpcodeType, response: Fw.CmdResponse, actualCmdIdx: U16, expectedCmdIdx: U16) \
    severity warning high \
    format "Command response (opcode {}, response {}) was for a different command instance (actual idx {}, expected {})"

# ----------------------------------------------------------------------
# Guest-emitted events (fprime.event host function). One event per guest-
# permitted F´ severity; the host function switches on the requested
# severity id to emit the matching one so the event carries the correct
# FPP severity. FATAL and COMMAND are intentionally NOT guest-emittable
# (FATAL would let untrusted guest code trigger the FatalHandler; COMMAND
# is reserved for the command dispatcher). A guest request for either, or
# for an out-of-range id, is reported via HostFunctionInvalidSeverity.
# ----------------------------------------------------------------------

@ A WARNING_HI event emitted by the guest program
event LogWarningHi(msg: string size 128) severity warning high format "{}"

@ A WARNING_LO event emitted by the guest program
event LogWarningLo(msg: string size 128) severity warning low format "{}"

@ An ACTIVITY_HI event emitted by the guest program
event LogActivityHi(msg: string size 128) severity activity high format "{}"

@ An ACTIVITY_LO event emitted by the guest program
event LogActivityLo(msg: string size 128) severity activity low format "{}"

@ A DIAGNOSTIC event emitted by the guest program
event LogDiagnostic(msg: string size 128) severity diagnostic format "{}"
