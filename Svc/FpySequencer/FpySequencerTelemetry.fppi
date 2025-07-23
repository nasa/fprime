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

@ the error code of the last directive that ran
telemetry LastDirectiveError: DirectiveErrorCode update on change

@ the currently running sequence
telemetry SeqPath: string size FileNameStringSize update on change

struct DebugTelemetry {
    @ true if there are no statements remaining in the sequence file
    reachedEndOfFile: bool
    @ true if we were able to deserialize the next statement successfully
    nextStatementReadSuccess: bool
    @ the opcode of the next statement to dispatch.
    nextStatementOpcode: U8
    @ if the next statement is a cmd directive, the opcode of that cmd
    nextCmdOpcode: FwOpcodeType
}

@ the current debug breakpoint index
telemetry DebugBreakpointIdx: U32 update on change

@ debug-only telemetry
@ only updated in the RUNNING.DEBUG_BROKEN state
telemetry Debug: DebugTelemetry update on change

@ value of prm STATEMENT_TIMEOUT_SECS
telemetry PRM_STATEMENT_TIMEOUT_SECS: F32 update on change
