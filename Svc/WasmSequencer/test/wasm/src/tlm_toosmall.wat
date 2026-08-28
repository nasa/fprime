;; Requests value_size 2, smaller than the 4-byte telemetry value the host returns.
;; Writing 4 bytes into a 2-byte guest request would either overrun guest intent or
;; force truncation, so dispatchTelemetry rejects it after pausing: BufferTooSmall(TELEMETRY, 2, 4)
;; -> interpreter_sendSignal_hostResponseFailure -> ExitReason::HOST_FAILURE -> SequenceHostFailure,
;; without writing anything to guest memory.
(module
  (import "fprime_v1" "tlm"
    (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 42   ;; channel id
    i32.const 0    ;; time_ptr
    i32.const 11   ;; time_size == SERIALIZED_SIZE
    i32.const 16   ;; value_ptr
    i32.const 2    ;; value_size < actual (4)
    call $tlm
    drop))
