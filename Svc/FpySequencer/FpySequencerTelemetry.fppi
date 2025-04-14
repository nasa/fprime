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

@ value of prm STATEMENT_TIMEOUT_SECS
telemetry STATEMENT_TIMEOUT_SECS: F32 update on change