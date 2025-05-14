@ the current state of the sequencer
telemetry State: I32 update on change

@ the number of sequences successfully completed
telemetry SequencesSucceeded: U64 update on change

@ the number of sequences that failed to validate or execute
telemetry SequencesFailed: U64 update on change

@ the number of sequences that were cancelled
telemetry SequencesCancelled: U64 update on change

@ the number of statements dispatched (successfully or otherwise) total.
@ Note this is distinct from the number of statements executed. This
@ number just tracks how many we've sent out
telemetry StatementsDispatched: U64 update on change

@ the number of statements that failed to execute
telemetry StatementsFailed: U64 update on change

@ the currently running sequence
telemetry SeqPath: string size FileNameStringSize update on change

struct DebugTelemetry {
    @ true if there are no statements remaining in the sequence file
    reachedEndOfFile: bool
    @ the opcode of the next statement to dispatch.
    nextStatementOpcode: U32
    @ the type of the next statement to dispatch.
    nextStatementType: Fpy.StatementType
}

@ the current debug breakpoint index
telemetry DebugBreakpointIdx: U32 update on change

@ debug-only telemetry
@ only updated in the RUNNING.DEBUG_BROKEN state
telemetry Debug: DebugTelemetry update on change

@ value of prm STATEMENT_TIMEOUT_SECS
telemetry STATEMENT_TIMEOUT_SECS: F32 update on change