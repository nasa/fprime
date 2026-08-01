;; Reads tlm chan 42; the time pointer is out of bounds so the FIRST mem_write
;; (time) fails -> HostFunctionInvalidPointer(TELEMETRY) + stmtResponse_failure.
;; time_size == SERIALIZED_SIZE (11) so the size assert passes.
(module
  (import "fprime_v1" "tlm" (func $tlm (param i64 i32 i32 i32 i32) (result i32)))
  (memory 64 (pagesize 1))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (func $main
    i64.const 42
    i32.const 10000  ;; OOB time_ptr
    i32.const 11     ;; time_size == SERIALIZED_SIZE
    i32.const 16     ;; value_ptr (valid)
    i32.const 4      ;; value_size
    call $tlm
    drop))
