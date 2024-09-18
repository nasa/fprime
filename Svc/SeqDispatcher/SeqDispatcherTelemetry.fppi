@ Number of sequences dispatched
telemetry dispatchedCount: U32
@ Number of sequences dispatched that returned an error. Note: if a sequence
@ was run in non-blocking mode, even if the sequence errors out, this error
@ count will never increase
telemetry errorCount: U32
@ Number of sequencers in an available state
telemetry sequencersAvailable: U32