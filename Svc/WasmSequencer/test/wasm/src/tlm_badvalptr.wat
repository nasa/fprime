;; Reads tlm chan 42; the time pointer is valid (write succeeds) but the value
;; pointer is out of bounds so the SECOND mem_write (value) fails ->
;; HostFunctionInvalidPointer(TELEMETRY) + stmtResponse_failure.
(module
  (import "fprime_v1" "tlm" (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 42
    i32.const 0      ;; time_ptr (valid, 11 bytes fit in 64)
    i32.const 11     ;; time_size == SERIALIZED_SIZE
    i32.const 10000  ;; OOB value_ptr
    i32.const 4      ;; value_size
    call $tlm
    drop))
