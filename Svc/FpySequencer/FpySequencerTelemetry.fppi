@ the number of sequences successfully completed
telemetry SequencesSucceeded: U64 update always

@ the number of sequences that failed to validate or execute, or were cancelled
telemetry SequencesFailed: U64 update always

@ the number of statements successfully dispatched total
telemetry StatementsDispatched: U64 update always

@ the number of statements that failed to execute
telemetry StatementsFailed: U64 update always

@ the currently running sequence
telemetry SeqPath: string size FileNameStringSize update always

@ value of prm STATEMENT_TIMEOUT_SECS
telemetry STATEMENT_TIMEOUT_SECS: F32 update always