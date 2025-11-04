@ the current state of the sequencer
telemetry State: FwEnumStoreType update on change

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
telemetry LastDirectiveError: Fpy.DirectiveErrorCode update on change

@ the index of the last directive to error
telemetry DirectiveErrorIndex: U64 update on change

@ the id of the last directive to error
telemetry DirectiveErrorId: Fpy.DirectiveId update on change

@ the currently running sequence
telemetry SeqPath: string size FileNameStringSize update on change

# debug telemetry
# only valid in RUNNING.PAUSED state
@ true if there are no statements remaining in the sequence file
telemetry Debug_ReachedEndOfFile: bool update on change
@ true if we were able to deserialize the next statement successfully
telemetry Debug_NextStatementReadSuccess: bool update on change
@ the opcode of the next statement to dispatch.
telemetry Debug_NextStatementOpcode: U8 update on change
@ if the next statement is a cmd directive, the opcode of that cmd
telemetry Debug_NextCmdOpcode: FwOpcodeType update on change
@ the size of the stack in bytes
telemetry Debug_StackSize: Fpy.StackSizeType update on change

# breakpoint telemetry
@ whether or not to break at the breakpoint index
telemetry BreakpointInUse: bool update on change
@ the current breakpoint index. The sequence will break at this point 
@ just before dispatching the directive at this index
telemetry BreakpointIndex: U32 update on change
@ whether or not to remove the breakpoint after breaking on it
telemetry BreakOnlyOnceOnBreakpoint: bool update on change
@ whether or not to break before dispatching the next line,
@ independent of what line it is.
@ can be used in combination with breakpointIndex
telemetry BreakBeforeNextLine: bool update on change

@ value of prm STATEMENT_TIMEOUT_SECS
telemetry PRM_STATEMENT_TIMEOUT_SECS: F32 update on change

@ value of prm FLAG_DEFAULT_EXIT_ON_CMD_FAIL
telemetry PRM_FLAG_DEFAULT_EXIT_ON_CMD_FAIL: bool update on change
