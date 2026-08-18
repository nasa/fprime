@ The interpreter store was allocated with capacity for the given number of modules
event StoreAllocationSucceeded(moduleCount: U16) \
  severity diagnostic \
  format "Successfully allocated store with {} modules"

@ A command was issued in a state where it is not valid
event ControllerBusy(
  signalSource: SignalSource
  $state: ControllerStateMachine.State
) \
  severity warning low \
  format "Controller cannot process signal {} in state {}"

@ Failed to find a loaded module with the requested name
event ModuleNotFound(module_name: string size 16) \
  severity warning low \
  format "Failed to invoke main on module with name {}"

@ A cancel was requested on the seqCancelIn port from a state where it is not valid
event InvalidSeqCancelCall($state: ControllerStateMachine.State) \
  severity warning high \
  format "Cannot cancel sequence from a port in state {}"

@ Failed to open the requested Wasm module file
event FileOpenError(fileName: string size FileNameStringSize, status: I32) \
  severity warning high \
  format "Failed to open Wasm module file {}: status {}"

@ The SEQ_BASE_DIR base directory joined with the requested sequence file path
@ exceeds the file-path buffer; the load will fail to open the truncated path.
event SequenceFilePathTooLong(
  baseDir: string size FileNameStringSize
  fileName: string size FileNameStringSize
) \
  severity warning high \
  format "Sequence file path too long: base dir {} + file {}"

@ A module failed to decode/validate while loading
event ModuleLoadFailed(status: Status) \
  severity warning high \
  format "Module load failed: {}"

@ Failed to invoke a module's main entrypoint
event ModuleMainInvokeFailed(status: Status) \
  severity warning high \
  format "Module main invoke failed: {}"

@ Failed to invoke a module's start function
event ModuleStartInvokeFailed(status: Status) \
  severity warning high \
  format "Module start invoke failed: {}"

@ A blocking command could not be queued because too many commands are already
@ waiting on the sequencer to finish
event TooManyBlockingCommands \
  severity warning high \
  format "Too many commands blocking on sequencer finish"

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
event HostFunctionInvalidPort(
  host_function: HostFunction
  index: I32
  numPorts: U32
) \
  severity warning high \
  format "Host function {} requested invalid serial port index {} (have {} ports)"

@ A guest serial port payload could not be serialized onto the serial output port
@ (the receiving port rejected the guest-supplied bytes)
event SerialPortSendFailed(host_function: HostFunction, status: I32) \
  severity warning high \
  format "Host function {} failed to send on serial port: serialize status {}"

@ The guest requested an event at a severity that is not permitted for guest
@ programs (an out-of-range id, or FATAL/COMMAND which are reserved for the
@ host). The guest message is preserved and reported here at WARNING_HI.
event HostFunctionInvalidSeverity(raw: I32, msg: string size 128) \
  severity warning high \
  format "Guest event at invalid/forbidden severity '{}': {}"

@ A sequence is starting after the main entrypoint was invoked
event SequenceStarting(index: ModuleIdx) \
  severity activity high \
  format "Sequence at index {} finished starting"

@ The interpreter ran a program to completion
event SequenceSucceeded(index: ModuleIdx) \
  severity activity high \
  format "Sequence at index {} finished successfully"

@ The sequence was cancelled
event SequenceCancelled(index: ModuleIdx) \
  severity activity high \
  format "Sequence at index {} was cancelled"

enum ExitReason: U8 {
  UNKNOWN              @< We did not explicitely set the exit reason, bug?
  INTERPRETER_FINISHED @< The interpreter exited normally and set m_exitCode
  INTERPRETER_TRAP     @< The interpreter ran into a byte-code trap
  HOST_EXIT            @< The interpreter exited via HOST_TRAP but a host function indicated it was a standard exit()
  HOST_PANIC           @< The interpreter exited via HOST_TRAP and we indicated HOST_PANIC in the host function
  REPLY_TIMEOUT        @< An async reply timeout overran while waiting for a host function reply
  HOST_FAILURE         @< An async host function replied with an error
  UNEXPECTED_REPLY     @< We got an unexpected async reply which caused us to abort
  CANCEL               @< A cancel signal was sent and we exited the interpreter
  TIMER_INCOMPARABLE   @< A sleep/statement timer became incomparable (time base changed) and could not be evaluated
}

@ The sequence failed to execute
event SequenceFailed(
  index: ModuleIdx
  exitReason: ExitReason
  exitCode: I32
  trapReason: TrapReason
  hostFunction: HostFunction
) \
  severity warning high \
  format "Sequence at index {} failed with reason={} code={} trap={} hostFn={}"

@ The sequence failed to execute start function
event SequenceStartFailed(
  index: ModuleIdx
  exitReason: ExitReason
  exitCode: I32
  trapReason: TrapReason
  hostFunction: HostFunction
) \
  severity warning high \
  format "Sequence at index {} failed start with reason={} code={} trap={} hostFn={}"

@ Execution was paused at a breakpoint
event SequencePaused() severity activity high format "Sequence execution paused"

@ No sequence is running so we cannot continue from a pause
event SequenceNotRunning() severity warning low format "Sequence cannot continue when interpreter is not running"

@ The guest program called fprime.exit with a non-zero code
event SequenceExited(code: I32) \
  severity warning high \
  format "Sequence exited with code {}"

@ The guest program called fprime.panic, or the interpreter itself panicked
event SequencePanic(code: I32) \
  severity warning high \
  format "Wasm program panicked: code {}"

@ The guest requested a sleep whose duration (in microseconds) does not fit the
@ host timer representation (its whole-seconds part overflows U32)
event SleepDurationTooLarge(host_function: HostFunction, micros: U64) \
  severity warning high \
  format "Host function {} requested a sleep of {} us that overflows the host timer"

@ A command response arrived carrying a context (cmdUid) from a previous
@ sequence run. Reported (not failed) because a late reply after CANCEL is nominal.
event CmdResponseFromOldSequence(
  $opcode: FwOpcodeType
  response: Fw.CmdResponse
  oldSequenceIdx: U16
  currentSequenceIdx: U16
) \
  severity warning low \
  format "Command response (opcode {}, response {}) was from a previous sequence (old idx {}, current idx {})"

@ A command response arrived for the correct opcode from this sequence, but for a
@ different dispatch instance than the one currently being awaited.
event WrongCmdResponseIndex(
  $opcode: FwOpcodeType
  response: Fw.CmdResponse
  actualCmdIdx: U16
  expectedCmdIdx: U16
) \
  severity warning high \
  format "Command response (opcode {}, response {}) was for a different command instance (actual idx {}, expected {})"

event InvalidModuleEntrypoint(moduleIdx: ModuleIdx, status: Status) \
  severity warning high \
  format "Module at index {} does not have a valid 'main' entrypoint: {}"

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
