@ Current executing state of the sequencer
telemetry State: SequencerStateMachine.State update on change

@ Sequences successfully completed
telemetry SequencesSucceeded: U64 update on change

@ Sequences that failed to validate or execute
telemetry SequencesFailed: U64 update on change

@ Sequences that were cancelled
telemetry SequencesCancelled: U64 update on change

@ Commands dispatched (successfully or otherwise) total.
@ Note this is distinct from the number of statements executed. This
@ number just tracks how many we've sent out
telemetry CommandsDispatched: U64 update on change

@ Number of commands that failed
telemetry CommandsFailed: U64 update on change

@ Number of commands that failed
telemetry LastTrapReason: TrapReason update on change

@ Currently running sequence name.
@ When executing sequence with `RUN` or `LOAD` the name will be the filename
@ without `.wasm`. When using `LOAD_NAME`, the name is provided by the user
telemetry SeqName: string size FileNameStringSize update on change


