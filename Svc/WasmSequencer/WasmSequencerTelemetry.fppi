@ Current executing state of the sequencer controller
telemetry ControllerState: ControllerStateMachine.State update on change

@ Current executing state of the WebAssembly interpreter engine
telemetry InterpreterState: InterpreterStateMachine.State update on change

@ Sequences successfully completed
telemetry SequencesSucceeded: U64 update on change

@ Sequences that failed to validate or execute
telemetry SequencesFailed: U64 update on change

@ Sequences that were cancelled
telemetry SequencesCancelled: U64 update on change

@ Commands dispatched total
telemetry CommandsDispatched: U64 update on change

@ Number of commands that failed
telemetry CommandsFailed: U64 update on change

@ Reason last sequence trapped
telemetry LastTrapReason: TrapReason update on change

@ Currently running sequence name.
@ When a module is loaded under an empty name (`RUN`, or `LOAD` with no name) the
@ name is the filename without `.wasm`; otherwise it is the module name provided to `LOAD`
telemetry SeqName: string size FileNameStringSize update on change
